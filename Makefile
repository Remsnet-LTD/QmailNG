# Don't edit Makefile!
# If you want to use a different compiler, change conf-cc.sh.

NROFF=nroff -man

default: all

addresses.0: \
addresses.5
	$(NROFF) addresses.5 > addresses.0

all: \
qmail-alias qmail-clean qmail-home qmail-inject qmail-lspawn \
qmail-queue qmail-remote qmail-rspawn qmail-send qmail-setup \
qmail-qmtpd qmail-smtpd qmail-makectl qmail-showctl qmail-check \
qmail-setup qmail-start qmail-pop3d qmail-popup tokenize dnscname \
dnsptr dnsip dnsmxip dnsfq hostname ipmeprint tcp-env sendmail \
qmail-qread qmail-qstat qmail-tcpto qlist qlist2 qreceipt qsmhook \
qbiff forward preline condredirect maildirmake maildir2mbox \
maildir2qmtp maildir2smtp maildirwatch mailsubj qail elq pinq \
qmail-upq

alloc.o: \
compile alloc.c error.h alloc.h
	./compile alloc.c

alloc_re.o: \
compile alloc_re.c alloc.h byte.h
	./compile alloc_re.c

auto-hasflock.h: \
tryflock.c compile load
	( ( ./compile tryflock.c && ./load tryflock ) >/dev/null 2>&1 \
	&& echo \#define HASFLOCK 1 || exit 0 ) > auto-hasflock.h

auto-hasmkffo.h: \
trymkffo.c compile load
	( ( ./compile trymkffo.c && ./load trymkffo ) >/dev/null 2>&1 \
	&& echo \#define HASMKFIFO 1 || exit 0 ) > auto-hasmkffo.h

auto-hasnpbg1.h: \
trynpbg1.c compile load open.h open.o fifo.h fifo.o select.h
	( ( ./compile trynpbg1.c \
	&& ./load trynpbg1 open.o fifo.o && ./trynpbg1 ) >/dev/null 2>&1 \
	&& echo \#define HASNAMEDPIPEBUG1 1 || exit 0 ) > auto-hasnpbg1.h

auto-hassalen.h: \
trysalen.c compile
	( ./compile trysalen.c >/dev/null 2>&1 \
	&& echo \#define HASSALEN 1 || exit 0 ) > auto-hassalen.h

auto-hassgact.h: \
trysgact.c compile load
	( ( ./compile trysgact.c && ./load trysgact ) >/dev/null 2>&1 \
	&& echo \#define HASSIGACTION 1 || exit 0 ) > auto-hassgact.h

auto-hassgprm.h: \
trysgprm.c compile load
	( ( ./compile trysgprm.c && ./load trysgprm ) >/dev/null 2>&1 \
	&& echo \#define HASSIGPROCMASK 1 || exit 0 ) > auto-hassgprm.h

auto-hasshsgr.h: \
tryshsgr.c chkshsgr compile load
	./chkshsgr || ( cat warn-shsgr; exit 1 )
	( ( ./compile tryshsgr.c \
	&& ./load tryshsgr && ./tryshsgr ) >/dev/null 2>&1 \
	&& echo \#define HASSHORTSETGROUPS 1 || exit 0 ) > auto-hasshsgr.h

auto-haswaitp.h: \
trywaitp.c compile load
	( ( ./compile trywaitp.c && ./load trywaitp ) >/dev/null 2>&1 \
	&& echo \#define HASWAITPID 1 || exit 0 ) > auto-haswaitp.h

auto-uids.h: \
printuids
	./printuids > temp-uids.h
	mv temp-uids.h auto-uids.h

byte_chr.o: \
compile byte_chr.c byte.h
	./compile byte_chr.c

byte_copy.o: \
compile byte_copy.c byte.h
	./compile byte_copy.c

byte_cr.o: \
compile byte_cr.c byte.h
	./compile byte_cr.c

byte_diff.o: \
compile byte_diff.c byte.h
	./compile byte_diff.c

byte_rchr.o: \
compile byte_rchr.c byte.h
	./compile byte_rchr.c

byte_zero.o: \
compile byte_zero.c byte.h
	./compile byte_zero.c

case.o: \
compile case.c case.h
	./compile case.c

check: \
do-check

chkshsgr: \
chkshsgr.o load
	./load chkshsgr

chkshsgr.o: \
compile chkshsgr.c exit.h
	./compile chkshsgr.c

chkspawn: \
load chkspawn.o libsubstdio.a liberror.a libstr.a libfs.a
	./load chkspawn libsubstdio.a liberror.a libstr.a libfs.a

chkspawn.o: \
compile chkspawn.c fmt.h substdio.h select.h subfd.h exit.h \
conf-unusual.h
	./compile chkspawn.c

clean: \
do-clean

coe.o: \
compile coe.c coe.h
	./compile coe.c

compile: \
make-commands systype
	./make-commands "`cat ./systype`" compile > compile
	chmod 755 compile

condredirect: \
condredirect.o signal.o wait.o qqtalk.o seek.o libenv.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load condredirect signal.o wait.o qqtalk.o seek.o libenv.a \
	libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

condredirect.0: \
condredirect.1
	$(NROFF) condredirect.1 > condredirect.0

condredirect.o: \
compile condredirect.c stralloc.h gen_alloc.h exit.h qqtalk.h error.h \
substdio.h wait.h subfd.h env.h fork.h readwrite.h seek.h signal.h
	./compile condredirect.c

constmap.o: \
compile constmap.c alloc.h case.h constmap.h
	./compile constmap.c

control.o: \
compile control.c scan.h open.h substdio.h error.h control.h alloc.h \
getline.h readwrite.h stralloc.h gen_alloc.h
	./compile control.c

date822fmt.o: \
compile date822fmt.c date822fmt.h datetime.h fmt.h
	./compile date822fmt.c

datetime.o: \
compile datetime.c datetime.h
	./compile datetime.c

direntry.h: \
compile trydrent.c direntry.h1 direntry.h2
	( ./compile trydrent.c >/dev/null 2>&1 \
	&& cat direntry.h2 || cat direntry.h1 ) > direntry.h

dns.o: \
compile dns.c stralloc.h gen_alloc.h fmt.h case.h ipalloc.h ip.h \
alloc.h str.h dns.h
	./compile dns.c

dnscname: \
dnscname.o dns.o dnsdoe.o ip.o ipalloc.o case.o libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnscname dns.o dnsdoe.o ip.o ipalloc.o case.o \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a \
	libfs.a

dnscname.o: \
compile dnscname.c gen_alloc.h exit.h dnsdoe.h substdio.h subfd.h \
readwrite.h dns.h stralloc.h
	./compile dnscname.c

dnsdoe.o: \
compile dnsdoe.c dnsdoe.h substdio.h subfd.h dns.h exit.h
	./compile dnsdoe.c

dnsfq: \
dnsfq.o dns.o dnsdoe.o ip.o ipalloc.o case.o libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsfq dns.o dnsdoe.o ip.o ipalloc.o case.o \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsfq.o: \
compile dnsfq.c gen_alloc.h exit.h dnsdoe.h ipalloc.h ip.h substdio.h \
subfd.h dns.h stralloc.h
	./compile dnsfq.c

dnsip: \
dnsip.o dns.o dnsdoe.o ip.o ipalloc.o case.o libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsip dns.o dnsdoe.o ip.o ipalloc.o case.o \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsip.o: \
compile dnsip.c gen_alloc.h exit.h dnsdoe.h ipalloc.h ip.h substdio.h \
subfd.h dns.h stralloc.h
	./compile dnsip.c

dnsmxip: \
dnsmxip.o dns.o dnsdoe.o ip.o ipalloc.o now.o case.o libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsmxip dns.o dnsdoe.o ip.o ipalloc.o now.o case.o \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsmxip.o: \
compile dnsmxip.c stralloc.h now.h gen_alloc.h exit.h fmt.h dnsdoe.h \
ipalloc.h ip.h substdio.h subfd.h dns.h datetime.h
	./compile dnsmxip.c

dnsptr: \
dnsptr.o dns.o dnsdoe.o ip.o ipalloc.o case.o libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsptr dns.o dnsdoe.o ip.o ipalloc.o case.o \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsptr.o: \
compile dnsptr.c stralloc.h gen_alloc.h exit.h scan.h dnsdoe.h ip.h \
substdio.h subfd.h str.h dns.h
	./compile dnsptr.c

do-check:
	./qmail-check

do-clean:
	rm -f `cat TARGETS`

do-setup: \
all man
	./qmail-setup

do-shar:
	shar -m `cat FILES` > qmail.shar
	chmod 400 qmail.shar

dot-qmail.0: \
dot-qmail.5
	$(NROFF) dot-qmail.5 > dot-qmail.0

dot-qmail.5: \
dot-qmail.9 printbreak printnumd
	cat dot-qmail.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> dot-qmail.5

elq: \
elq.sh qmail-home warn-auto.sh
	cat warn-auto.sh elq.sh \
	| sed s}HOME}"`./qmail-home`"}g > elq
	chmod 755 elq

env.o: \
compile env.c env.h str.h alloc.h
	./compile env.c

envelopes.0: \
envelopes.5
	$(NROFF) envelopes.5 > envelopes.0

envread.o: \
compile envread.c str.h env.h
	./compile envread.c

error.o: \
compile error.c error.h
	./compile error.c

error_str.o: \
compile error_str.c error.h
	./compile error_str.c

error_temp.o: \
compile error_temp.c error.h
	./compile error_temp.c

fifo.o: \
compile fifo.c auto-hasmkffo.h fifo.h
	./compile fifo.c

find-systype: \
find-systype.sh conf-cc.sh warn-auto.sh
	cat warn-auto.sh conf-cc.sh find-systype.sh > find-systype
	chmod 755 find-systype

fmt_str.o: \
compile fmt_str.c fmt.h
	./compile fmt_str.c

fmt_strn.o: \
compile fmt_strn.c fmt.h
	./compile fmt_strn.c

fmt_uint.o: \
compile fmt_uint.c fmt.h
	./compile fmt_uint.c

fmt_uint0.o: \
compile fmt_uint0.c fmt.h
	./compile fmt_uint0.c

fmt_ulong.o: \
compile fmt_ulong.c fmt.h
	./compile fmt_ulong.c

fmtqfn.o: \
compile fmtqfn.c fmtqfn.h conf-unusual.h fmt.h
	./compile fmtqfn.c

forgeries.0: \
forgeries.7
	$(NROFF) forgeries.7 > forgeries.0

fork.h: \
tryvfork.c compile load fork.h1 fork.h2
	( ( ./compile tryvfork.c && ./load tryvfork ) >/dev/null 2>&1 \
	&& cat fork.h2 || cat fork.h1 ) > fork.h

forward: \
forward.o signal.o wait.o qqtalk.o libenv.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load forward signal.o wait.o qqtalk.o libenv.a libstralloc.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

forward.0: \
forward.1
	$(NROFF) forward.1 > forward.0

forward.o: \
compile forward.c gen_alloc.h exit.h qqtalk.h substdio.h subfd.h env.h \
readwrite.h stralloc.h signal.h
	./compile forward.c

getline.o: \
compile getline.c substdio.h byte.h alloc.h getline.h stralloc.h \
gen_alloc.h
	./compile getline.c

getline2.o: \
compile getline2.c substdio.h alloc.h byte.h getline.h stralloc.h \
gen_alloc.h
	./compile getline2.c

gfrom.o: \
compile gfrom.c str.h gfrom.h
	./compile gfrom.c

headerbody.o: \
compile headerbody.c gen_alloc.h headerbody.h substdio.h byte.h \
getline.h hfield.h stralloc.h
	./compile headerbody.c

hfield.o: \
compile hfield.c hfield.h
	./compile hfield.c

hostname: \
loaddns hostname.o libsubstdio.a liballoc.a liberror.a libstr.a \
libfs.a
	./loaddns hostname libsubstdio.a liballoc.a liberror.a libstr.a \
	libfs.a

hostname.o: \
compile hostname.c readwrite.h exit.h substdio.h
	./compile hostname.c

ip.o: \
compile ip.c ip.h scan.h fmt.h
	./compile ip.c

ipalloc.o: \
compile ipalloc.c ipalloc.h ip.h alloc.h gen_alloc.h gen_allocdefs.h
	./compile ipalloc.c

ipme.o: \
compile ipme.c ipalloc.h ipme.h ip.h byte.h auto-hassalen.h \
gen_alloc.h
	./compile ipme.c

ipmeprint: \
ipmeprint.o ipme.o ip.o ipalloc.o libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load ipmeprint ipme.o ip.o ipalloc.o libsubstdio.a \
	liballoc.a liberror.a libstr.a libfs.a

ipmeprint.o: \
compile ipmeprint.c ipme.h ipalloc.h ip.h substdio.h subfd.h \
gen_alloc.h exit.h
	./compile ipmeprint.c

liballoc.a: \
alloc.o alloc_re.o makelib
	./makelib liballoc.a alloc.o alloc_re.o

libenv.a: \
env.o envread.o makelib
	./makelib libenv.a env.o envread.o

liberror.a: \
error.o error_temp.o error_str.o makelib
	./makelib liberror.a error.o error_temp.o error_str.o

libfs.a: \
fmt_strn.o fmt_str.o fmt_uint.o fmt_uint0.o fmt_ulong.o scan_nbblong.o \
scan_ulong.o makelib
	./makelib libfs.a fmt_strn.o fmt_str.o fmt_uint.o fmt_uint0.o \
	fmt_ulong.o scan_nbblong.o scan_ulong.o

libgetline.a: \
getline.o getline2.o makelib
	./makelib libgetline.a getline.o getline2.o

libgetopt.a: \
subgetopt.o sgetopt.o makelib
	./makelib libgetopt.a subgetopt.o sgetopt.o

libstr.a: \
str_len.o str_diff.o str_diffn.o str_cpy.o str_chr.o str_rchr.o \
byte_chr.o byte_rchr.o byte_diff.o byte_copy.o byte_cr.o byte_zero.o \
makelib
	./makelib libstr.a str_len.o str_diff.o str_diffn.o str_cpy.o \
	str_chr.o str_rchr.o byte_chr.o byte_rchr.o byte_diff.o \
	byte_copy.o byte_cr.o byte_zero.o

libstralloc.a: \
stralloc_eady.o stralloc_copy.o stralloc_opyb.o stralloc_cat.o \
stralloc_catb.o stralloc_opys.o stralloc_cats.o stralloc_pend.o \
makelib
	./makelib libstralloc.a stralloc_eady.o stralloc_copy.o \
	stralloc_opyb.o stralloc_catb.o stralloc_cat.o stralloc_opys.o \
	stralloc_cats.o stralloc_pend.o

libstrerr.a: \
strerr.o strerr_die.o strerr_sys.o makelib
	./makelib libstrerr.a strerr.o strerr_sys.o strerr_die.o

libsubstdio.a: \
makelib substdio.o substdi.o substdo.o subfderr.o subfdout.o subfdin.o \
substdio_copy.o
	./makelib libsubstdio.a substdio.o substdi.o substdo.o \
	subfderr.o subfdout.o subfdin.o substdio_copy.o

load: \
make-commands systype
	./make-commands "`cat ./systype`" load > load
	chmod 755 load

loaddns: \
make-commands systype
	./make-commands "`cat ./systype`" loaddns > loaddns
	chmod 755 loaddns

lock.o: \
compile lock.c lock.h auto-hasflock.h
	./compile lock.c

maildir.0: \
maildir.5
	$(NROFF) maildir.5 > maildir.0

maildir.o: \
compile maildir.c stralloc.h now.h prioq.h gen_alloc.h maildir.h \
strerr.h direntry.h env.h str.h datetime.h
	./compile maildir.c

maildir2mbox: \
maildir2mbox.o maildir.o prioq.o now.o open.o lock.o myctime.o \
datetime.o gfrom.o libstrerr.a libgetline.a libstralloc.a libenv.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load maildir2mbox prioq.o now.o open.o lock.o myctime.o \
	maildir.o \
	datetime.o gfrom.o \
	libstrerr.a libgetline.a libstralloc.a libenv.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

maildir2mbox.0: \
maildir2mbox.1
	$(NROFF) maildir2mbox.1 > maildir2mbox.0

maildir2mbox.o: \
compile maildir2mbox.c getline.h lock.h env.h stralloc.h readwrite.h \
gfrom.h prioq.h gen_alloc.h exit.h open.h substdio.h error.h maildir.h \
strerr.h subfd.h str.h myctime.h datetime.h
	./compile maildir2mbox.c

maildir2qmtp: \
load maildir2qmtp.o signal.o maildir.o prioq.o open.o ip.o now.o \
wait.o ndelay.o timeoutconn.o timeoutread.o timeoutwrite.o libstrerr.a \
libgetline.a libsubstdio.a libstralloc.a liballoc.a liberror.a \
libstr.a libenv.a libfs.a
	./load maildir2qmtp maildir.o prioq.o open.o \
	signal.o ip.o ndelay.o now.o wait.o \
	timeoutconn.o timeoutread.o timeoutwrite.o \
	libstrerr.a libgetline.a libsubstdio.a libstralloc.a \
	libenv.a liballoc.a liberror.a libstr.a libfs.a

maildir2qmtp.0: \
maildir2qmtp.1
	$(NROFF) maildir2qmtp.1 > maildir2qmtp.0

maildir2qmtp.o: \
compile maildir2qmtp.c ip.h timeoutconn.h substdio.h datetime.h \
error.h signal.h stralloc.h prioq.h gen_alloc.h exit.h subfd.h fmt.h \
wait.h open.h getline.h case.h fork.h maildir.h readwrite.h \
timeoutwrite.h timeoutread.h strerr.h str.h
	./compile maildir2qmtp.c

maildir2smtp: \
load maildir2smtp.o signal.o maildir.o prioq.o open.o case.o quote.o \
ip.o now.o ndelay.o timeoutconn.o timeoutread.o timeoutwrite.o \
libstrerr.a libgetline.a libsubstdio.a libstralloc.a liballoc.a \
liberror.a libstr.a libenv.a libfs.a
	./load maildir2smtp maildir.o prioq.o open.o \
	signal.o case.o quote.o ip.o ndelay.o now.o \
	timeoutconn.o timeoutread.o timeoutwrite.o \
	libstrerr.a libgetline.a libsubstdio.a libstralloc.a \
	libenv.a liballoc.a liberror.a libstr.a libfs.a

maildir2smtp.0: \
maildir2smtp.1
	$(NROFF) maildir2smtp.1 > maildir2smtp.0

maildir2smtp.o: \
compile maildir2smtp.c maildir.h timeoutconn.h timeoutwrite.h \
datetime.h error.h stralloc.h signal.h prioq.h gen_alloc.h exit.h \
subfd.h scan.h quote.h open.h getline.h case.h readwrite.h ip.h \
substdio.h timeoutread.h strerr.h str.h
	./compile maildir2smtp.c

maildirmake: \
maildirmake.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
load
	./load maildirmake libsubstdio.a liballoc.a liberror.a \
	libstr.a libfs.a

maildirmake.0: \
maildirmake.1
	$(NROFF) maildirmake.1 > maildirmake.0

maildirmake.o: \
compile maildirmake.c error.h subfd.h exit.h substdio.h
	./compile maildirmake.c

maildirwatch: \
maildirwatch.o maildir.o prioq.o now.o open.o hfield.o headerbody.o \
libstrerr.a libgetline.a libstralloc.a libenv.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load maildirwatch \
	prioq.o now.o open.o lock.o hfield.o headerbody.o \
	maildir.o \
	libstrerr.a libgetline.a libstralloc.a libenv.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

maildirwatch.0: \
maildirwatch.1
	$(NROFF) maildirwatch.1 > maildirwatch.0

maildirwatch.o: \
compile maildirwatch.c hfield.h stralloc.h prioq.h gen_alloc.h exit.h \
headerbody.h open.h maildir.h substdio.h strerr.h subfd.h getline.h \
str.h readwrite.h datetime.h
	./compile maildirwatch.c

mailsubj: \
warn-auto.sh mailsubj.sh qmail-home
	cat warn-auto.sh mailsubj.sh \
	| sed s}HOME}"`./qmail-home`"}g > mailsubj
	chmod 755 mailsubj

mailsubj.0: \
mailsubj.1
	$(NROFF) mailsubj.1 > mailsubj.0

make-commands: \
make-cmds.sh conf-cc.sh warn-auto.sh
	cat warn-auto.sh conf-cc.sh make-cmds.sh > make-commands
	chmod 755 make-commands

makelib: \
make-commands systype
	./make-commands "`cat ./systype`" makelib > makelib
	chmod 755 makelib

man: \
addresses.0 envelopes.0 forgeries.0 forward.0 condredirect.0 \
dot-qmail.0 maildir.0 maildirmake.0 maildir2mbox.0 maildir2qmtp.0 \
maildir2smtp.0 maildirwatch.0 mailsubj.0 mbox.0 preline.0 qbiff.0 \
qlist.0 qmail.0 qreceipt.0 qmail-popup.0 qmail-pop3d.0 qmail-alias.0 \
qmail-clean.0 qmail-command.0 qmail-control.0 qmail-header.0 \
qmail-inject.0 qmail-limits.0 qmail-log.0 qmail-lspawn.0 qmail-qread.0 \
qmail-qstat.0 qmail-tcpto.0 qmail-queue.0 qmail-remote.0 \
qmail-rspawn.0 qmail-send.0 qmail-qmtpd.0 qmail-smtpd.0 qmail-start.0 \
qmail-showctl.0 qmail-upgrade.0 tcp-env.0 tcp-environ.0

mbox.0: \
mbox.5
	$(NROFF) mbox.5 > mbox.0

myctime.o: \
compile myctime.c myctime.h datetime.h fmt.h
	./compile myctime.c

ndelay.o: \
compile ndelay.c ndelay.h
	./compile ndelay.c

new.gjhe:

newfield.o: \
compile newfield.c fmt.h alloc.h date822fmt.h datetime.h newfield.h
	./compile newfield.c

now.o: \
compile now.c datetime.h now.h
	./compile now.c

open.o: \
compile open.c open.h
	./compile open.c

pinq: \
pinq.sh qmail-home warn-auto.sh
	cat warn-auto.sh pinq.sh \
	| sed s}HOME}"`./qmail-home`"}g > pinq
	chmod 755 pinq

