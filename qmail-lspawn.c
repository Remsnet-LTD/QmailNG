#include "fd.h"
#include "wait.h"
#include "prot.h"
#include "substdio.h"
#include "stralloc.h"
#include "scan.h"
#include "exit.h"
#include "fork.h"
#include "error.h"
#include "cdb.h"
#include "case.h"
#include "slurpclose.h"
#include "auto_qmail.h"
#include "auto_uids.h"
#include "qlx.h"

#define QLDAP           /* enable LDAP for qmail */
// #define QLDAPDEBUG     /* We should set this with the -D option of gcc */

#ifdef QLDAP /* Includes needed to make LDAP work */

#define QLDAP_PORT LDAP_PORT
#include "control.h"
#include "env.h"
#include <lber.h>
#include <ldap.h>
#include "auto_usera.h"
#include "auto_uids.h"
#include "fmt.h"
#include "check.h"

#endif /* end -- Includes needed to make LDAP work */

char *aliasempty;

#ifdef QLDAPDEBUG /* Teststring for debugging */

int qldap_get( stralloc *mail );
char sentest[]="nobody@nowhere.com";
char rcptest[]="opi@opi.flirtbox.ch";

#endif /* end -- Teststring for debugging */


#ifdef QLDAP /* routine to read the control files */

/* initialize the string arrays, this uses DJB's libs */
stralloc    qldap_server = {0};
stralloc    qldap_basedn = {0};
stralloc    qldap_user = {0};
stralloc    qldap_password = {0};
stralloc    qldap_defdotmode = {0};
int         qldap_localdelivery = 1;

#ifdef QLDAPDEBUG
stralloc    ra = {0}; /* temporary definition for debug purposes */
#endif
/* init done */

/* read the various LDAP control files */
void get_qldap_controls()
{
   stralloc qldap_defaultquota = {0};

   if (control_rldef(&qldap_server,"../../control/ldapserver",0,(char *) 0) != 1) {
      printf("unable to read control/ldapserver\n");
      _exit(200);
   }
   if (!stralloc_0(&qldap_server)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_basedn,"../../control/ldapbasedn",0,"") != 1);
   if (!stralloc_0(&qldap_basedn)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_user,"../../control/ldapuser",0,"") != 1);
   if (!stralloc_0(&qldap_user)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_password,"../../control/ldappassword",0,"") != 1);
   if (!stralloc_0(&qldap_password)) _exit(QLX_NOMEM);

   if (control_readint(&qldap_localdelivery,"../../control/ldaplocaldelivery") != 1);

   if (control_rldef(&qldap_defaultquota,"../../control/ldapdefaultquota",0,"0") != 1) {
      printf("unable to read control/ldapdefaultquota\n");
      _exit(200);
   }
   if (!stralloc_0(&qldap_defaultquota)) _exit(QLX_NOMEM);
   if (!env_put2("QMAILQUOTA", qldap_defaultquota.s)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_defdotmode,"../../control/ldapdefaultdotmode",0,"1") != 1);
   if (!stralloc_0(&qldap_defdotmode)) _exit(QLX_NOMEM);

/* reading of the various LDAP control files done */

#ifdef QLDAPDEBUG /* print the read values from the control files */
   if ( qldap_server.s == 0 )
      printf("ldapserver = \\0\n");
   else
      printf("ldapserver = %s\n", qldap_server.s);
   if ( qldap_basedn.s == 0 )
      printf("ldapbasedn = \\0\n");
   else
      printf("ldapbasedn = %s", qldap_basedn.s);
   if ( qldap_user.s == 0 )
      printf("ldapuser = \\0\n");
   else
      printf("ldapuser = %s", qldap_user.s);
   if ( qldap_password.s == 0 )
      printf("ldappassword = \\0\n");
   else
      printf("ldappassword = %s", qldap_password.s);
   printf("ldaplocaldelivery = %i\n", qldap_localdelivery);
   printf("ldapdefaultquota= %s\n", qldap_defaultquota.s);
   printf("ldapdefaultdotmode= %s\n", qldap_defdotmode.s);

   if (!stralloc_copys(&ra, "opi@opi.flirtbox.ch")) _exit(QLX_NOMEM);
//   printf("qldap_get returned: %i\n", qldap_get(&ra) );

   spawn(3,3,sentest,rcptest,3);
#endif
}

#endif /* end -- routine to read the control files */

