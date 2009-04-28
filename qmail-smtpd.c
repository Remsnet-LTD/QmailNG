#include "sig.h"
#include "readwrite.h"
#include "stralloc.h"
#include "substdio.h"
#include "alloc.h"
#include "auto_qmail.h"
#include "control.h"
#include "received.h"
#include "constmap.h"
#include "error.h"
#include "ipme.h"
#include "ip.h"
#include "qmail.h"
#include "str.h"
#include "fmt.h"
#include "scan.h"
#include "byte.h"
#include "case.h"
#include "env.h"
#include "now.h"
#include "exit.h"
#include "rcpthosts.h"
#include "timeoutread.h"
#include "timeoutwrite.h"
#include "commands.h"
#include "spf.h"
#include "errbits.h"
#include "verifyrcpt.h"

#define enew()	{ eout("qmail-smtpd: pid "); epid(); eout3(" from ",remoteip,": "); }
/* Or if you prefer shorter log messages (deduce IP from tcpserver PID entry), */
/*              { eout("qmail-smtpd: pid "); epid(); eout(": "); } */
#define MAXHOPS 100
unsigned int databytes = 0;
int timeout = 1200;
unsigned int spfbehavior = 0;

char *remoteip="(not yet set)";
char *remotehost;
char *remoteinfo;
char *local;
char *relayclient;
char *verify;
int verifydefer=0;

stralloc mailfrom = {0};
stralloc rcptto = {0};
int rcptcount;
stralloc addr = {0}; /* will be 0-terminated, if addrparse returns 1 */

int safewrite(fd,buf,len) int fd; char *buf; int len;
{
  int r;
  r = timeoutwrite(timeout,fd,buf,len);
  if (r <= 0)
  {
    enew(); eout("Write error (disconnect?): quitting\n"); eflush();
    _exit(1);
  }
  return r;
}

char ssoutbuf[512];
substdio ssout = SUBSTDIO_FDBUF(safewrite,1,ssoutbuf,sizeof ssoutbuf);

void flush() { substdio_flush(&ssout); }
void out(s) char *s; { substdio_puts(&ssout,s); }

void die_read()
{
  enew(); eout("Read error (disconnect?): quitting\n"); eflush(); _exit(1);
}
void die_alarm()
{
  enew(); eout("Connection timed out: quitting\n"); eflush();
  out("451 timeout (#4.4.2)\r\n"); flush(); _exit(1);
}
void die_nomem()
{
  enew(); eout("Out of memory: quitting\n"); eflush();
  out("421 out of memory (#4.3.0)\r\n"); flush(); _exit(1);
}
void die_control()
{
  enew(); eout("Unable to read controls: quitting\n"); eflush();
  out("421 unable to read controls (#4.3.0)\r\n"); flush(); _exit(1);
}
void die_ipme()
{
  enew(); eout("Unable to figure out my IP addresses: quitting\n"); eflush();
  out("421 unable to figure out my IP addresses (#4.3.0)\r\n"); flush(); _exit(1);
}
void straynewline()
{
  enew(); eout("Stray newline: quitting\n"); eflush();
  out("451 See http://pobox.com/~djb/docs/smtplf.html.\r\n"); flush(); _exit(1);
}