preline: \
load preline.o wait.o signal.o libenv.a libgetopt.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a
	./load preline wait.o signal.o \
	libenv.a libgetopt.a libsubstdio.a \
	liballoc.a liberror.a libstr.a libfs.a

preline.0: \
preline.1
	$(NROFF) preline.1 > preline.0

preline.o: \
compile preline.c exit.h substdio.h error.h subfd.h wait.h fork.h \
env.h readwrite.h subgetopt.h signal.h sgetopt.h
	./compile preline.c

printbreak: \
printbreak.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printbreak libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printbreak.o: \
compile printbreak.c conf-home.h substdio.h readwrite.h exit.h \
conf-unusual.h
	./compile printbreak.c

printnumd: \
printnumd.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printnumd libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printnumd.o: \
compile printnumd.c fmt.h conf-home.h substdio.h readwrite.h exit.h \
conf-unusual.h
	./compile printnumd.c

printsplit: \
printsplit.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printsplit libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printsplit.o: \
compile printsplit.c fmt.h conf-home.h substdio.h readwrite.h exit.h \
conf-unusual.h
	./compile printsplit.c

printuids: \
printuids.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printuids libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printuids.o: \
compile printuids.c fmt.h substdio.h subfd.h exit.h conf-user.h
	./compile printuids.c