void initialize(argc,argv)
int argc;
char **argv;
{
   aliasempty = argv[1];
   if (!aliasempty) {

#ifdef QLDAPDEBUG
      printf("alias empty\n");
#endif

   _exit(100);
   }

#ifdef QLDAP /* read the control files */
   get_qldap_controls();
#endif
}

int truncreport = 3000;

void report(ss,wstat,s,len)
substdio *ss;
int wstat;
char *s;
int len;
{
   int i;
   if (wait_crashed(wstat)) {
      substdio_puts(ss,"Zqmail-local crashed.\n");
      return;
   }
   switch(wait_exitcode(wstat)) {
      case QLX_CDB:
         substdio_puts(ss,"ZTrouble reading users/cdb in qmail-lspawn.\n");
      return;

      case QLX_NOMEM:
         substdio_puts(ss,"ZOut of memory in qmail-lspawn.\n");
      return;

      case QLX_SYS:
         substdio_puts(ss,"ZTemporary failure in qmail-lspawn.\n");
      return;

      case QLX_NOALIAS:
         substdio_puts(ss,"ZUnable to find alias user!\n");
      return;

      case QLX_ROOT:
         substdio_puts(ss,"ZNot allowed to perform deliveries as root.\n");
      return;

      case QLX_USAGE:
         substdio_puts(ss,"ZInternal qmail-lspawn bug.\n");
      return;

      case QLX_USAGE1:
         substdio_puts(ss,"ZInternal qmail-lspawn ldap structure bug.\n");
      return;

      case QLX_NFS:
         substdio_puts(ss,"ZNFS failure in qmail-local.\n");
      return;

      case QLX_EXECHARD:
         substdio_puts(ss,"DUnable to run qmail-local.\n");
      return;

      case QLX_EXECSOFT:
         substdio_puts(ss,"ZUnable to run qmail-local.\n");
      return;

      case QLX_EXECPW:
         substdio_puts(ss,"ZUnable to run qmail-getpw.\n");
      return;

      case 111: case 71: case 74: case 75:
         substdio_put(ss,"Z",1);
      break;

      case 0:
         substdio_put(ss,"K",1);
      break;

#ifdef QLDAP /* report LDAP errors */
      case 200:
         substdio_puts(ss, "ZError in get_qldap_controls (control-file missing ?).\n");
      return;

      case 201:
         substdio_puts(ss, "ZFailed to init LDAP-connection: Bad Host or Port.\n");
      return;

      case 202:
         substdio_puts(ss, "Dldap_set_option failed\n");
      return;

      case 203:
         substdio_puts(ss, "ZFailed to connect the LDAP-Server\n");
      return;

      case 204:
         substdio_puts(ss, "Zldap_search_ext_s returned an error\n");
      return;

      case 205:
         substdio_puts(ss, "DThe email address contains illegal characters for LDAP\n");
      return;

      case 206:
         substdio_puts(ss, "ZLDAP-database error\n");
      return;

#endif /* end -- report LDAP errors */

      case 100:
      default:
         substdio_put(ss,"D",1);
      break;
   }

   for (i = 0;i < len;++i)
      if (!s[i])
         break;

   substdio_put(ss,s,i);
}


stralloc nughde = {0};

#ifdef QLDAP /* LDAP server query routines */