void err_size() { out("552 sorry, that message size exceeds my databytes limit (#5.3.4)\r\n"); }
void err_bmf()
{
  enew(); eout("Sender address in badmailfrom\n"); eflush();
  out("553 sorry, your envelope sender is in my badmailfrom list (#5.7.1)\r\n");
}
void err_nogateway()
{
  enew(); eout("Recipient domain not in rcpthosts <"); eoutclean(addr.s); eout(">\n"); eflush();
  out("553 sorry, that domain isn't in my list of allowed rcpthosts (#5.7.1)\r\n");
}
void err_unimpl(arg) char *arg;
{
  enew(); eout("Unimplemented command\n"); eflush();
  out("502 unimplemented (#5.5.1)\r\n");
}
void err_syntax(cmd) char *cmd;
{
  char tmp_buf[FMT_ULONG];
  tmp_buf[fmt_ulong(tmp_buf,addr.len)] = 0;
  enew(); eout4(cmd," with too long address (",tmp_buf," bytes) given\n"); eflush();
  out("555 syntax error (#5.5.4)\r\n");
}
void err_wantmail()
{
  enew(); eout("Attempted RCPT or DATA before MAIL\n"); eflush();
  out("503 MAIL first (#5.5.1)\r\n");
}
void err_wantrcpt()
{
  enew(); eout("Attempted DATA before RCPT\n"); eflush();
  out("503 RCPT first (#5.5.1)\r\n");
}
void err_noop(arg) char *arg;
{
  enew(); eout("NOOP\n"); eflush();
  out("250 ok\r\n");
}
void err_vrfy(arg) char *arg;
{
  enew(); eout("VRFY requested\n"); eflush();
  out("252 send some mail, i'll try my best\r\n");
}
void err_qqt()
{
  enew(); eout("qqt failure\n"); eflush();
  out("451 qqt failure (#4.3.0)\r\n");
}
void err_hops()
{
  enew(); eout("Exceeded hop count\n"); eflush();
  out("554 too many hops, this message is looping (#5.4.6)\r\n");
}
void err_databytes()
{
  enew(); eout("Exceeded DATABYTES limit\n"); eflush();
  out("552 sorry, that message size exceeds my databytes limit (#5.3.4)\r\n");
}
void die_qvsetup()
{
  enew(); eout3("setup failure (",error_str(errno),")\n"); eflush();
  out("451 qv setup failure (#4.3.0)\r\n"); flush(); _exit(1);
}
void die_qvtimeout()
{
  enew(); eout("Timeout (no response from verification server)\n"); eflush();
  out("451 qv temporary failure (#4.3.0)\r\n"); flush(); _exit(1);
}
void die_qvmiscfail()
{
  enew(); eout3("temporary failure (",error_str(errno),")\n"); eflush();
  out("451 qv temporary failure (#4.3.0)\r\n"); flush(); _exit(1);
}
void err_nosuchuser550()
{
  enew(); eout("Unverified mailbox at RCPT time\n"); eflush();
  out("550 sorry, no mailbox here by that name. (#5.1.1)\r\n");
}
void err_nosuchuser554()
{
  enew(); eout("Unverified mailbox(es) at DATA time\n"); eflush();
  out("554 sorry, invalid mailbox name(s). (#5.1.1)\r\n");
}


stralloc greeting = {0};
stralloc spflocal = {0};
stralloc spfguess = {0};
stralloc spfexp = {0};

void smtp_greet(code) char *code;
{
  substdio_puts(&ssout,code);
  substdio_put(&ssout,greeting.s,greeting.len);
}
void smtp_help()
{
  out("214 netqmail home page: http://qmail.org/netqmail\r\n");
}
void smtp_quit()
{
  enew(); eout("Remote end QUIT: quitting\n"); eflush();
  smtp_greet("221 "); out("\r\n"); flush(); _exit(0);
}

stralloc helohost = {0};
char *fakehelo; /* pointer into helohost, or 0 */

void dohelo(arg) char *arg; {
  if (!stralloc_copys(&helohost,arg)) die_nomem(); 
  if (!stralloc_0(&helohost)) die_nomem(); 
  fakehelo = case_diffs(remotehost,helohost.s) ? helohost.s : 0;
}

int liphostok = 0;
stralloc liphost = {0};
int bmfok = 0;
stralloc bmf = {0};
struct constmap mapbmf;