prioq.o: \
compile prioq.c prioq.h gen_allocdefs.h alloc.h datetime.h gen_alloc.h
	./compile prioq.c

prot.o: \
compile prot.c prot.h auto-hasshsgr.h
	./compile prot.c

qail: \
qail.sh qmail-home warn-auto.sh
	cat warn-auto.sh qail.sh \
	| sed s}HOME}"`./qmail-home`"}g > qail
	chmod 755 qail

qbiff: \
qbiff.o open.o headerbody.o hfield.o libgetline.a libenv.a \
libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
load
	./load qbiff open.o headerbody.o hfield.o libgetline.a \
	libenv.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
	libstr.a libfs.a

qbiff.0: \
qbiff.1
	$(NROFF) qbiff.1 > qbiff.0

qbiff.o: \
compile qbiff.c stralloc.h gen_alloc.h exit.h headerbody.h open.h \
substdio.h subfd.h byte.h env.h readwrite.h str.h hfield.h
	./compile qbiff.c

qlist: \
qlist.o signal.o hfield.o token822.o headerbody.o open.o lock.o wait.o \
qqtalk.o case.o libenv.a libgetline.a libstralloc.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load qlist signal.o hfield.o token822.o headerbody.o open.o case.o \
	lock.o wait.o qqtalk.o libenv.a libgetline.a libstralloc.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qlist.0: \
