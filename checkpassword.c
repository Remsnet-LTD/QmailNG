#include <errno.h>
#define QLDAP_PORT LDAP_PORT
#include "control.h"
#include "stralloc.h"
#include "env.h"
#include "lber.h"
#include "ldap.h"
#include "auto_usera.h"
#include "auto_uids.h"
#include "auto_qmail.h"
#include "fmt.h"
#include "check.h"
#include "qlx.h"

// #define QLDAPDEBUG

#ifdef QLDAPDEBUG
 #include <string.h>
#endif

extern int errno;
extern char *crypt();
extern char *malloc();
extern char **environ;

char up[513];

int uplen;

char *str1e2(name,value) char *name; char *value;
{
 char *nv;
 nv = malloc(strlen(name) + strlen(value) + 2);
 if (!nv) _exit(111);
 strcpy(nv,name);
 strcat(nv,"=");
 strcat(nv,value);
 return nv;
}



/* initialize the string arrays, this uses DJB's libs */
stralloc    qldap_server = {0};
stralloc    qldap_basedn = {0};
stralloc    qldap_user = {0};
stralloc    qldap_password = {0};

stralloc    qldap_messagestore = {0};
stralloc    qldap_uid = {0};
stralloc    qldap_gid = {0};
/* init done */

/* read the various LDAP control files */

void get_qldap_controls()
{
   if (control_rldef(&qldap_server,"/var/qmail/control/ldapserver",0,(char *) 0) != 1) {
#if QLDAP_DEBUG
      printf("unable to read control/ldapserver\n");
#endif
      _exit(1);
   }
   if (!stralloc_0(&qldap_server)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_basedn,"/var/qmail/control/ldapbasedn",0,"") != 1);
   if (!stralloc_0(&qldap_basedn)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_user,"/var/qmail/control/ldaplogin",0,"") != 1);
   if (!stralloc_0(&qldap_user)) _exit(QLX_NOMEM);

   if (control_rldef(&qldap_password,"/var/qmail/control/ldappassword",0,"") != 1);
   if (!stralloc_0(&qldap_password)) _exit(QLX_NOMEM);

#ifdef QLDAPDEBUG
      printf("ldapserver = %s\n", qldap_server.s);
      printf("ldapbasedn = %s\n", qldap_basedn.s);
      printf("ldapuser = %s\n", qldap_user.s);
      printf("ldappassword = %s\n", qldap_password.s);
#endif
}