void setup()
{
  char *x;
  unsigned long u;
 
  if (control_init() == -1) die_control();
  if (control_rldef(&greeting,"control/smtpgreeting",1,(char *) 0) != 1)
    die_control();
  liphostok = control_rldef(&liphost,"control/localiphost",1,(char *) 0);
  if (liphostok == -1) die_control();
  if (control_readint(&timeout,"control/timeoutsmtpd") == -1) die_control();
  if (timeout <= 0) timeout = 1;

  if (rcpthosts_init() == -1) die_control();

  bmfok = control_readfile(&bmf,"control/badmailfrom",0);
  if (bmfok == -1) die_control();
  if (bmfok)
    if (!constmap_init(&mapbmf,bmf.s,bmf.len,0)) die_nomem();
 
  if (control_readint(&databytes,"control/databytes") == -1) die_control();
  x = env_get("DATABYTES");
  if (x) { scan_ulong(x,&u); databytes = u; }
  if (!(databytes + 1)) --databytes;
 
  if (control_readint(&spfbehavior,"control/spfbehavior") == -1)
    die_control();
  x = env_get("SPFBEHAVIOR");
  if (x) { scan_ulong(x,&u); spfbehavior = u; }

  if (control_readline(&spflocal,"control/spfrules") == -1) die_control();
  if (spflocal.len && !stralloc_0(&spflocal)) die_nomem();
  if (control_readline(&spfguess,"control/spfguess") == -1) die_control();
  if (spfguess.len && !stralloc_0(&spfguess)) die_nomem();
  if (control_rldef(&spfexp,"control/spfexp",0,SPF_DEFEXP) == -1)
    die_control();
  if (!stralloc_0(&spfexp)) die_nomem();

  remoteip = env_get("TCPREMOTEIP");
  if (!remoteip) remoteip = "unknown";
  local = env_get("TCPLOCALHOST");
  if (!local) local = env_get("TCPLOCALIP");
  if (!local) local = "unknown";
  remotehost = env_get("TCPREMOTEHOST");
  if (!remotehost) remotehost = "unknown";
  remoteinfo = env_get("TCPREMOTEINFO");
  relayclient = env_get("RELAYCLIENT");
  verify =  env_get("VERIFY");
  if (verify)
  {
    if (*verify == '\0') /* Disable verification if VERIFY="" */
    {
      verify = 0;
      /* Warning message since previous version of qmail-verify used VERIFY="" to enable verification */
      enew(); eout("Note recipient verification explicitly disabled.\n"); eflush();
    }
    else
      if ((*verify == 'D') || (*verify == 'd')) verifydefer=1;
  }
  dohelo(remotehost);
  enew(); eout("New session\n"); eflush();
}


int addrparse(arg)
char *arg;
{
  int i;
  char ch;
  char terminator;
  struct ip_address ip;
  int flagesc;
  int flagquoted;
 
  terminator = '>';
  i = str_chr(arg,'<');
  if (arg[i])
    arg += i + 1;
  else { /* partner should go read rfc 821 */
    terminator = ' ';
    arg += str_chr(arg,':');
    if (*arg == ':') ++arg;
    while (*arg == ' ') ++arg;
  }

  /* strip source route */
  if (*arg == '@') while (*arg) if (*arg++ == ':') break;

  if (!stralloc_copys(&addr,"")) die_nomem();
  flagesc = 0;
  flagquoted = 0;
  for (i = 0;ch = arg[i];++i) { /* copy arg to addr, stripping quotes */
    if (flagesc) {
      if (!stralloc_append(&addr,&ch)) die_nomem();
      flagesc = 0;
    }
    else {
      if (!flagquoted && (ch == terminator)) break;
      switch(ch) {
        case '\\': flagesc = 1; break;
        case '"': flagquoted = !flagquoted; break;
        default: if (!stralloc_append(&addr,&ch)) die_nomem();
      }
    }
  }
  /* could check for termination failure here, but why bother? */
  if (!stralloc_append(&addr,"")) die_nomem();

  if (liphostok) {
    i = byte_rchr(addr.s,addr.len,'@');
    if (i < addr.len) /* if not, partner should go read rfc 821 */
      if (addr.s[i + 1] == '[')
        if (!addr.s[i + 1 + ip_scanbracket(addr.s + i + 1,&ip)])
          if (ipme_is(&ip)) {
            addr.len = i + 1;
            if (!stralloc_cat(&addr,&liphost)) die_nomem();
            if (!stralloc_0(&addr)) die_nomem();
          }
  }

  if (addr.len > 900) return 0;
  return 1;
}