qlist.1
	$(NROFF) qlist.1 > qlist.0

qlist.o: \
compile qlist.c alloc.h lock.h getline.h stralloc.h env.h signal.h \
gen_alloc.h exit.h headerbody.h qqtalk.h token822.h gen_allocdefs.h \
readwrite.h open.h case.h substdio.h error.h str.h subfd.h hfield.h
	./compile qlist.c

qlist2: \
qlist2.sh qmail-home printbreak warn-auto.sh
	cat warn-auto.sh qlist2.sh \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}HOME}"`./qmail-home`"}g > qlist2
	chmod 755 qlist2

qmail-alias: \
qmail-alias.o signal.o now.o seek.o open.o lock.o qqtalk.o wait.o \
myctime.o datetime.o quote.o gfrom.o case.o libenv.a libgetopt.a \
libgetline.a libsubstdio.a libstralloc.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load qmail-alias signal.o now.o seek.o open.o lock.o \
	qqtalk.o wait.o myctime.o datetime.o quote.o gfrom.o case.o \
	libenv.a libgetopt.a libgetline.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-alias.0: \
qmail-alias.8
	$(NROFF) qmail-alias.8 > qmail-alias.0

qmail-alias.o: \
compile qmail-alias.c getline.h case.h lock.h myctime.h open.h seek.h \
env.h error.h stralloc.h fork.h sgetopt.h signal.h gfrom.h now.h \
substdio.h gen_alloc.h readwrite.h wait.h conf-unusual.h subgetopt.h \
subfd.h exit.h byte.h qqtalk.h quote.h fmt.h str.h alloc.h qlx.h \
datetime.h
	./compile qmail-alias.c