int qldap_get( stralloc *mail )
{
   LDAP           *ld;
   LDAPMessage    *res, *msg;
   char           *dn,
                  **vals;
   int            version,
                  rc, i,
                  num_entries = 0;

   stralloc       filter = {0};

   /* check the mailaddress for illegal characters       *
    * only [a-z][0-9][.-_]@[a-z][0-9][.-] are allowed    *
    * because all other stuff would kill the LDAP search */
   if ( !chck_mailb(mail->s, mail->len) ) return 15;

   /* initialize the LDAP connection and get a handle */
   if ( (ld = ldap_init(qldap_server.s,QLDAP_PORT)) == NULL ) return 11;

   /* set LDAP connection options (only with Mozilla LDAP SKD) */
#ifdef LDAP_OPT_PROTOCOL_VERSION
   version = LDAP_VERSION2;
   if ( ldap_set_option(ld,LDAP_OPT_PROTOCOL_VERSION,&version) != LDAP_SUCCESS ) return 12;
#endif

   /* connect to the LDAP server */
   if ( (rc = ldap_simple_bind_s(ld,qldap_user.s,qldap_password.s)) != LDAP_SUCCESS ) {
#ifdef QLDAPDEBUG
      printf("exit ldap %s\n", ldap_err2string(rc));
#endif
       return 13;
   }

   /* build the search string for the email address */
   if (!stralloc_copys(&filter,"(|(mail=" ) ) _exit(QLX_NOMEM);
   if (!stralloc_cat(&filter,mail)) _exit(QLX_NOMEM);
   if (!stralloc_cats(&filter,")(mailalternateaddress=")) _exit(QLX_NOMEM);
   if (!stralloc_cat(&filter,mail)) _exit(QLX_NOMEM);
   if (!stralloc_cats(&filter,"))")) _exit(QLX_NOMEM);
   if (!stralloc_0(&filter)) _exit(QLX_NOMEM);

#ifdef QLDAPDEBUG
   printf("filter: %s\n", filter.s);
#endif

   /* do the search for the email address */
   if ( (rc = ldap_search_ext_s(ld,qldap_basedn.s,LDAP_SCOPE_SUBTREE,filter.s,NULL,0,NULL,NULL,NULL,1,&res)) != LDAP_SUCCESS ) {
#ifdef QLDAPDEBUG
      printf("exit ldap %s\n", ldap_err2string(rc));
#endif
      return 14;
   }

   /* count the results, we must have least one */
   if ( (num_entries = ldap_count_entries(ld,res)) != 1) return 1;

   /* go to the first entry */
   msg = ldap_first_entry(ld,res);

   /* get the dn and free it (we dont need it, to prevent memory leaks) */
   if ( (dn = ldap_get_dn(ld,msg)) != NULL ) ldap_memfree(dn);

   /* go through the attributes and set the proper args for qmail-local  *
    * this can probably done with some sort of loop, but hey, how cares? */

   /* get the username for delivery on the local system */
   if ( (vals = ldap_get_values(ld,msg,"qmailUser")) != NULL ) {
      /* set the value for qmail-local... */
      if (!stralloc_copys(&nughde, vals[0])) _exit(QLX_NOMEM);

#ifdef QLDAPDEBUG
      printf("qmailUser: %s\n", vals[0]);
#endif
      if (!chck_users(vals[0]) ) return 16;
   } else {
      /* ...or set the default one (or break) */
      return 1;
   }
   ldap_value_free(vals);

   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);

   /* get the UID for delivery on the local system */
   if ( (vals = ldap_get_values(ld,msg,"qmailUID")) != NULL ) {
      if (!stralloc_cats(&nughde, vals[0])) _exit(QLX_NOMEM);
#ifdef QLDAPDEBUG
      printf("qmailUID: %s\n", vals[0]);
#endif
      if (100 > chck_ids(vals[0]) ) return 15;
   } else {
      return 1;
   }
   ldap_value_free(vals);

   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);

   /* get the GID for delivery on the local system */
   if ( (vals = ldap_get_values(ld,msg,"qmailGID")) != NULL ) {
      if (!stralloc_cats(&nughde, vals[0])) _exit(QLX_NOMEM);
#ifdef QLDAPDEBUG
      printf("qmailGID: %s\n", vals[0]);
#endif
      if ( 100 > chck_ids(vals[0]) ) return 16;
   } else {
      return 1;
   }
   ldap_value_free(vals);

   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);

   /* get the path of the maildir or mbox */
   if ( (vals = ldap_get_values(ld,msg,"mailMessageStore")) != NULL ) {
      if (!stralloc_cats(&nughde, vals[0])) _exit(QLX_NOMEM);
#ifdef QLDAPDEBUG
      printf("mailMessageStore: %s\n", vals[0]);
#endif
      if (!chck_paths(vals[0]) ) return 16;
   } else {
      return 2;
   }
   ldap_value_free(vals);

   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);

   /* At the moment we ignore the dash-field and the extension field *
    * so we fill up the nughde structure with '\0'                   */

   if (!stralloc_cats(&nughde, "")) _exit(QLX_NOMEM);
   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
   if (!stralloc_cats(&nughde, "")) _exit(QLX_NOMEM);
   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
   if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);

#ifdef QLDAPDEBUG
   printf("No of chars: %i\n", nughde.len);