int bmfcheck()
{
  int j;
  if (!bmfok) return 0;
  if (constmap(&mapbmf,addr.s,addr.len - 1)) return 1;
  j = byte_rchr(addr.s,addr.len,'@');
  if (j < addr.len)
    if (constmap(&mapbmf,addr.s + j,addr.len - j - 1)) return 1;
  return 0;
}

int addrallowed()
{
  int r;
  r = rcpthosts(addr.s,str_len(addr.s));
  if (r == -1) die_control();
  return r;
}


int seenmail = 0;
int flagbarf; /* defined if seenmail */
int flagbarfspf;
stralloc spfbarfmsg = {0};

void smtp_helo(arg) char *arg;
{
  enew(); eout("Received HELO "); eoutclean(arg); eout("\n"); eflush();
  smtp_greet("250 "); out("\r\n");
  seenmail = 0; dohelo(arg);
}
void smtp_ehlo(arg) char *arg;
{
  enew(); eout("Received EHLO "); eoutclean(arg); eout("\n"); eflush();
  smtp_greet("250-"); out("\r\n250-PIPELINING\r\n250 8BITMIME\r\n");
  seenmail = 0; dohelo(arg);
}
void smtp_rset()
{
  flagdenyany = 0;
  seenmail = 0;
  enew(); eout("Session RSET\n"); eflush();
  out("250 flushed\r\n");
}

void smtp_mail(arg) char *arg;
{
  int r;

  if (!addrparse(arg)) { err_syntax("MAIL"); return; }
  flagbarf = bmfcheck();
  flagbarfspf = 0;
  if (spfbehavior && !relayclient)
   {
    switch(r = spfcheck()) {
    case SPF_OK: env_put2("SPFRESULT","pass"); break;
    case SPF_NONE: env_put2("SPFRESULT","none"); break;
    case SPF_UNKNOWN: env_put2("SPFRESULT","unknown"); break;
    case SPF_NEUTRAL: env_put2("SPFRESULT","neutral"); break;
    case SPF_SOFTFAIL: env_put2("SPFRESULT","softfail"); break;
    case SPF_FAIL: env_put2("SPFRESULT","fail"); break;
    case SPF_ERROR: env_put2("SPFRESULT","error"); break;
    }
    switch (r) {
    case SPF_NOMEM:
      die_nomem();
    case SPF_ERROR:
      if (spfbehavior < 2) break;
      out("451 SPF lookup failure (#4.3.0)\r\n");
      return;
    case SPF_NONE:
    case SPF_UNKNOWN:
      if (spfbehavior < 6) break;
    case SPF_NEUTRAL:
      if (spfbehavior < 5) break;
    case SPF_SOFTFAIL:
      if (spfbehavior < 4) break;
    case SPF_FAIL:
      if (spfbehavior < 3) break;
      if (!spfexplanation(&spfbarfmsg)) die_nomem();
      if (!stralloc_0(&spfbarfmsg)) die_nomem();
      flagbarfspf = 1;
    }
   }
  else
   env_unset("SPFRESULT");
  seenmail = 1;
  if (!stralloc_copys(&rcptto,"")) die_nomem();
  if (!stralloc_copys(&mailfrom,addr.s)) die_nomem();
  if (!stralloc_0(&mailfrom)) die_nomem();
  rcptcount = 0;
  enew(); eout("Sender <"); eoutclean(mailfrom.s); eout(">\n"); eflush();
  out("250 ok\r\n");
}

void err_spf() {
  int i,j;

  for(i = 0; i < spfbarfmsg.len; i = j + 1) {
    j = byte_chr(spfbarfmsg.s + i, spfbarfmsg.len - i, '\n') + i;
    if (j < spfbarfmsg.len) {
      out("550-");
      spfbarfmsg.s[j] = 0;
      out(spfbarfmsg.s);
      spfbarfmsg.s[j] = '\n';
      out("\r\n");
    } else {
      out("550 ");
      out(spfbarfmsg.s);
      out(" (#5.7.1)\r\n");
    }
  }
}