qmail-check: \
qmail-check.o open.o libsubstdio.a libstralloc.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load qmail-check open.o libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-check.o: \
compile qmail-check.c fmt.h conf-home.h auto-uids.h open.h error.h \
substdio.h subfd.h conf-unusual.h exit.h
	./compile qmail-check.c

qmail-clean: \
qmail-clean.o now.o signal.o fmtqfn.o libgetline.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load qmail-clean now.o signal.o fmtqfn.o libgetline.a \
	libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qmail-clean.0: \
qmail-clean.8
	$(NROFF) qmail-clean.8 > qmail-clean.0

qmail-clean.o: \
compile qmail-clean.c direntry.h stralloc.h readwrite.h signal.h now.h \
gen_alloc.h exit.h scan.h fmt.h conf-home.h fmtqfn.h substdio.h \
error.h byte.h str.h getline.h datetime.h
	./compile qmail-clean.c

qmail-command.0: \
qmail-command.8
	$(NROFF) qmail-command.8 > qmail-command.0

qmail-control.0: \
qmail-control.5
	$(NROFF) qmail-control.5 > qmail-control.0

qmail-header.0: \
qmail-header.5
	$(NROFF) qmail-header.5 > qmail-header.0

qmail-home: \
qmail-home.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load qmail-home libsubstdio.a liballoc.a liberror.a libstr.a \
	libfs.a

qmail-home.o: \
compile qmail-home.c substdio.h readwrite.h exit.h conf-home.h
	./compile qmail-home.c

qmail-inject: \
qmail-inject.o hfield.o newfield.o datetime.o date822fmt.o now.o \
signal.o quote.o token822.o control.o open.o headerbody.o qqtalk.o \
wait.o libenv.a libgetopt.a libgetline.a libsubstdio.a libstralloc.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load qmail-inject hfield.o newfield.o datetime.o \
	date822fmt.o now.o signal.o quote.o token822.o control.o open.o \
	headerbody.o qqtalk.o wait.o libenv.a libgetopt.a libgetline.a \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

qmail-inject.0: \
qmail-inject.8
	$(NROFF) qmail-inject.8 > qmail-inject.0

qmail-inject.o: \
compile qmail-inject.c token822.h error.h hfield.h control.h \
stralloc.h substdio.h sgetopt.h signal.h gen_alloc.h now.h newfield.h \
subfd.h exit.h alloc.h qqtalk.h quote.h headerbody.h getline.h \
gen_allocdefs.h env.h fmt.h str.h subgetopt.h conf-home.h datetime.h
	./compile qmail-inject.c

qmail-limits.0: \
qmail-limits.7
	$(NROFF) qmail-limits.7 > qmail-limits.0

qmail-limits.7: \
qmail-limits.9 printbreak printnumd
	cat qmail-limits.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> qmail-limits.7

qmail-log.0: \
qmail-log.5
	$(NROFF) qmail-log.5 > qmail-log.0

qmail-lspawn: \
qmail-lspawn.o coe.o open.o spawn.o wait.o case.o prot.o signal.o \
libgetline.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load qmail-lspawn coe.o open.o spawn.o wait.o case.o prot.o \
	signal.o libgetline.a libstralloc.a libsubstdio.a liballoc.a \
	liberror.a libstr.a libfs.a

qmail-lspawn.0: \
qmail-lspawn.8
	$(NROFF) qmail-lspawn.8 > qmail-lspawn.0

qmail-lspawn.8: \
qmail-lspawn.9 printbreak printnumd
	cat qmail-lspawn.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> qmail-lspawn.8

qmail-lspawn.o: \
compile qmail-lspawn.c exit.h conf-unusual.h conf-user.h conf-home.h \
auto-uids.h case.h qlx.h substdio.h error.h wait.h byte.h fork.h str.h \
prot.h
	./compile qmail-lspawn.c

qmail-makectl: \
warn-auto.sh qmail-mctl.sh qmail-home
	cat warn-auto.sh qmail-mctl.sh \
	| sed s}HOME}"`./qmail-home`"}g > qmail-makectl
	chmod 755 qmail-makectl

qmail-pop3d: \
load qmail-pop3d.o signal.o datetime.o open.o prioq.o lock.o now.o \
libgetline.a libsubstdio.a libstralloc.a liballoc.a liberror.a \
libstr.a libfs.a
	./load qmail-pop3d signal.o datetime.o open.o prioq.o \
	lock.o now.o libgetline.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-pop3d.0: \
qmail-pop3d.8
	$(NROFF) qmail-pop3d.8 > qmail-pop3d.0

qmail-pop3d.o: \
compile qmail-pop3d.c alloc.h datetime.h getline.h prot.h direntry.h \
stralloc.h signal.h now.h prioq.h gen_alloc.h readwrite.h exit.h \
scan.h fmt.h open.h substdio.h str.h error.h lock.h
	./compile qmail-pop3d.c

qmail-popup: \
load qmail-popup.o signal.o datetime.o wait.o now.o libgetline.a \
libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a
	./load qmail-popup signal.o datetime.o wait.o now.o \
	libgetline.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-popup.0: \