#endif

   /* get the quota for the user of that maildir mbox */
   if ( (vals = ldap_get_values(ld,msg,"mailQuota")) != NULL ) {
      /* set the environment variable for qmail-local */
      if ( !env_put2("QMAILQUOTA", vals[0] ) ) _exit(QLX_NOMEM);
   }
   ldap_value_free(vals);

   /* get the forwarding addresses and build a list *
    * equals to &jdoe@heaven.af.mil in .qmail       */
   if ( (vals = ldap_get_values(ld,msg,"forwardingAddress")) != NULL ) {
      stralloc forwarders = {0};
      for ( i = 0; vals[i] != NULL; i++ ) {
         /* append */
         if (!chck_mails(vals[i]) ) continue;
         if (!stralloc_cats(&forwarders, vals[i])) _exit(QLX_NOMEM);
         if (vals[i+1] == NULL ) break;
         if (!stralloc_cats(&forwarders, ",") ) _exit(QLX_NOMEM);
      }
      if (!stralloc_0(&forwarders) ) _exit(QLX_NOMEM);
      if ( !env_put2("QMAILFORWARDS", forwarders.s) ) _exit(QLX_NOMEM);
   } else {
      /* default */
      if ( !env_unset("QMAILFORWARDS") ) _exit(QLX_NOMEM);
   }
   ldap_value_free(vals);

   /* get the path of the local delivery program *
    * equals to |/usr/bin/program in .qmail      */
   if ( (vals = ldap_get_values(ld,msg,"deliveryProgramPath")) != NULL ) {
      stralloc progpath = {0};
      stralloc_copys(&progpath, "");
      for ( i = 0; vals[i] != NULL; i++ ) {
         /* append */
         if (!chck_paths(vals[i]) ) continue;
         if (!stralloc_cats(&progpath, vals[i])) _exit(QLX_NOMEM);
         if (vals[i+1] == NULL ) break;
         if (!stralloc_cats(&progpath, ",") ) _exit(QLX_NOMEM);
      }
      if (!stralloc_0(&progpath) ) _exit(QLX_NOMEM);
      if ( !env_put2("QMAILDELIVERYPROGRAM", progpath.s) ) _exit(QLX_NOMEM);
   } else {
      /* default */
      if ( !env_unset("QMAILDELIVERYPROGRAM") ) _exit(QLX_NOMEM);
   }
   ldap_value_free(vals);

   /* get the deliverymode of the mailbox: reply, echo, forwardonly */
   if ( (vals = ldap_get_values(ld,msg,"mailMode")) != NULL ) {
      case_lowers(vals[0]);
      if ( !str_diff("reply", vals[0]) ) {
         if ( !env_put2("QMAILMODE", "reply") ) _exit(QLX_NOMEM);
            ldap_value_free(vals);
            if ( (vals = ldap_get_values(ld,msg,"mailReplyText")) != NULL )
               if ( !env_put2("QMAILREPLYTEXT", vals[0]) ) _exit(QLX_NOMEM);
      } else if ( !str_diff("echo", vals[0]) ) {
         if ( !env_put2("QMAILMODE", "echo") ) _exit(QLX_NOMEM);
         if ( !env_unset("QMAILREPLYTEXT") ) _exit(QLX_NOMEM);
      } else if ( !str_diff("forwardonly", vals[0]) ) {
         if ( !env_put2("QMAILMODE", "forwardonly") ) _exit(QLX_NOMEM);
         if ( !env_unset("QMAILREPLYTEXT") ) _exit(QLX_NOMEM);
      } else {
         if ( !env_unset("QMAILMODE") ) _exit(QLX_NOMEM);
         if ( !env_unset("QMAILREPLYTEXT") ) _exit(QLX_NOMEM);
      }
   } else {
      /* default */
      if ( !env_unset("QMAILMODE") ) _exit(QLX_NOMEM);
      if ( !env_unset("QMAILREPLYTEXT") ) _exit(QLX_NOMEM);
   }
   ldap_value_free(vals);

   /* get the mode of the .qmail interpretion: on, off, both */
   if ( (vals = ldap_get_values(ld,msg,"qmailDotMode")) != NULL ) {
      case_lowers(vals[0]);
      if ( !str_diff("envonly", vals[0]) ) {
         if ( !env_put2("QMAILDOTMODE", "envonly") ) _exit(QLX_NOMEM);
      } else if ( !str_diff("dotonly", vals[0]) ) {
         if ( !env_put2("QMAILDOTMODE", "dotonly") ) _exit(QLX_NOMEM);
      } else if ( !str_diff("both", vals[0]) ) {
         if ( !env_put2("QMAILDOTMODE", "both") ) _exit(QLX_NOMEM);
      } else if ( !str_diff("none", vals[0]) ) {
         if ( !env_put2("QMAILDOTMODE", "none") ) _exit(QLX_NOMEM);
      } else {
         if ( !env_put2("QMAILDOTMODE", qldap_defdotmode.s) ) _exit(QLX_NOMEM);
      }
   } else {
      /* default */
      if ( !env_put2("QMAILDOTMODE", qldap_defdotmode.s) ) _exit(QLX_NOMEM);
   }
   ldap_value_free(vals);

  /* list of attribs to implement: */
   /*
   mailMode one def (forwardonly|reply|echo|normal) ok
   mailReplyText one ok
   mailForwardingAddress more ok
   mailProgramDeliveryPath one ok
   mailHost one req def(me) (don't know if this is cool)
   *mailMessagestore one req ok
   mailQuota one def ok
   *qmailUser one req def ok
   *qmailUID one req def ok
   *qmailGID one req def ok
   uid one req (POP3/IMAP)
   userPassword one req (POP3/IMAP) ({SHA|MD5|DES|...}password)
   */
   /* thats all */

   /* ok, we finished, lets clean up and *
    * disconnect from the LDAP server    */
   ldap_unbind_s(ld);
   return(0);
}
#endif /* end -- LDAP server query routines */

