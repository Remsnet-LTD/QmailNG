#include <sys/types.h>
#include <unistd.h>
#include "byte.h"
#include "error.h"
#include "localdelivery.h"
#include "locallookup.h"
#include "output.h"
#include "pbsexec.h"
#include "qldap.h"
#include "qldap-debug.h"
#include "qldap-errno.h"
#include "read-ctrl.h"
#include "readwrite.h"
#include "stralloc.h"
#ifdef QLDAP_CLUSTER
#include <sys/socket.h>
#include "dns.h"
#include "ipalloc.h"
#include "ipme.h"
#include "ndelay.h"
#include "qldap-cluster.h"
#include "select.h"
#include "timeoutconn.h"
#endif
#ifdef AUTOHOMEDIRMAKE
#include "dirmaker.h"
#endif
#ifdef AUTOMAILDIRMAKE
#include "mailmaker.h"
#endif


#include "checkpassword.h"
#include "auth_mod.h"

stralloc	loginstr = {0};
stralloc	authdatastr = {0};

ctrlfunc	ctrls[] = {
		qldap_controls,
		localdelivery_init,
#ifdef QLDAP_CLUSTER
		cluster_init,
#endif
#ifdef AUTOHOMEDIRMAKE
		dirmaker_init,
#endif
		0 };

checkfunc	cfuncs[] = {
	check_ldap,
	check_passwd,
	0
};

void chdir_or_make(char *, char *);

#ifdef QLDAP_CLUSTER
void forward(char *, char *, struct credentials *);
#endif

int
main(int argc, char **argv)
{
	struct	credentials c;
	int r;

	log_init(STDERR, ~256, 0);	/* XXX limited so that it is not
					   possible to get passwords via
					   debug on production systems.
					 */
	if (read_controls(ctrls) == -1)
		auth_error(AUTH_CONF);

	auth_init(argc, argv, &loginstr, &authdatastr);
	log(256, "auth_init: login=%s, authdata=%s\n",
	    loginstr.s, authdatastr.s);

	if (authdatastr.len <= 1) {
		log(1, "alert: null password.\n");
		auth_fail(loginstr.s, BADPASS);
	}

	byte_zero(&c, sizeof(c));
	r = check(cfuncs, &loginstr, &authdatastr, &c, 0);
	switch (r) {
	case OK:
		/* authdata no longer needed */
		byte_zero(authdatastr.s, authdatastr.len);
		change_uid(c.uid, c.gid);
		setup_env(loginstr.s, &c);
		if (c.maildir.s && *c.maildir.s) {
			/* use default maildir aka aliasempty or
			   in other words the last argv */
			if (!stralloc_copys(&c.maildir, argv[argc-1]))
				auth_error(ERRNO);
			if (!stralloc_0(&c.maildir))
				auth_error(ERRNO);
		}
		chdir_or_make(c.home.s, c.maildir.s);
		auth_success(loginstr.s);
	case FORWARD:
#ifdef QLDAP_CLUSTER
		change_uid(-1, -1);
		setup_env(loginstr.s, &c);
		forward(loginstr.s, authdatastr.s, &c);
		/* does not return */
#else
		/* authdata no longer needed */
		byte_zero(authdatastr.s, authdatastr.len);
		/* system error, now way out ... module likes to forward
		   but we don't have support for it. */
		auth_error(r);
#endif
	case NOSUCH: /* FALLTHROUGH */
	case BADPASS:
		/* authdata no longer needed */
		byte_zero(authdatastr.s, authdatastr.len);
		auth_fail(loginstr.s, r);
	default:
		/* authdata no longer needed */
		byte_zero(authdatastr.s, authdatastr.len);
		/* system error, now way out ... */
		auth_error(r);
	}

	auth_error(PANIC);
	return 1; /* should never get here */
}

void
chdir_or_make(char *home, char *maildir)
{
	char	*md;

	if (maildir == (char *)0)
		md = auth_aliasempty();
	else
		md = maildir;

	/* ... go to home dir and create it if needed */
	if (chdir(home) == -1) {
#ifdef AUTOHOMEDIRMAKE
		log(8, "makeing homedir for %s %s\n", home, md);

		switch (dirmaker_make(home, md)) {
		case OK:
			break;
		case MAILDIR_CRASHED:
			log(2, "warning: dirmaker failed: program crashed\n");
			auth_error(MAILDIR_FAILED);
		case MAILDIR_FAILED:
			log(2, "warning: dirmaker failed: bad exit status\n");
			auth_error(MAILDIR_FAILED);
		case MAILDIR_UNCONF:
			log(2, "warning: dirmaker failed: not configured\n");
			auth_error(MAILDIR_NONEXIST);
		case MAILDIR_HARD:
			log(2, "warning: dirmaker failed: hard error\n");
		case ERRNO:
		default:
			log(2, "warning: dirmaker failed (%s)\n",
			    error_str(errno));
			auth_error(MAILDIR_FAILED);
		}
		if (chdir(home) == -1) {
			log(2, "warning: 2nd chdir failed: %s\n",
			    error_str(errno));
			auth_error(MAILDIR_FAILED);
		}
		log(32, "homedir successfully made\n");
#else
		log(2, "warning: chdir failed: %s\n", error_str(errno));
		auth_error(MAILDIR_NONEXIST);
#endif
	}
#ifdef AUTOMAILDIRMAKE
	switch (maildir_make(md)) {
	case OK:
		break;
	case MAILDIR_CORRUPT:
		log(2, "warning: maildir_make failed (%s)\n",
		    "maildir seems to be corrupt");
		auth_error(MAILDIR_CORRUPT);
	case ERRNO:
	default:
		log(2, "warning: maildir_make failed (%s)\n",
		    error_str(errno));
		auth_error(MAILDIR_FAILED);
	}
#endif
}