qmail-popup.8
	$(NROFF) qmail-popup.8 > qmail-popup.0

qmail-popup.o: \
compile qmail-popup.c stralloc.h signal.h now.h gen_alloc.h exit.h \
fmt.h substdio.h error.h wait.h alloc.h getline.h readwrite.h str.h \
datetime.h
	./compile qmail-popup.c

qmail-qmtpd: \
load qmail-qmtpd.o qqtalk.o now.o wait.o signal.o datetime.o \
date822fmt.o control.o constmap.o open.o case.o libgetline.a \
libstralloc.a libsubstdio.a libenv.a liballoc.a liberror.a libstr.a \
libfs.a
	./load qmail-qmtpd \
	qqtalk.o now.o wait.o signal.o datetime.o date822fmt.o \
	control.o constmap.o open.o case.o \
	libgetline.a libstralloc.a libsubstdio.a \
	libenv.a liballoc.a liberror.a libstr.a libfs.a

qmail-qmtpd.0: \
qmail-qmtpd.8
	$(NROFF) qmail-qmtpd.8 > qmail-qmtpd.0

qmail-qmtpd.o: \
compile qmail-qmtpd.c signal.h stralloc.h now.h gen_alloc.h qqtalk.h \
fmt.h conf-home.h constmap.h control.h substdio.h env.h date822fmt.h \
str.h readwrite.h datetime.h
	./compile qmail-qmtpd.c

qmail-qread: \
load qmail-qread.o fmtqfn.o readsubdir.o open.o datetime.o \
date822fmt.o libgetline.a libsubstdio.a libstralloc.a liballoc.a \
liberror.a libstr.a libfs.a
	./load qmail-qread fmtqfn.o readsubdir.o open.o datetime.o \
	date822fmt.o libgetline.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-qread.0: \
qmail-qread.8
	$(NROFF) qmail-qread.8 > qmail-qread.0

qmail-qread.o: \
compile qmail-qread.c date822fmt.h stralloc.h readwrite.h gen_alloc.h \
exit.h fmt.h conf-home.h fmtqfn.h open.h error.h substdio.h subfd.h \
readsubdir.h getline.h str.h direntry.h datetime.h
	./compile qmail-qread.c

qmail-qstat: \
warn-auto.sh qmail-qstat.sh qmail-home
	cat warn-auto.sh qmail-qstat.sh \
	| sed s}HOME}"`./qmail-home`"}g > qmail-qstat
	chmod 755 qmail-qstat

qmail-qstat.0: \
qmail-qstat.8
	$(NROFF) qmail-qstat.8 > qmail-qstat.0

qmail-queue: \
qmail-queue.o signal.o triggerpull.o datetime.o fmtqfn.o date822fmt.o \
now.o ndelay.o open.o seek.o libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load qmail-queue signal.o triggerpull.o datetime.o fmtqfn.o \
	date822fmt.o now.o ndelay.o open.o seek.o libsubstdio.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-queue.0: \
qmail-queue.8
	$(NROFF) qmail-queue.8 > qmail-queue.0

qmail-queue.o: \
compile qmail-queue.c readwrite.h signal.h now.h triggerpull.h exit.h \
fmt.h conf-home.h auto-uids.h fmtqfn.h open.h substdio.h qqx.h alloc.h \
seek.h date822fmt.h datetime.h
	./compile qmail-queue.c

qmail-remote: \
qmail-remote.o signal.o dns.o ipme.o ip.o tcpto.o lock.o seek.o \
ndelay.o ipalloc.o control.o constmap.o now.o open.o case.o quote.o \
timeoutread.o timeoutwrite.o timeoutconn.o libgetline.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns qmail-remote signal.o control.o dns.o ipme.o ip.o \
	tcpto.o lock.o seek.o ndelay.o ipalloc.o now.o open.o case.o \
	quote.o timeoutread.o timeoutwrite.o timeoutconn.o constmap.o \
	libgetline.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
	libstr.a libfs.a

qmail-remote.0: \
qmail-remote.8
	$(NROFF) qmail-remote.8 > qmail-remote.0

qmail-remote.o: \
compile qmail-remote.c alloc.h ipme.h timeoutread.h quote.h case.h \
str.h getline.h timeoutwrite.h datetime.h stralloc.h signal.h \
gen_alloc.h now.h substdio.h exit.h error.h scan.h control.h \
gen_allocdefs.h ip.h conf-home.h subfd.h constmap.h dns.h tcpto.h \
ipalloc.h timeoutconn.h
	./compile qmail-remote.c

qmail-rspawn: \
qmail-rspawn.o coe.o open.o spawn.o wait.o signal.o tcpto_clean.o \
libgetline.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load qmail-rspawn spawn.o signal.o tcpto_clean.o coe.o \
	open.o wait.o libgetline.a libstralloc.a libsubstdio.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-rspawn.0: \
qmail-rspawn.8
	$(NROFF) qmail-rspawn.8 > qmail-rspawn.0

qmail-rspawn.o: \
compile qmail-rspawn.c error.h tcpto.h wait.h fork.h exit.h substdio.h
	./compile qmail-rspawn.c

qmail-send: \
qmail-send.o trigger.o control.o qsutil.o signal.o constmap.o quote.o \
fmtqfn.o newfield.o now.o datetime.o date822fmt.o seek.o ndelay.o \
readsubdir.o open.o lock.o case.o prioq.o qqtalk.o wait.o libgetline.a \
libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
load
	./load qmail-send trigger.o control.o qsutil.o signal.o \
	constmap.o quote.o fmtqfn.o newfield.o now.o datetime.o \
	date822fmt.o seek.o ndelay.o readsubdir.o open.o lock.o case.o \
	prioq.o qqtalk.o wait.o libgetline.a libstralloc.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qmail-send.0: \
qmail-send.8
	$(NROFF) qmail-send.8 > qmail-send.0

qmail-send.8: \
qmail-send.9 printbreak printnumd
	cat qmail-send.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> qmail-send.8

qmail-send.o: \
compile qmail-send.c readwrite.h control.h qqtalk.h seek.h substdio.h \
qsutil.h lock.h select.h fmt.h ndelay.h readsubdir.h stralloc.h \
trigger.h signal.h prioq.h conf-home.h now.h quote.h open.h \
gen_alloc.h byte.h constmap.h exit.h alloc.h fmtqfn.h conf-unusual.h \
direntry.h case.h newfield.h str.h getline.h error.h scan.h datetime.h
	./compile qmail-send.c