stralloc lower = {0};
stralloc wildchars = {0};

void nughde_get(local)
char *local;
{
   char *(args[3]);
   int   pi[2],
         gpwpid,
         gpwstat,
         r,
         fd,
         flagwild;

   if (!stralloc_copys(&lower,"!")) _exit(QLX_NOMEM);
   if (!stralloc_cats(&lower,local)) _exit(QLX_NOMEM);
   if (!stralloc_0(&lower)) _exit(QLX_NOMEM);
   case_lowerb(lower.s,lower.len);

   if (!stralloc_copys(&nughde,"")) _exit(QLX_NOMEM);

   fd = open_read("users/cdb");
   if (fd == -1)
      if (errno != error_noent)
         _exit(QLX_CDB);

   if (fd != -1) {
      uint32         dlen;
      unsigned int   i;

      r = cdb_seek(fd,"",0,&dlen);
      if (r != 1) _exit(QLX_CDB);
      if (!stralloc_ready(&wildchars,(unsigned int) dlen)) _exit(QLX_NOMEM);
      wildchars.len = dlen;
      if (cdb_bread(fd,wildchars.s,wildchars.len) == -1) _exit(QLX_CDB);

      i = lower.len;
      flagwild = 0;

      do {
         /* i > 0 */
         if (!flagwild || (i == 1) || (byte_chr(wildchars.s,wildchars.len,lower.s[i - 1]) < wildchars.len)) {
            r = cdb_seek(fd,lower.s,i,&dlen);
            if (r == -1) _exit(QLX_CDB);
            if (r == 1) {
               if (!stralloc_ready(&nughde,(unsigned int) dlen)) _exit(QLX_NOMEM);
               nughde.len = dlen;
               if (cdb_bread(fd,nughde.s,nughde.len) == -1) _exit(QLX_CDB);
               if (flagwild)
                  if (!stralloc_cats(&nughde,local + i - 1)) _exit(QLX_NOMEM);
               if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
               close(fd);
               return;
            }
         }
         --i;
         flagwild = 1;
      } while (i);

      close(fd);
   }

   if (pipe(pi) == -1) _exit(QLX_SYS);
   args[0] = "bin/qmail-getpw";
   args[1] = local;
   args[2] = 0;
   switch(gpwpid = vfork()) {
      case -1:
         _exit(QLX_SYS);

      case 0:
         if (prot_gid(auto_gidn) == -1) _exit(QLX_USAGE);
         if (prot_uid(auto_uidp) == -1) _exit(QLX_USAGE);
         close(pi[0]);
         if (fd_move(1,pi[1]) == -1) _exit(QLX_SYS);
         execv(*args,args);
         _exit(QLX_EXECPW);
   }
   close(pi[1]);

   if (slurpclose(pi[0],&nughde,128) == -1) _exit(QLX_SYS);

   if (wait_pid(&gpwstat,gpwpid) != -1) {
      if (wait_crashed(gpwstat)) _exit(QLX_SYS);
      if (wait_exitcode(gpwstat) != 0) _exit(wait_exitcode(gpwstat));
   }
}