#ifdef QLDAP_CLUSTER
static int allwrite(int (*)(),int, char *,int);
static void copyloop(int, int, int);
static char copybuf[4096];

static int
allwrite(int (*op)(),int fd, char *buf,int len)
{
	int	w;

	while (len) {
		w = op(fd,buf,len);
		if (w == -1) {
			if (errno == error_intr) continue;
			return -1;
		}
		if (w == 0) ; /* luser's fault */
		buf += w;
		len -= w;
	}
	return 0;
}

static void
copyloop(int infd, int outfd, int timeout)
{
	fd_set	iofds;
	struct	timeval tv;
	int	maxfd;	/* Maximum numbered fd used */
	int	bytes, ret;

	ndelay_off(infd); ndelay_off(outfd);
	while (1) {
		/* file descriptor bits */
		FD_ZERO(&iofds);
		maxfd = -1;
		FD_SET(infd, &iofds);
		if (infd > maxfd)
			maxfd = infd;
		FD_SET(outfd, &iofds);
		if (outfd > maxfd)
			maxfd = outfd;

		/* Set up timeout */
		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		ret = select(maxfd + 1, &iofds, (fd_set *)0, (fd_set *)0, &tv);
		if (ret == -1) {
			log(1, "copyloop: select failed %s\n",
			    error_str(errno));
			break;
		} else if (ret == 0) {
			log(32, "copyloop: select timeout\n");
			break;
		}
		if (FD_ISSET(infd, &iofds)) {
			if ((bytes = read(infd, copybuf,
					    sizeof(copybuf))) < 0) {
				log(1, "copyloop: read failed: %s\n",
				    error_str(errno));
				break;
			}
			if (bytes == 0)
				break;
			if (allwrite(subwrite, outfd, copybuf, bytes) != 0) {
				log(1, "copyloop: write out failed: %s\n",
				    error_str(errno));
				break;
			}
		}
		if (FD_ISSET(outfd, &iofds)) {
			if ((bytes = read(outfd, copybuf,
					    sizeof(copybuf))) < 0) {
				log(1, "copyloop: read failed: %s\n",
				    error_str(errno));
				break;
			}
			log(32, "copyloop: read in %i bytes read\n", bytes);
			if (bytes == 0)
				break;
			if (allwrite(subwrite, infd, copybuf, bytes) != 0) {
				log(1, "copyloop: write in failed: %s\n",
				    error_str(errno));
				break;
			}
		}
	}
	close(infd);
	close(outfd);
	return;
}

void
forward(char *name, char *passwd, struct credentials *c)
{
	struct	ip_address outip;
	ipalloc	ip = {0};
	int	ffd;
	int	timeout = 31*60; /* ~30 min timeout RFC1730 */
	int	ctimeout = 30;

	/* pop befor smtp */
	pbsexec();

	if (!ip_scan("0.0.0.0", &outip))
		auth_error(ERRNO);

	dns_init(0);
	switch (dns_ip(&ip,&c->forwarder)) {
		case DNS_MEM:
			auth_error(ERRNO);
		case DNS_SOFT:
		case DNS_HARD:
			auth_error(BADCLUSTER);
		case 1:
			if (ip.len <= 0)
				auth_error(BADCLUSTER);
	}
	/*
	   20010523 Don't check if only one IP is returned, so it is
	   possible to have a cluster node consisting of multiple machines.
	   XXX If your mailhost is bad (bad entries in ldap) you will get
	   bad loops, the only limit is the tcpserver concurrency limit.
	   20030627 Could we use the ipme stuff of qmail-remote, to make
	   single hop loops impossible? Let's try it.
	 */
	if (ipme_is(&ip.ix[0].ip) == 1)
		auth_error(BADCLUSTER);

	ffd = socket(AF_INET, SOCK_STREAM, 0);
	if (ffd == -1)
		auth_error(ERRNO);

	if (timeoutconn(ffd, &ip.ix[0].ip, &outip, auth_port, ctimeout) != 0)
		auth_error(ERRNO);

	/* We have a connection, first send user and pass */
	auth_forward(ffd, name, passwd);
	copyloop(0, ffd, timeout);

	_exit(0); /* all went ok, exit normaly */
}

#endif /* QLDAP_CLUSTER */