qmail-setup: \
qmail-setup.o open.o fifo.o libsubstdio.a libstralloc.a liballoc.a \
liberror.a libstr.a libfs.a load
	./load qmail-setup open.o fifo.o libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-setup.o: \
compile qmail-setup.c exit.h conf-unusual.h fmt.h conf-home.h \
auto-uids.h open.h substdio.h fifo.h error.h subfd.h readwrite.h \
stralloc.h gen_alloc.h
	./compile qmail-setup.c

qmail-showctl: \
qmail-sctl.sh qmail-home warn-auto.sh
	cat warn-auto.sh qmail-sctl.sh \
	| sed s}HOME}"`./qmail-home`"}g > qmail-showctl
	chmod 755 qmail-showctl

qmail-showctl.0: \
qmail-showctl.8
	$(NROFF) qmail-showctl.8 > qmail-showctl.0

qmail-smtpd: \
qmail-smtpd.o ipme.o ip.o ipalloc.o control.o newfield.o datetime.o \
date822fmt.o signal.o now.o open.o case.o qqtalk.o wait.o constmap.o \
libgetline.a libstralloc.a libsubstdio.a libenv.a liballoc.a \
liberror.a libstr.a libfs.a load
	./load qmail-smtpd ipme.o ip.o ipalloc.o control.o newfield.o \
	datetime.o date822fmt.o signal.o now.o open.o case.o qqtalk.o \
	wait.o constmap.o libgetline.a libstralloc.a libsubstdio.a \
	libenv.a liballoc.a liberror.a libstr.a libfs.a

qmail-smtpd.0: \
qmail-smtpd.8
	$(NROFF) qmail-smtpd.8 > qmail-smtpd.0

qmail-smtpd.o: \
compile qmail-smtpd.c substdio.h stralloc.h control.h signal.h error.h \
now.h gen_alloc.h newfield.h exit.h qqtalk.h ip.h fmt.h byte.h \
conf-home.h alloc.h case.h getline.h constmap.h env.h ipme.h str.h \
readwrite.h ipalloc.h datetime.h
	./compile qmail-smtpd.c

qmail-start: \
qmail-start.o prot.o load
	./load qmail-start prot.o

qmail-start.0: \
qmail-start.8
	$(NROFF) qmail-start.8 > qmail-start.0

qmail-start.o: \
compile qmail-start.c auto-uids.h fork.h prot.h exit.h
	./compile qmail-start.c

qmail-tcpto: \
load qmail-tcpto.o now.o lock.o open.o ip.o liberror.a libsubstdio.a \
libfs.a libstr.a
	./load qmail-tcpto now.o open.o lock.o ip.o \
	liberror.a libsubstdio.a libfs.a libstr.a

qmail-tcpto.0: \
qmail-tcpto.8
	$(NROFF) qmail-tcpto.8 > qmail-tcpto.0

qmail-tcpto.o: \
compile qmail-tcpto.c now.h exit.h conf-home.h fmt.h ip.h error.h \
substdio.h subfd.h lock.h datetime.h
	./compile qmail-tcpto.c

qmail-upgrade.0: \
qmail-upgrade.7
	$(NROFF) qmail-upgrade.7 > qmail-upgrade.0

qmail-upgrade.7: \
qmail-upgrade.9 printbreak printnumd
	cat qmail-upgrade.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> qmail-upgrade.7

qmail-upq: \
qmail-upq.sh qmail-home printsplit warn-auto.sh
	cat warn-auto.sh qmail-upq.sh \
	| sed s}CONF-SPLIT}"`./printsplit`"}g \
	| sed s}HOME}"`./qmail-home`"}g > qmail-upq
	chmod 755 qmail-upq

qmail.0: \
qmail.7
	$(NROFF) qmail.7 > qmail.0

qqtalk.o: \
compile qqtalk.c substdio.h error.h qqx.h wait.h fork.h readwrite.h \
exit.h qqtalk.h
	./compile qqtalk.c

qreceipt: \
qreceipt.o signal.o hfield.o token822.o headerbody.o open.o wait.o \
quote.o qqtalk.o libenv.a libgetline.a libstralloc.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load qreceipt signal.o hfield.o token822.o headerbody.o open.o \
	wait.o quote.o qqtalk.o libenv.a libgetline.a libstralloc.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qreceipt.0: \
qreceipt.1
	$(NROFF) qreceipt.1 > qreceipt.0

qreceipt.o: \
compile qreceipt.c getline.h stralloc.h token822.h signal.h env.h \
gen_alloc.h exit.h qqtalk.h headerbody.h gen_allocdefs.h open.h \
substdio.h error.h subfd.h quote.h str.h alloc.h hfield.h
	./compile qreceipt.c

qsmhook: \
qsmhook.o wait.o case.o signal.o libstralloc.a libenv.a libgetopt.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load qsmhook wait.o case.o signal.o libstralloc.a libenv.a \
	libgetopt.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qsmhook.o: \
compile qsmhook.c env.h sgetopt.h fork.h stralloc.h gen_alloc.h exit.h \
case.h error.h substdio.h wait.h subfd.h byte.h alloc.h readwrite.h \
str.h subgetopt.h signal.h
	./compile qsmhook.c

qsutil.o: \
compile qsutil.c qsutil.h substdio.h readwrite.h stralloc.h \
gen_alloc.h
	./compile qsutil.c

quote.o: \
compile quote.c gen_alloc.h quote.h str.h stralloc.h
	./compile quote.c

readsubdir.o: \
compile readsubdir.c scan.h fmt.h readsubdir.h direntry.h str.h \
conf-unusual.h
	./compile readsubdir.c

remoteinfo.o: \
compile remoteinfo.c fmt.h substdio.h ip.h byte.h readwrite.h \
timeoutconn.h remoteinfo.h
	./compile remoteinfo.c

scan_nbblong.o: \
compile scan_nbblong.c scan.h
	./compile scan_nbblong.c

scan_ulong.o: \
compile scan_ulong.c scan.h
	./compile scan_ulong.c

seek.o: \
compile seek.c seek.h
	./compile seek.c

select.h: \
trysysel.c compile select.h1 select.h2
	( ./compile trysysel.c >/dev/null 2>&1 \
	&& cat select.h2 || cat select.h1 ) > select.h