void smtp_rcpt(arg) char *arg; {
  if (!seenmail) { err_wantmail(); return; }
  if (!addrparse(arg)) { err_syntax("RCPT"); return; }
  if (flagbarf) { err_bmf(); return; }
  if (flagbarfspf) { err_spf(); return; }
  if (relayclient) {
    --addr.len;
    if (!stralloc_cats(&addr,relayclient)) die_nomem();
    if (!stralloc_0(&addr)) die_nomem();
  }
  else
  {
    if (!addrallowed()) { err_nogateway(); return; }
    if (verify && verifyrcpt(find_digit_colon_eos(verify),&addr,verifydefer,die_qvtimeout,die_qvmiscfail))
    {
      err_nosuchuser550();
      return;
    }
  }
  if (!stralloc_cats(&rcptto,"T")) die_nomem();
  if (!stralloc_cats(&rcptto,addr.s)) die_nomem();
  if (!stralloc_0(&rcptto)) die_nomem();
  ++rcptcount;
  enew(); eout("Recipient <"); eoutclean(addr.s); eout(">\n"); eflush();
  out("250 ok\r\n");
}


int saferead(fd,buf,len) int fd; char *buf; int len;
{
  int r;
  flush();
  r = timeoutread(timeout,fd,buf,len);
  if (r == -1) if (errno == error_timeout) die_alarm();
  if (r <= 0) die_read();
  return r;
}

char ssinbuf[1024];
substdio ssin = SUBSTDIO_FDBUF(saferead,0,ssinbuf,sizeof ssinbuf);

struct qmail qqt;
unsigned int bytestooverflow = 0;
unsigned int messagebytes = 0;

void put(ch)
char *ch;
{
  if (bytestooverflow)
    if (!--bytestooverflow)
      qmail_fail(&qqt);
  messagebytes++;
  qmail_put(&qqt,ch,1);
}

void blast(hops)
int *hops;
{
  char ch;
  int state;
  int flaginheader;
  int pos; /* number of bytes since most recent \n, if fih */
  int flagmaybex; /* 1 if this line might match RECEIVED, if fih */
  int flagmaybey; /* 1 if this line might match \r\n, if fih */
  int flagmaybez; /* 1 if this line might match DELIVERED, if fih */
 
  state = 1;
  *hops = 0;
  flaginheader = 1;
  pos = 0; flagmaybex = flagmaybey = flagmaybez = 1;
  for (;;) {
    substdio_get(&ssin,&ch,1);
    if (flaginheader) {
      if (pos < 9) {
        if (ch != "delivered"[pos]) if (ch != "DELIVERED"[pos]) flagmaybez = 0;
        if (flagmaybez) if (pos == 8) ++*hops;
        if (pos < 8)
          if (ch != "received"[pos]) if (ch != "RECEIVED"[pos]) flagmaybex = 0;
        if (flagmaybex) if (pos == 7) ++*hops;
        if (pos < 2) if (ch != "\r\n"[pos]) flagmaybey = 0;
        if (flagmaybey) if (pos == 1) flaginheader = 0;
	++pos;
      }
      if (ch == '\n') { pos = 0; flagmaybex = flagmaybey = flagmaybez = 1; }
    }
    switch(state) {
      case 0:
        if (ch == '\n') straynewline();
        if (ch == '\r') { state = 4; continue; }
        break;
      case 1: /* \r\n */
        if (ch == '\n') straynewline();
        if (ch == '.') { state = 2; continue; }
        if (ch == '\r') { state = 4; continue; }
        state = 0;
        break;
      case 2: /* \r\n + . */
        if (ch == '\n') straynewline();
        if (ch == '\r') { state = 3; continue; }
        state = 0;
        break;
      case 3: /* \r\n + .\r */
        if (ch == '\n') return;
        put(".");
        put("\r");
        if (ch == '\r') { state = 4; continue; }
        state = 0;
        break;
      case 4: /* + \r */
        if (ch == '\n') { state = 1; break; }
        if (ch != '\r') { put("\r"); state = 0; }
    }
    put(&ch);
  }
}