void qldap_get( char *login, stralloc *passwd, unsigned int *uid, unsigned int *gid, stralloc *homedir )
{
   LDAP           *ld;
   LDAPMessage    *res, *msg;
   char           *dn,
                  **vals,
                  *attrs[] = {  "uid",
                                "userPassword",
                                "qmailUID",
                                "qmailGID",
                                "mailMessagestore", NULL };

   int            version,
                  rc,
                  num_entries = 0;

   stralloc       filter = {0};

#ifdef QLDAPDEBUG
 printf("qldap_get called\n");
#endif

   /* check the login uid for illegal characters         *
    * only [a-z][0-9][.-_]@[a-z][0-9][.-] are allowed    *
    * because all other stuff would kill the LDAP search */
//   if ( !chck_mails(login) ) _exit(1);

#ifdef QLDAPDEBUG
 printf("check_mails succeeded");
#endif

   /* initialize the LDAP connection and get a handle */
   if ( (ld = ldap_init(qldap_server.s,QLDAP_PORT)) == NULL ) _exit(1);

   /* set LDAP connection options (only with Mozilla LDAP SDK) */
#ifdef LDAP_OPT_PROTOCOL_VERSION
   version = LDAP_VERSION2;
   if ( ldap_set_option(ld,LDAP_OPT_PROTOCOL_VERSION,&version) != LDAP_SUCCESS ) _exit(1);
#endif

   /* connect to the LDAP server */
   if ( (rc = ldap_simple_bind_s(ld,qldap_user.s,qldap_password.s)) != LDAP_SUCCESS ) {
#ifdef QLDAPDEBUG
      printf("exit ldap %s\n", ldap_err2string(rc));
#endif
      _exit(1);
   }

   /* build the search string for the login uid */
   if (!stralloc_copys(&filter,"(uid=" ) ) _exit(QLX_NOMEM);
   if (!stralloc_cats(&filter,login)) _exit(QLX_NOMEM);
   if (!stralloc_cats(&filter,")")) _exit(QLX_NOMEM);
   if (!stralloc_0(&filter)) _exit(QLX_NOMEM);

#ifdef QLDAPDEBUG
   printf("filter: %s\n", filter.s);
#endif

   /* do the search for the login uid */
   if ( (rc = ldap_search_ext_s(ld,qldap_basedn.s,LDAP_SCOPE_SUBTREE,filter.s,attrs,0,NULL,NULL,NULL,1,&res)) != LDAP_SUCCESS ) {
#ifdef QLDAPDEBUG
      printf("exit ldap %s\n", ldap_err2string(rc));
#endif
      _exit(1);
   }

   /* count the results, we must have exactly one */
   if ( (num_entries = ldap_count_entries(ld,res)) != 1) _exit(1);
   /* go to the first entry */
   msg = ldap_first_entry(ld,res);

   /* get the dn and free it (we dont need it, to prevent memory leaks) */
   if ( (dn = ldap_get_dn(ld,msg)) != NULL ) ldap_memfree(dn);

   /* go through the attributes and set the proper args for qmail-pop3d */

   if ( (vals = ldap_get_values(ld,msg,attrs[1])) != NULL ) {
      if (!stralloc_copys(passwd, vals[0])) _exit(QLX_NOMEM);
      if (!stralloc_0(passwd)) _exit(QLX_NOMEM);
#ifdef QLDAPDEBUG
      printf("userPassword: %s\n", vals[0]);
#endif
   } else {
#ifdef QLDAPDEBUG
     printf("No userpassword\n");
#endif
      _exit(1);
   }
   ldap_value_free(vals);

   /* get the UID for delivery on the local system */
   if ( (vals = ldap_get_values(ld,msg,attrs[2])) != NULL ) {
#ifdef QLDAPDEBUG
      printf("qmailUID: %s\n", vals[0]);
#endif
      *uid = chck_ids(vals[0]);
      if (100 > *uid ) _exit(1);
   } else { /* default */
      if (!qldap_uid.len) _exit(1);
      *uid = chck_idb(qldap_uid.s,qldap_uid.len);
      if (100 > *uid ) _exit(1);
   }
   ldap_value_free(vals);

   /* get the GID for delivery on the local system */
   if ( (vals = ldap_get_values(ld,msg,attrs[3])) != NULL ) {
#ifdef QLDAPDEBUG
      printf("qmailGID: %s\n", vals[0]);
#endif
      *gid = chck_ids(vals[0]);
      if ( 100 > *gid ) _exit(1);   /*??? muss gid wirklich >100 sein? */
   } else { /* default */
      if (!qldap_gid.len) _exit(1);
      *gid = chck_idb(qldap_gid.s,qldap_gid.len);
      if ( 100 > *gid ) _exit(1);   /*??? s.o. */
   }
   ldap_value_free(vals);

   /* get the path of the maildir or mbox */
   if ( (vals = ldap_get_values(ld,msg,attrs[4])) != NULL ) {
#ifdef QLDAPDEBUG
      printf("mailMessageStore: %s\n", vals[0]);
#endif
      if (vals[0][0] != '/') {   /* relative path */
         /* ??? ACHTUNG qldap_messagestore ist leer! */
         // if (qldap_messagestore.s[0] != '/') _exit(1);
         // if (qldap_messagestore.s[qldap_messagestore.len -1] != '/') _exit(1);
         if (!stralloc_cats(&qldap_messagestore, vals[0])) _exit(QLX_NOMEM);
         if (!chck_pathb(qldap_messagestore.s,qldap_messagestore.len) ) _exit(1);
         if (qldap_messagestore.s[qldap_messagestore.len -1] != '/') _exit(1);
         if (!stralloc_copy(homedir,qldap_messagestore)) _exit(QLX_NOMEM);
      } else {                   /* absolute path */
         if (!chck_paths(vals[0]) ) _exit(1);
         if (!stralloc_copys(homedir, vals[0])) _exit(QLX_NOMEM);
         //if (homedir.s[homedir.len -1] != '/') _exit(1);
         if (homedir->s[homedir->len -1] != '/') _exit(1);
      }
   if (!stralloc_0(homedir)) _exit(QLX_NOMEM);
   } else {
      _exit(1);
   }
   ldap_value_free(vals);
}

void main(argc,argv)
int argc;
char **argv;
{
 char *login,
      *encrypted,
      *entredpassword;

 stralloc password={0};
 stralloc homedir={0};

 int  r,
      i;

 unsigned int uid,
              gid;

 char **newenv;

 int numenv;

 if (!argv[1]) _exit(2);

 get_qldap_controls();

#ifndef QLDAPDEBUG
 uplen = 0;
 for (;;)
  {
   do r = read(3,up + uplen,sizeof(up) - uplen);
   while ((r == -1) && (errno == EINTR));
   if (r == -1) _exit(111);
   if (r == 0) break;
   uplen += r;
   if (uplen >= sizeof(up)) _exit(1);
  }
 close(3);
#else
 uplen=22;
 memcpy(up,"opi\0testit\0 123456789\0",22);
#endif

 i = 0;
 login = up + i;
 while (up[i++]) if (i == uplen) _exit(2);
 entredpassword = up + i;
 if (i == uplen) _exit(2);
 while (up[i++]) if (i == uplen) _exit(2);

#ifdef QLDAPDEBUG
 printf("Before LDAP lookup: (%s)  (%s)  (%s)\n", up, login, entredpassword);
#endif

 qldap_get(login, &password, &uid, &gid, &homedir);

 encrypted = crypt(entredpassword,password.s);

#ifdef QLDAPDEBUG
 printf("entred: %s  login: %s  userpass: %s\n",entredpassword,login,password.s);
#endif

 if (!*password.s || strcmp(entredpassword,password.s)) _exit(1);
 for(i = 0; i < sizeof(up); i++) up[i] = 0;

 if (setgid(gid) == -1) _exit(1);

 if (setuid(uid) == -1) _exit(1);
 if (chdir(homedir.s) == -1) _exit(111);

 numenv = 0;

 while (environ[numenv]) ++numenv;
 newenv = (char **) malloc((numenv + 4) * sizeof(char *)); /* ??? warum 4 */

 if (!newenv) _exit(111);

 for (i = 0;i < numenv;++i) newenv[i] = environ[i];
 newenv[numenv++] = str1e2("USER",login);
 newenv[numenv++] = str1e2("HOME",homedir.s);
 newenv[numenv] = 0;

 environ = newenv;

 execvp(argv[1],argv + 1);

 _exit(111);
}