sendmail: \
sendmail.o libenv.a libgetopt.a libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load sendmail libenv.a libgetopt.a libsubstdio.a liballoc.a \
	liberror.a libstr.a libfs.a

sendmail.o: \
compile sendmail.c exit.h conf-home.h substdio.h subfd.h alloc.h env.h \
subgetopt.h str.h sgetopt.h
	./compile sendmail.c

setup: \
do-setup

sgetopt.o: \
compile sgetopt.c subfd.h subgetopt.h sgetopt.h substdio.h
	./compile sgetopt.c

shar: \
do-shar

signal.o: \
compile signal.c auto-hassgprm.h auto-hassgact.h signal.h
	./compile signal.c

spawn.o: \
chkspawn compile spawn.c getline.h stralloc.h gen_alloc.h exit.h coe.h \
conf-unusual.h conf-home.h auto-uids.h open.h substdio.h error.h \
select.h wait.h str.h byte.h signal.h
	./chkspawn
	./compile spawn.c

str_chr.o: \
compile str_chr.c str.h
	./compile str_chr.c

str_cpy.o: \
compile str_cpy.c str.h
	./compile str_cpy.c

str_diff.o: \
compile str_diff.c str.h
	./compile str_diff.c

str_diffn.o: \
compile str_diffn.c str.h
	./compile str_diffn.c

str_len.o: \
compile str_len.c str.h
	./compile str_len.c

str_rchr.o: \
compile str_rchr.c str.h
	./compile str_rchr.c

stralloc_cat.o: \
compile stralloc_cat.c byte.h stralloc.h gen_alloc.h
	./compile stralloc_cat.c

stralloc_catb.o: \
compile stralloc_catb.c byte.h stralloc.h gen_alloc.h
	./compile stralloc_catb.c

stralloc_cats.o: \
compile stralloc_cats.c gen_alloc.h byte.h str.h stralloc.h
	./compile stralloc_cats.c

stralloc_copy.o: \
compile stralloc_copy.c byte.h stralloc.h gen_alloc.h
	./compile stralloc_copy.c

stralloc_eady.o: \
compile stralloc_eady.c gen_allocdefs.h alloc.h stralloc.h gen_alloc.h
	./compile stralloc_eady.c

stralloc_opyb.o: \
compile stralloc_opyb.c byte.h stralloc.h gen_alloc.h
	./compile stralloc_opyb.c

stralloc_opys.o: \
compile stralloc_opys.c gen_alloc.h byte.h str.h stralloc.h
	./compile stralloc_opys.c

stralloc_pend.o: \
compile stralloc_pend.c gen_allocdefs.h alloc.h stralloc.h gen_alloc.h
	./compile stralloc_pend.c

strerr.o: \
compile strerr.c strerr.h stralloc.h gen_alloc.h
	./compile strerr.c

strerr_die.o: \
compile strerr_die.c strerr.h subfd.h exit.h substdio.h
	./compile strerr_die.c

strerr_sys.o: \
compile strerr_sys.c error.h strerr.h
	./compile strerr_sys.c

subfderr.o: \
compile subfderr.c readwrite.h substdio.h subfd.h
	./compile subfderr.c

subfdin.o: \
compile subfdin.c readwrite.h substdio.h subfd.h
	./compile subfdin.c

subfdout.o: \
compile subfdout.c readwrite.h substdio.h subfd.h
	./compile subfdout.c

subgetopt.o: \
compile subgetopt.c subgetopt.h
	./compile subgetopt.c

substdi.o: \
compile substdi.c byte.h error.h substdio.h
	./compile substdi.c

substdio.o: \
compile substdio.c substdio.h
	./compile substdio.c

substdio_copy.o: \
compile substdio_copy.c substdio.h
	./compile substdio_copy.c

substdo.o: \
compile substdo.c error.h byte.h str.h substdio.h
	./compile substdo.c

systype: \
find-systype trycpp.c
	./find-systype > systype

tcp-env: \
tcp-env.o dns.o ip.o ipalloc.o remoteinfo.o timeoutconn.o case.o \
signal.o ndelay.o libgetopt.a libenv.a libsubstdio.a libstralloc.a \
liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns tcp-env dns.o ip.o ipalloc.o remoteinfo.o case.o ndelay.o \
	timeoutconn.o signal.o libgetopt.a libenv.a libsubstdio.a \
	libstralloc.a liballoc.a liberror.a libstr.a libfs.a

tcp-env.0: \
tcp-env.1
	$(NROFF) tcp-env.1 > tcp-env.0

tcp-env.o: \
compile tcp-env.c remoteinfo.h signal.h stralloc.h gen_alloc.h exit.h \
scan.h fmt.h case.h ip.h byte.h env.h subgetopt.h str.h dns.h
	./compile tcp-env.c

tcp-environ.0: \
tcp-environ.5
	$(NROFF) tcp-environ.5 > tcp-environ.0

tcpto.o: \
compile tcpto.c lock.h now.h open.h ip.h tcpto.h byte.h readwrite.h \
seek.h datetime.h
	./compile tcpto.c

tcpto_clean.o: \
compile tcpto_clean.c tcpto.h readwrite.h open.h substdio.h
	./compile tcpto_clean.c

timeoutconn.o: \
compile timeoutconn.c error.h ip.h select.h readwrite.h timeoutconn.h \
ndelay.h
	./compile timeoutconn.c

timeoutread.o: \
compile timeoutread.c select.h readwrite.h timeoutread.h error.h
	./compile timeoutread.c

timeoutwrite.o: \
compile timeoutwrite.c select.h readwrite.h error.h timeoutwrite.h
	./compile timeoutwrite.c

token822.o: \
compile token822.c gen_alloc.h gen_allocdefs.h alloc.h token822.h \
str.h stralloc.h
	./compile token822.c

tokenize: \
tokenize.o token822.o libgetline.a libsubstdio.a libstralloc.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load tokenize token822.o libgetline.a libsubstdio.a \
	libstralloc.a liballoc.a liberror.a libstr.a libfs.a

tokenize.o: \
compile tokenize.c substdio.h error.h subfd.h alloc.h getline.h \
token822.h stralloc.h gen_alloc.h
	./compile tokenize.c

trigger.o: \
compile trigger.c open.h select.h trigger.h auto-hasnpbg1.h
	./compile trigger.c

triggerpull.o: \
compile triggerpull.c open.h ndelay.h triggerpull.h
	./compile triggerpull.c

wait.o: \
compile wait.c error.h auto-haswaitp.h
	./compile wait.c