int spawn(fdmess,fdout,s,r,at)
int fdmess; int fdout;
char *s; char *r; int at;
{
   int f;

   if (!(f = fork())) {
      char           *(args[11]);
      unsigned long  u;
      int            n,i,
                     uid,
                     gid;
      char           *x;
      unsigned int   xlen;

#ifdef QLDAP /* copy the whole email address before the @ gets destroyed */
      stralloc ra = {0};
      int      rv;

      if (!stralloc_copys(&ra,r)) _exit(QLX_NOMEM);
#endif /* end -- save the @ */

#ifdef QLDAPDEBUG
      printf("mailaddr1: %s\n", r);
      printf("mailaddr2: %s\n", ra.s);
#endif

      r[at] = 0;
      if (!r[0]) _exit(0); /* <> */

      if (chdir(auto_qmail) == -1) _exit(QLX_USAGE);

#ifdef QLDAP /* do the address lookup - part 1 */
   rv = qldap_get(&ra);
#ifdef QLDAPDEBUG
   printf("RV: %i\n", rv);
#endif
   switch( rv ) {
      case 0:
#ifdef QLDAPDEBUG
         printf("hallo2\n");
#endif
         break;

      case 1: case 2:
         if (!stralloc_copys(&nughde,"")) _exit(QLX_NOMEM);
         if ( rv == 1 && qldap_localdelivery == 1 ) {
#endif /* end -- do the address lookup - part 1 */

      nughde_get(r);

#ifdef QLDAP /* the alias-user handling for LDAP only mode - part 2 */
         } else {
            char num[FMT_ULONG];

            if (!stralloc_copys(&nughde,auto_usera)) _exit(QLX_NOMEM);
            if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
            if (!stralloc_copys(&nughde,num,fmt_ulong(num, (long) auto_uida ))) _exit(QLX_NOMEM);
            if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
            if (!stralloc_copyb(&nughde,num,fmt_ulong(num, (long) auto_gidn ))) _exit(QLX_NOMEM);
            if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
            if (!stralloc_copys(&nughde,"/home/")) _exit(QLX_NOMEM);
            if (!stralloc_copys(&nughde,auto_usera)) _exit(QLX_NOMEM);
            if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
            if (!stralloc_copys(&nughde,"-")) _exit(QLX_NOMEM);
            if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
            if (!stralloc_copys(&nughde,r)) _exit(QLX_NOMEM);
            if (!stralloc_0(&nughde)) _exit(QLX_NOMEM);
         }
      break;

      default:
         _exit(190 + rv);
      break;
   } /* end switch */

#endif /* end -- alias-user handling - part 2 */

#ifdef QLDAPDEBUG
   printf("No of chars: %i\n", nughde.len);
   for (i=0; i<nughde.len; i++ ) {
      printf("Char[%i] = %i\n", i,(int) nughde.s[i] );
   }
#endif

      x = nughde.s;
      xlen = nughde.len;

//      args[0] = "bin/qmail-wrapper";
      args[0] = "bin/qmail-local";
      args[1] = "--";
      args[2] = x;
#ifdef QLDAPDEBUG
printf("%s\n", x);
#endif
      n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE); x += n; xlen -= n;

      scan_ulong(x,&u);
      uid = u;
#ifdef QLDAPDEBUG
printf("%s\n", x);
#endif
      n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE1); x += n; xlen -= n;

      scan_ulong(x,&u);
      gid = u;
#ifdef QLDAPDEBUG
printf("%s\n", x);
#endif
      n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE1); x += n; xlen -= n;

      args[3] = x;
#ifdef QLDAPDEBUG
printf("%s\n", x);
#endif
      n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE1); x += n; xlen -= n;

      args[4] = r;
      args[5] = x;
#ifdef QLDAPDEBUG
printf("%s\n", x);
#endif
      n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE1); x += n; xlen -= n;

      args[6] = x;
#ifdef QLDAPDEBUG
printf("%s\n", x);
#endif
      n = byte_chr(x,xlen,0); if (n++ == xlen) _exit(QLX_USAGE1); x += n; xlen -= n;

      args[7] = r + at + 1;
      args[8] = s;
      args[9] = aliasempty;
      args[10] = 0;


      if (fd_move(0,fdmess) == -1) _exit(QLX_SYS);
      if (fd_move(1,fdout) == -1) _exit(QLX_SYS);
      if (fd_copy(2,1) == -1) _exit(QLX_SYS);
      if (prot_gid(gid) == -1) _exit(QLX_USAGE);
      if (prot_uid(uid) == -1) _exit(QLX_USAGE);
      if (!getuid()) _exit(QLX_ROOT);

      execv(*args,args);
      if (error_temp(errno)) _exit(QLX_EXECSOFT);
      _exit(QLX_EXECHARD);
   }
 return f;
}