void spfreceived()
{
  stralloc sa = {0};
  stralloc rcvd_spf = {0};

  if (!spfbehavior || relayclient) return;

  if (!stralloc_copys(&rcvd_spf, "Received-SPF: ")) die_nomem();
  if (!spfinfo(&sa)) die_nomem();
  if (!stralloc_cat(&rcvd_spf, &sa)) die_nomem();
  if (!stralloc_append(&rcvd_spf, "\n")) die_nomem();
  if (bytestooverflow) {
    bytestooverflow -= rcvd_spf.len;
    if (bytestooverflow <= 0) qmail_fail(&qqt);
  }
  qmail_put(&qqt,rcvd_spf.s,rcvd_spf.len);
}


char accept_buf[FMT_ULONG];
void acceptmessage(qp) unsigned long qp;
{
  datetime_sec when;
  when = now();
  out("250 ok ");
  accept_buf[fmt_ulong(accept_buf,(unsigned long) when)] = 0;
  out(accept_buf);
  out(" qp ");
  accept_buf[fmt_ulong(accept_buf,qp)] = 0;
  out(accept_buf);
  out("\r\n");
  enew(); eout2("Message accepted, qp ",accept_buf);
  accept_buf[fmt_ulong(accept_buf,rcptcount)] = 0;
  eout3(" (",accept_buf," recipients, ");
  accept_buf[fmt_ulong(accept_buf,messagebytes)] = 0;
  eout2(accept_buf," bytes)\n"); eflush();
}

void smtp_data() {
  int hops;
  unsigned long qp;
  char *qqx;

  if (!seenmail) { err_wantmail(); return; }
  if (!rcptto.len) { err_wantrcpt(); return; }
  if (verifydefer && flagdenyany) { err_nosuchuser554(); return; }
  seenmail = 0;
  if (databytes) bytestooverflow = databytes + 1;
  messagebytes = 0;
  if (qmail_open(&qqt) == -1) { err_qqt(); return; }
  qp = qmail_qp(&qqt);
  out("354 go ahead\r\n");
 
  received(&qqt,"SMTP",local,remoteip,remotehost,remoteinfo,fakehelo);
  spfreceived();
  blast(&hops);
  hops = (hops >= MAXHOPS);
  if (hops) qmail_fail(&qqt);
  qmail_from(&qqt,mailfrom.s);
  qmail_put(&qqt,rcptto.s,rcptto.len);
 
  qqx = qmail_close(&qqt);
  if (!*qqx) { acceptmessage(qp); return; }
  if (hops) { err_hops(); return; }
  if (databytes) if (!bytestooverflow) { err_databytes(); return; }
  if (*qqx == 'D') out("554 "); else out("451 ");
  out(qqx + 1);
  out("\r\n");
  enew(); eout("Message rejected (");
  if (*qqx == 'D') eout("554 "); else eout("451 ");
  eoutclean(qqx + 1); eout(")\n");
  eflush();
}

struct commands smtpcommands[] = {
  { "rcpt", smtp_rcpt, 0 }
, { "mail", smtp_mail, 0 }
, { "data", smtp_data, flush }
, { "quit", smtp_quit, flush }
, { "helo", smtp_helo, flush }
, { "ehlo", smtp_ehlo, flush }
, { "rset", smtp_rset, 0 }
, { "help", smtp_help, flush }
, { "noop", err_noop, flush }
, { "vrfy", err_vrfy, flush }
, { 0, err_unimpl, flush }
} ;

void main()
{
  sig_pipeignore();
  /* esetfd(2); Errors default to FD2 (stderr), change here if needed */
  if (chdir(auto_qmail) == -1) die_control();
  setup();
  if (ipme_init() != 1) die_ipme();
  smtp_greet("220 ");
  out(" ESMTP\r\n");
  if (commands(&ssin,&smtpcommands) == 0) die_read();
  die_nomem();
}
