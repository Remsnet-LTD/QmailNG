# Don't edit Makefile!
# If you want to use a different compiler, change conf-cc.sh.

NROFF=nroff -man
SHELL=/bin/sh

default: all

addresses.0: \
addresses.5
	$(NROFF) addresses.5 > addresses.0

all: \
qmail-alias qmail-clean qmail-home qmail-inject qmail-lspawn \
qmail-newu qmail-pw2u qmail-getpw qmail-queue qmail-remote \
qmail-rspawn qmail-send qmail-setup qmail-qmtpd qmail-smtpd \
qmail-makectl qmail-showctl qmail-check qmail-setup qmail-start \
qmail-pop3d qmail-popup tokenize dnscname dnsptr dnsip dnsmxip dnsfq \
hostname ipmeprint tcp-env sendmail qmail-qread qmail-qstat \
qmail-tcpto qlist qlist2 qreceipt qsmhook qbiff forward preline \
predate datemail condredirect maildirmake maildir2mbox maildirwatch \
mailsubj splogger qail elq pinq qmail-upq

alloc.o: \
compile alloc.c error.h alloc.h
	./compile alloc.c

alloc_re.o: \
compile alloc_re.c byte.h alloc.h
	./compile alloc_re.c

auto-hasflock.h: \
tryflock.c compile load
	( ( ./compile tryflock.c && ./load tryflock ) >/dev/null 2>&1 \
	&& echo \#define HASFLOCK 1 || exit 0 ) > auto-hasflock.h
	rm -f tryflock.o tryflock

auto-hasnpbg1.h: \
trynpbg1.c compile load open.h libopen.a fifo.h fifo.o select.h
	( ( ./compile trynpbg1.c \
	&& ./load trynpbg1 fifo.o libopen.a && ./trynpbg1 ) >/dev/null 2>&1 \
	&& echo \#define HASNAMEDPIPEBUG1 1 || exit 0 ) > auto-hasnpbg1.h
	rm -f trynpbg1.o trynpbg1

auto-hassalen.h: \
trysalen.c compile
	( ./compile trysalen.c >/dev/null 2>&1 \
	&& echo \#define HASSALEN 1 || exit 0 ) > auto-hassalen.h
	rm -f trysalen.o

auto-hasshsgr.h: \
tryshsgr.c chkshsgr compile load
	./chkshsgr || ( cat warn-shsgr; exit 1 )
	( ( ./compile tryshsgr.c \
	&& ./load tryshsgr && ./tryshsgr ) >/dev/null 2>&1 \
	&& echo \#define HASSHORTSETGROUPS 1 || exit 0 ) > auto-hasshsgr.h
	rm -f tryshsgr.o tryshsgr

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

case_diffb.o: \
compile case_diffb.c case.h
	./compile case_diffb.c

case_diffs.o: \
compile case_diffs.c case.h
	./compile case_diffs.c

case_lowerb.o: \
compile case_lowerb.c case.h
	./compile case_lowerb.c

case_lowers.o: \
compile case_lowers.c case.h
	./compile case_lowers.c

cdb_hash.o: \
compile cdb_hash.c uint32.h cdb.h
	./compile cdb_hash.c

cdb_seek.o: \
compile cdb_seek.c uint32.h cdb.h
	./compile cdb_seek.c

cdb_unpack.o: \
compile cdb_unpack.c uint32.h cdb.h
	./compile cdb_unpack.c

cdbmake_add.o: \
compile cdbmake_add.c uint32.h cdbmake.h
	./compile cdbmake_add.c

cdbmake_hash.o: \
compile cdbmake_hash.c uint32.h cdbmake.h
	./compile cdbmake_hash.c

cdbmake_pack.o: \
compile cdbmake_pack.c uint32.h cdbmake.h
	./compile cdbmake_pack.c

cdbmss.o: \
compile cdbmss.c substdio.h cdbmake.h alloc.h readwrite.h cdbmss.h \
seek.h uint32.h
	./compile cdbmss.c

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
compile chkspawn.c fmt.h substdio.h subfd.h select.h conf-unusual.h \
exit.h
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
condredirect.o signal.o libwait.a qqtalk.o libseek.a libenv.a \
libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
libfd.a load
	./load condredirect signal.o qqtalk.o libenv.a \
	libfd.a \
	libseek.a \
	libwait.a \
	libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

condredirect.0: \
condredirect.1
	$(NROFF) condredirect.1 > condredirect.0

condredirect.o: \
compile condredirect.c stralloc.h gen_alloc.h exit.h qqtalk.h \
substdio.h error.h subfd.h wait.h readwrite.h fork.h env.h seek.h \
signal.h
	./compile condredirect.c

constmap.o: \
compile constmap.c alloc.h constmap.h case.h
	./compile constmap.c

control.o: \
compile control.c scan.h open.h control.h error.h substdio.h alloc.h \
getline.h readwrite.h stralloc.h gen_alloc.h
	./compile control.c

date822fmt.o: \
compile date822fmt.c date822fmt.h datetime.h fmt.h
	./compile date822fmt.c

datemail: \
datemail.sh qmail-home warn-auto.sh
	cat warn-auto.sh datemail.sh \
	| sed s}HOME}"`./qmail-home`"}g > datemail
	chmod 755 datemail

datetime.o: \
compile datetime.c datetime.h
	./compile datetime.c

datetime_un.o: \
compile datetime_un.c datetime.h
	./compile datetime_un.c

direntry.h: \
compile trydrent.c direntry.h1 direntry.h2
	( ./compile trydrent.c >/dev/null 2>&1 \
	&& cat direntry.h2 || cat direntry.h1 ) > direntry.h
	rm -f trydrent.o

dns.o: \
compile dns.c stralloc.h gen_alloc.h fmt.h ipalloc.h case.h ip.h \
alloc.h dns.h str.h
	./compile dns.c

dnscname: \
dnscname.o dns.o dnsdoe.o ip.o ipalloc.o libcase.a libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnscname dns.o dnsdoe.o ip.o ipalloc.o \
	libcase.a \
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
dnsfq.o dns.o dnsdoe.o ip.o ipalloc.o libcase.a libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsfq dns.o dnsdoe.o ip.o ipalloc.o \
	libcase.a \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsfq.o: \
compile dnsfq.c gen_alloc.h exit.h dnsdoe.h ipalloc.h substdio.h ip.h \
subfd.h dns.h stralloc.h
	./compile dnsfq.c

dnsip: \
dnsip.o dns.o dnsdoe.o ip.o ipalloc.o libcase.a libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsip dns.o dnsdoe.o ip.o ipalloc.o \
	libcase.a \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsip.o: \
compile dnsip.c gen_alloc.h exit.h dnsdoe.h ipalloc.h substdio.h ip.h \
subfd.h dns.h stralloc.h
	./compile dnsip.c

dnsmxip: \
dnsmxip.o dns.o dnsdoe.o ip.o ipalloc.o now.o libcase.a libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsmxip dns.o dnsdoe.o ip.o ipalloc.o now.o \
	libcase.a \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsmxip.o: \
compile dnsmxip.c stralloc.h gen_alloc.h now.h exit.h dnsdoe.h fmt.h \
ipalloc.h substdio.h ip.h subfd.h dns.h datetime.h
	./compile dnsmxip.c

dnsptr: \
dnsptr.o dns.o dnsdoe.o ip.o ipalloc.o libcase.a libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a loaddns
	./loaddns dnsptr dns.o dnsdoe.o ip.o ipalloc.o \
	libcase.a \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

dnsptr.o: \
compile dnsptr.c stralloc.h gen_alloc.h exit.h scan.h dnsdoe.h \
substdio.h ip.h subfd.h dns.h str.h
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

fd_copy.o: \
compile fd_copy.c fd.h
	./compile fd_copy.c

fd_move.o: \
compile fd_move.c fd.h
	./compile fd_move.c

fifo.o: \
compile fifo.c hasmkffo.h fifo.h
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
	rm -f tryvfork.o tryvfork

forward: \
forward.o signal.o libwait.a qqtalk.o libenv.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a libfd.a load
	./load forward signal.o qqtalk.o libenv.a libstralloc.a \
	libfd.a \
	libwait.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

forward.0: \
forward.1
	$(NROFF) forward.1 > forward.0

forward.o: \
compile forward.c gen_alloc.h exit.h qqtalk.h substdio.h subfd.h \
readwrite.h env.h signal.h stralloc.h
	./compile forward.c

getline.o: \
compile getline.c substdio.h alloc.h byte.h getline.h stralloc.h \
gen_alloc.h
	./compile getline.c

getline2.o: \
compile getline2.c substdio.h byte.h alloc.h getline.h stralloc.h \
gen_alloc.h
	./compile getline2.c

gfrom.o: \
compile gfrom.c str.h gfrom.h
	./compile gfrom.c

hasmkffo.h: \
trymkffo.c compile load
	( ( ./compile trymkffo.c && ./load trymkffo ) >/dev/null 2>&1 \
	&& echo \#define HASMKFIFO 1 || exit 0 ) > hasmkffo.h
	rm -f trymkffo.o trymkffo

hassgact.h: \
trysgact.c compile load
	( ( ./compile trysgact.c && ./load trysgact ) >/dev/null 2>&1 \
	&& echo \#define HASSIGACTION 1 || exit 0 ) > hassgact.h
	rm -f trysgact.o trysgact

hassgprm.h: \
trysgprm.c compile load
	( ( ./compile trysgprm.c && ./load trysgprm ) >/dev/null 2>&1 \
	&& echo \#define HASSIGPROCMASK 1 || exit 0 ) > hassgprm.h
	rm -f trysgprm.o trysgprm

haswaitp.h: \
trywaitp.c compile load
	( ( ./compile trywaitp.c && ./load trywaitp ) >/dev/null 2>&1 \
	&& echo \#define HASWAITPID 1 || exit 0 ) > haswaitp.h
	rm -f trywaitp.o trywaitp

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
compile hostname.c subfd.h readwrite.h exit.h substdio.h
	./compile hostname.c

ip.o: \
compile ip.c ip.h scan.h fmt.h
	./compile ip.c

ipalloc.o: \
compile ipalloc.c ipalloc.h ip.h alloc.h gen_alloc.h gen_allocdefs.h
	./compile ipalloc.c

ipme.o: \
compile ipme.c gen_alloc.h ipme.h ipalloc.h ip.h byte.h \
auto-hassalen.h stralloc.h
	./compile ipme.c

ipmeprint: \
ipmeprint.o ipme.o ip.o ipalloc.o libsubstdio.a libstralloc.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load ipmeprint ipme.o ip.o ipalloc.o libsubstdio.a \
	libstralloc.a liballoc.a liberror.a libstr.a libfs.a

ipmeprint.o: \
compile ipmeprint.c ipalloc.h ipme.h substdio.h ip.h subfd.h \
gen_alloc.h exit.h
	./compile ipmeprint.c

liballoc.a: \
alloc.o alloc_re.o makelib
	./makelib liballoc.a alloc.o alloc_re.o

libcase.a: \
makelib case_diffb.o case_diffs.o case_lowerb.o case_lowers.o
	./makelib libcase.a \
	case_diffb.o case_diffs.o case_lowerb.o case_lowers.o

libcdb.a: \
makelib cdb_hash.o cdb_seek.o cdb_unpack.o
	./makelib libcdb.a cdb_hash.o cdb_seek.o cdb_unpack.o

libcdbmake.a: \
makelib cdbmake_add.o cdbmake_hash.o cdbmake_pack.o
	./makelib libcdbmake.a cdbmake_add.o cdbmake_hash.o cdbmake_pack.o

libenv.a: \
env.o envread.o makelib
	./makelib libenv.a env.o envread.o

liberror.a: \
error.o error_temp.o error_str.o makelib
	./makelib liberror.a error.o error_temp.o error_str.o

libfd.a: \
makelib fd_copy.o fd_move.o
	./makelib libfd.a fd_copy.o fd_move.o

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

libopen.a: \
makelib open_append.o open_excl.o open_read.o open_trunc.o \
open_write.o
	./makelib libopen.a \
	open_read.o open_write.o open_excl.o open_trunc.o open_append.o

libseek.a: \
makelib seek_cur.o seek_end.o seek_set.o seek_trunc.o
	./makelib libseek.a \
	seek_cur.o seek_end.o seek_set.o seek_trunc.o

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
makelib substdio.o substdi.o substdo.o subfderr.o subfdout.o \
subfdouts.o subfdin.o subfdins.o substdio_copy.o
	./makelib libsubstdio.a substdio.o substdi.o substdo.o \
	subfderr.o subfdout.o subfdin.o subfdouts.o subfdins.o \
	substdio_copy.o

libwait.a: \
makelib wait_nohang.o wait_pid.o
	./makelib libwait.a \
	wait_pid.o wait_nohang.o

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
compile maildir.c stralloc.h gen_alloc.h prioq.h now.h strerr.h \
maildir.h direntry.h env.h str.h datetime.h
	./compile maildir.c

maildir2mbox: \
maildir2mbox.o maildir.o prioq.o now.o libopen.a lock.o myctime.o \
datetime.o gfrom.o libstrerr.a libgetline.a libstralloc.a libenv.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load maildir2mbox prioq.o now.o lock.o myctime.o \
	maildir.o \
	datetime.o gfrom.o \
	libopen.a \
	libstrerr.a libgetline.a libstralloc.a libenv.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

maildir2mbox.0: \
maildir2mbox.1
	$(NROFF) maildir2mbox.1 > maildir2mbox.0

maildir2mbox.o: \
compile maildir2mbox.c getline.h lock.h readwrite.h stralloc.h env.h \
gen_alloc.h prioq.h gfrom.h exit.h open.h strerr.h maildir.h error.h \
substdio.h subfd.h str.h myctime.h datetime.h
	./compile maildir2mbox.c

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
maildirwatch.o maildir.o prioq.o now.o libopen.a hfield.o headerbody.o \
libstrerr.a libgetline.a libstralloc.a libenv.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a load
	./load maildirwatch \
	prioq.o now.o lock.o hfield.o headerbody.o \
	maildir.o \
	libopen.a \
	libstrerr.a libgetline.a libstralloc.a libenv.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

maildirwatch.0: \
maildirwatch.1
	$(NROFF) maildirwatch.1 > maildirwatch.0

maildirwatch.o: \
compile maildirwatch.c hfield.h stralloc.h gen_alloc.h prioq.h exit.h \
headerbody.h open.h strerr.h substdio.h maildir.h subfd.h getline.h \
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
dot-qmail.0 maildir.0 maildirmake.0 maildir2mbox.0 maildirwatch.0 \
mailsubj.0 mbox.0 preline.0 qbiff.0 qlist.0 qmail.0 qreceipt.0 \
qmail-popup.0 qmail-pop3d.0 qmail-alias.0 qmail-clean.0 \
qmail-command.0 qmail-control.0 qmail-header.0 qmail-inject.0 \
qmail-limits.0 qmail-log.0 qmail-lspawn.0 qmail-newu.0 qmail-pw2u.0 \
qmail-users.0 qmail-getpw.0 qmail-qread.0 qmail-qstat.0 qmail-tcpto.0 \
qmail-queue.0 qmail-remote.0 qmail-rspawn.0 qmail-send.0 qmail-qmtpd.0 \
qmail-smtpd.0 qmail-start.0 qmail-showctl.0 qmail-upgrade.0 tcp-env.0 \
tcp-environ.0 splogger.0

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

new.hdgigbhc:

newfield.o: \
compile newfield.c gen_alloc.h newfield.h fmt.h date822fmt.h \
datetime.h stralloc.h
	./compile newfield.c

now.o: \
compile now.c datetime.h now.h
	./compile now.c

open_append.o: \
compile open_append.c open.h
	./compile open_append.c

open_excl.o: \
compile open_excl.c open.h
	./compile open_excl.c

open_read.o: \
compile open_read.c open.h
	./compile open_read.c

open_trunc.o: \
compile open_trunc.c open.h
	./compile open_trunc.c

open_write.o: \
compile open_write.c open.h
	./compile open_write.c

pinq: \
pinq.sh qmail-home warn-auto.sh
	cat warn-auto.sh pinq.sh \
	| sed s}HOME}"`./qmail-home`"}g > pinq
	chmod 755 pinq

predate: \
load predate.o datetime_un.o signal.o libsubstdio.a libwait.a libfd.a \
liberror.a libstr.a libfs.a
	./load predate datetime_un.o signal.o \
	libwait.a libfd.a \
	libsubstdio.a liberror.a libstr.a libfs.a

predate.o: \
compile predate.c exit.h fmt.h substdio.h subfd.h wait.h fork.h \
readwrite.h datetime.h fd.h
	./compile predate.c

preline: \
load preline.o libwait.a signal.o libenv.a libgetopt.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a libfd.a
	./load preline signal.o \
	libfd.a \
	libwait.a \
	libenv.a libgetopt.a libsubstdio.a \
	liballoc.a liberror.a libstr.a libfs.a

preline.0: \
preline.1
	$(NROFF) preline.1 > preline.0

preline.o: \
compile preline.c fd.h exit.h error.h substdio.h wait.h subfd.h \
subgetopt.h env.h fork.h readwrite.h sgetopt.h signal.h
	./compile preline.c

printbreak: \
printbreak.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printbreak libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printbreak.o: \
compile printbreak.c conf-home.h substdio.h subfd.h readwrite.h \
conf-unusual.h exit.h
	./compile printbreak.c

printnumd: \
printnumd.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printnumd libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printnumd.o: \
compile printnumd.c conf-home.h fmt.h substdio.h subfd.h readwrite.h \
conf-unusual.h exit.h
	./compile printnumd.c

printsplit: \
printsplit.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load printsplit libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

printsplit.o: \
compile printsplit.c conf-home.h fmt.h substdio.h subfd.h readwrite.h \
conf-unusual.h exit.h
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
qbiff.o libopen.a headerbody.o hfield.o libgetline.a libenv.a \
libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
load
	./load qbiff headerbody.o hfield.o libgetline.a \
	libopen.a \
	libenv.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
	libstr.a libfs.a

qbiff.0: \
qbiff.1
	$(NROFF) qbiff.1 > qbiff.0

qbiff.o: \
compile qbiff.c stralloc.h gen_alloc.h exit.h headerbody.h open.h \
substdio.h subfd.h byte.h readwrite.h env.h str.h hfield.h
	./compile qbiff.c

qlist: \
qlist.o signal.o hfield.o token822.o headerbody.o libopen.a lock.o \
libwait.a qqtalk.o libcase.a libenv.a libgetline.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a libfd.a load
	./load qlist signal.o hfield.o token822.o headerbody.o \
	lock.o qqtalk.o libenv.a libgetline.a libstralloc.a \
	libfd.a \
	libcase.a \
	libwait.a \
	libopen.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qlist.0: \
qlist.1
	$(NROFF) qlist.1 > qlist.0

qlist.o: \
compile qlist.c token822.h lock.h alloc.h signal.h getline.h \
stralloc.h gen_alloc.h exit.h qqtalk.h headerbody.h readwrite.h \
gen_allocdefs.h env.h case.h open.h error.h substdio.h str.h subfd.h \
hfield.h
	./compile qlist.c

qlist2: \
qlist2.sh qmail-home printbreak warn-auto.sh
	cat warn-auto.sh qlist2.sh \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}HOME}"`./qmail-home`"}g > qlist2
	chmod 755 qlist2

qmail-alias: \
qmail-alias.o signal.o now.o libseek.a libopen.a lock.o qqtalk.o \
libwait.a myctime.o datetime.o quote.o gfrom.o slurpclose.o libcase.a \
libenv.a libgetopt.a libgetline.a libsubstdio.a libstralloc.a \
liballoc.a liberror.a libstr.a libfs.a libfd.a load
	./load qmail-alias signal.o now.o lock.o \
	qqtalk.o myctime.o datetime.o quote.o gfrom.o slurpclose.o \
	libfd.a \
	libseek.a \
	libcase.a \
	libwait.a \
	libopen.a \
	libenv.a libgetopt.a libgetline.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-alias.0: \
qmail-alias.8
	$(NROFF) qmail-alias.8 > qmail-alias.0

qmail-alias.o: \
compile qmail-alias.c myctime.h open.h datetime.h getline.h case.h \
seek.h subgetopt.h substdio.h lock.h readwrite.h signal.h sgetopt.h \
stralloc.h gen_alloc.h error.h now.h fork.h subfd.h gfrom.h env.h \
wait.h conf-unusual.h alloc.h exit.h quote.h qqtalk.h str.h byte.h \
fmt.h slurpclose.h
	./compile qmail-alias.c

qmail-check: \
qmail-check.o libopen.a libsubstdio.a libstralloc.a liballoc.a \
liberror.a libstr.a libfs.a load
	./load qmail-check libopen.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-check.o: \
compile qmail-check.c auto-uids.h conf-home.h fmt.h open.h substdio.h \
error.h subfd.h exit.h conf-unusual.h
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
compile qmail-clean.c getline.h signal.h direntry.h stralloc.h \
readwrite.h gen_alloc.h now.h exit.h scan.h fmt.h conf-home.h fmtqfn.h \
error.h substdio.h subfd.h str.h byte.h datetime.h
	./compile qmail-clean.c

qmail-command.0: \
qmail-command.8
	$(NROFF) qmail-command.8 > qmail-command.0

qmail-control.0: \
qmail-control.5
	$(NROFF) qmail-control.5 > qmail-control.0

qmail-getpw: \
load qmail-getpw.o libcase.a libsubstdio.a liberror.a libstr.a libfs.a
	./load qmail-getpw \
	libcase.a \
	libsubstdio.a liberror.a libstr.a libfs.a

qmail-getpw.0: \
qmail-getpw.8
	$(NROFF) qmail-getpw.8 > qmail-getpw.0

qmail-getpw.8: \
qmail-getpw.9 printbreak printnumd
	cat qmail-getpw.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> qmail-getpw.8

qmail-getpw.o: \
compile qmail-getpw.c exit.h conf-user.h fmt.h case.h qlx.h substdio.h \
error.h subfd.h byte.h readwrite.h str.h conf-unusual.h
	./compile qmail-getpw.c

qmail-header.0: \
qmail-header.5
	$(NROFF) qmail-header.5 > qmail-header.0

qmail-home: \
qmail-home.o libsubstdio.a liballoc.a liberror.a libstr.a libfs.a load
	./load qmail-home libsubstdio.a liballoc.a liberror.a libstr.a \
	libfs.a

qmail-home.o: \
compile qmail-home.c substdio.h subfd.h readwrite.h exit.h conf-home.h
	./compile qmail-home.c

qmail-inject: \
qmail-inject.o hfield.o newfield.o datetime.o date822fmt.o now.o \
signal.o quote.o token822.o control.o libopen.a headerbody.o qqtalk.o \
libwait.a libenv.a libgetopt.a libgetline.a libsubstdio.a \
libstralloc.a liballoc.a liberror.a libstr.a libfs.a libfd.a load
	./load qmail-inject hfield.o newfield.o datetime.o \
	date822fmt.o now.o signal.o quote.o token822.o control.o \
	headerbody.o qqtalk.o libenv.a libgetopt.a libgetline.a \
	libfd.a \
	libwait.a \
	libopen.a \
	libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a

qmail-inject.0: \
qmail-inject.8
	$(NROFF) qmail-inject.8 > qmail-inject.0

qmail-inject.o: \
compile qmail-inject.c env.h substdio.h hfield.h control.h signal.h \
error.h sgetopt.h stralloc.h now.h gen_alloc.h exit.h subfd.h \
newfield.h quote.h headerbody.h token822.h qqtalk.h alloc.h \
conf-home.h getline.h fmt.h str.h subgetopt.h gen_allocdefs.h \
datetime.h
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
qmail-lspawn.o coe.o slurpclose.o libopen.a spawn.o libwait.a \
libcase.a prot.o signal.o libgetline.a libstralloc.a libsubstdio.a \
liballoc.a liberror.a libstr.a libfs.a libfd.a libcdb.a load
	./load qmail-lspawn coe.o spawn.o prot.o slurpclose.o \
	signal.o libgetline.a libstralloc.a libsubstdio.a liballoc.a \
	libcdb.a \
	libfd.a \
	libcase.a \
	libwait.a \
	libopen.a \
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
compile qmail-lspawn.c cdb.h stralloc.h fork.h gen_alloc.h fd.h \
uint32.h exit.h scan.h auto-uids.h conf-home.h case.h qlx.h error.h \
substdio.h slurpclose.h wait.h prot.h
	./compile qmail-lspawn.c

qmail-makectl: \
warn-auto.sh qmail-mctl.sh qmail-home
	cat warn-auto.sh qmail-mctl.sh \
	| sed s}HOME}"`./qmail-home`"}g > qmail-makectl
	chmod 755 qmail-makectl

qmail-newu: \
load qmail-newu.o cdbmss.o libcdbmake.a libcase.a libopen.a libseek.a \
libgetline.a libstralloc.a liballoc.a libsubstdio.a liberror.a \
libstr.a libfs.a
	./load qmail-newu \
	cdbmss.o libcdbmake.a \
	libseek.a \
	libopen.a \
	libcase.a \
	libgetline.a libstralloc.a \
	liballoc.a \
	libsubstdio.a liberror.a libstr.a libfs.a

qmail-newu.0: \
qmail-newu.8
	$(NROFF) qmail-newu.8 > qmail-newu.0

qmail-newu.o: \
compile qmail-newu.c exit.h uint32.h conf-home.h case.h open.h error.h \
substdio.h cdbmake.h subfd.h getline.h readwrite.h cdbmss.h stralloc.h \
gen_alloc.h
	./compile qmail-newu.c

qmail-pop3d: \
load qmail-pop3d.o signal.o datetime.o libopen.a prioq.o lock.o now.o \
libgetline.a libsubstdio.a libstralloc.a liballoc.a liberror.a \
libstr.a libfs.a
	./load qmail-pop3d signal.o datetime.o prioq.o \
	lock.o now.o libgetline.a libsubstdio.a libstralloc.a \
	libopen.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-pop3d.0: \
qmail-pop3d.8
	$(NROFF) qmail-pop3d.8 > qmail-pop3d.0

qmail-pop3d.o: \
compile qmail-pop3d.c direntry.h datetime.h getline.h signal.h \
readwrite.h stralloc.h gen_alloc.h prioq.h now.h exit.h scan.h fmt.h \
open.h error.h substdio.h str.h alloc.h prot.h
	./compile qmail-pop3d.c

qmail-popup: \
load qmail-popup.o signal.o datetime.o libwait.a now.o libgetline.a \
libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a libfs.a \
libfd.a
	./load qmail-popup signal.o datetime.o now.o \
	libfd.a \
	libwait.a \
	libgetline.a libsubstdio.a libstralloc.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-popup.0: \
qmail-popup.8
	$(NROFF) qmail-popup.8 > qmail-popup.0

qmail-popup.o: \
compile qmail-popup.c readwrite.h signal.h stralloc.h gen_alloc.h fd.h \
now.h exit.h fmt.h error.h substdio.h wait.h subfd.h alloc.h str.h \
getline.h datetime.h
	./compile qmail-popup.c

qmail-pw2u: \
load qmail-pw2u.o control.o constmap.o libopen.a libcase.a libgetopt.a \
libgetline.a libstralloc.a liballoc.a libsubstdio.a liberror.a \
libstr.a libfs.a
	./load qmail-pw2u \
	control.o constmap.o \
	libopen.a libcase.a \
	libgetline.a libstralloc.a \
	libgetopt.a liballoc.a \
	libsubstdio.a liberror.a libstr.a libfs.a

qmail-pw2u.0: \
qmail-pw2u.8
	$(NROFF) qmail-pw2u.8 > qmail-pw2u.0

qmail-pw2u.8: \
qmail-pw2u.9 printbreak printnumd
	cat qmail-pw2u.9 \
	| sed s}CONF-BREAK}"`./printbreak`"}g \
	| sed s}CONF-NUMD}"`./printnumd`"}g \
	> qmail-pw2u.8

qmail-pw2u.o: \
compile qmail-pw2u.c subgetopt.h gen_alloc.h readwrite.h \
conf-unusual.h conf-user.h scan.h conf-home.h fmt.h open.h constmap.h \
substdio.h error.h control.h subfd.h stralloc.h getline.h sgetopt.h
	./compile qmail-pw2u.c

qmail-qmtpd: \
load qmail-qmtpd.o qqtalk.o now.o libwait.a signal.o datetime.o \
date822fmt.o received.o control.o constmap.o libopen.a libcase.a \
libgetline.a libstralloc.a libsubstdio.a libenv.a liballoc.a \
liberror.a libstr.a libfs.a libfd.a
	./load qmail-qmtpd \
	qqtalk.o now.o signal.o datetime.o date822fmt.o \
	received.o control.o constmap.o \
	libfd.a \
	libcase.a \
	libwait.a \
	libopen.a \
	libgetline.a libstralloc.a libsubstdio.a \
	libenv.a liballoc.a liberror.a libstr.a libfs.a

qmail-qmtpd.0: \
qmail-qmtpd.8
	$(NROFF) qmail-qmtpd.8 > qmail-qmtpd.0

qmail-qmtpd.o: \
compile qmail-qmtpd.c date822fmt.h stralloc.h env.h signal.h \
gen_alloc.h now.h qqtalk.h received.h conf-home.h fmt.h constmap.h \
substdio.h control.h subfd.h str.h readwrite.h datetime.h
	./compile qmail-qmtpd.c

qmail-qread: \
load qmail-qread.o fmtqfn.o readsubdir.o libopen.a datetime.o \
date822fmt.o libgetline.a libsubstdio.a libstralloc.a liballoc.a \
liberror.a libstr.a libfs.a
	./load qmail-qread fmtqfn.o readsubdir.o datetime.o \
	date822fmt.o libgetline.a libsubstdio.a libstralloc.a \
	libopen.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-qread.0: \
qmail-qread.8
	$(NROFF) qmail-qread.8 > qmail-qread.0

qmail-qread.o: \
compile qmail-qread.c readwrite.h stralloc.h date822fmt.h gen_alloc.h \
exit.h conf-home.h fmt.h open.h fmtqfn.h substdio.h error.h \
readsubdir.h subfd.h direntry.h str.h getline.h datetime.h
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
now.o ndelay.o libopen.a libseek.a libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load qmail-queue signal.o triggerpull.o datetime.o fmtqfn.o \
	date822fmt.o now.o ndelay.o libsubstdio.a \
	libseek.a \
	libopen.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-queue.0: \
qmail-queue.8
	$(NROFF) qmail-queue.8 > qmail-queue.0

qmail-queue.o: \
compile qmail-queue.c readwrite.h signal.h date822fmt.h triggerpull.h \
now.h conf-unusual.h exit.h auto-uids.h conf-home.h fmt.h open.h \
fmtqfn.h qqx.h substdio.h datetime.h alloc.h seek.h
	./compile qmail-queue.c

qmail-remote: \
qmail-remote.o signal.o dns.o ipme.o ip.o tcpto.o lock.o libseek.a \
ndelay.o ipalloc.o control.o constmap.o now.o libopen.a libcase.a \
quote.o timeoutread.o timeoutwrite.o timeoutconn.o libgetline.a \
libstralloc.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
loaddns
	./loaddns qmail-remote signal.o control.o dns.o ipme.o ip.o \
	tcpto.o lock.o ndelay.o ipalloc.o now.o \
	quote.o timeoutread.o timeoutwrite.o timeoutconn.o constmap.o \
	libseek.a \
	libcase.a \
	libopen.a \
	libgetline.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
	libstr.a libfs.a

qmail-remote.0: \
qmail-remote.8
	$(NROFF) qmail-remote.8 > qmail-remote.0

qmail-remote.o: \
compile qmail-remote.c quote.h case.h str.h alloc.h ipme.h \
timeoutread.h getline.h ip.h datetime.h signal.h stralloc.h now.h \
gen_alloc.h control.h exit.h error.h scan.h substdio.h conf-home.h \
timeoutwrite.h gen_allocdefs.h tcpto.h ipalloc.h timeoutconn.h subfd.h \
constmap.h dns.h
	./compile qmail-remote.c

qmail-rspawn: \
qmail-rspawn.o coe.o libopen.a spawn.o libwait.a signal.o \
tcpto_clean.o libgetline.a libstralloc.a libsubstdio.a liballoc.a \
liberror.a libstr.a libfs.a libfd.a load
	./load qmail-rspawn spawn.o signal.o tcpto_clean.o coe.o \
	libgetline.a libstralloc.a libsubstdio.a \
	libfd.a \
	libwait.a \
	libopen.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-rspawn.0: \
qmail-rspawn.8
	$(NROFF) qmail-rspawn.8 > qmail-rspawn.0

qmail-rspawn.o: \
compile qmail-rspawn.c error.h substdio.h wait.h tcpto.h fork.h fd.h \
exit.h
	./compile qmail-rspawn.c

qmail-send: \
qmail-send.o trigger.o control.o qsutil.o signal.o constmap.o quote.o \
fmtqfn.o newfield.o now.o datetime.o date822fmt.o libseek.a ndelay.o \
readsubdir.o libopen.a lock.o libcase.a prioq.o qqtalk.o libwait.a \
libgetline.a libstralloc.a libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a libfd.a load
	./load qmail-send trigger.o control.o qsutil.o signal.o \
	constmap.o quote.o fmtqfn.o newfield.o now.o datetime.o \
	date822fmt.o ndelay.o readsubdir.o lock.o \
	prioq.o qqtalk.o libgetline.a libstralloc.a \
	libfd.a \
	libseek.a \
	libcase.a \
	libwait.a \
	libopen.a \
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
compile qmail-send.c control.h conf-home.h ndelay.h readsubdir.h fmt.h \
lock.h select.h qsutil.h seek.h alloc.h signal.h trigger.h stralloc.h \
gen_alloc.h case.h now.h byte.h fmtqfn.h prioq.h quote.h constmap.h \
newfield.h getline.h open.h exit.h direntry.h error.h scan.h str.h \
readwrite.h substdio.h qqtalk.h datetime.h
	./compile qmail-send.c

qmail-setup: \
qmail-setup.o libopen.a fifo.o libsubstdio.a libstralloc.a liballoc.a \
liberror.a libstr.a libfs.a load
	./load qmail-setup fifo.o libsubstdio.a libstralloc.a \
	libopen.a \
	liballoc.a liberror.a libstr.a libfs.a

qmail-setup.o: \
compile qmail-setup.c conf-unusual.h exit.h auto-uids.h conf-home.h \
fmt.h open.h error.h fifo.h substdio.h subfd.h readwrite.h stralloc.h \
gen_alloc.h
	./compile qmail-setup.c

qmail-showctl: \
load qmail-showctl.o control.o libgetline.a libstralloc.a \
libsubstdio.a liballoc.a libopen.a liberror.a libstr.a libfs.a
	./load qmail-showctl control.o \
	libgetline.a libstralloc.a libsubstdio.a liballoc.a \
	libopen.a liberror.a libstr.a libfs.a

qmail-showctl.0: \
qmail-showctl.8
	$(NROFF) qmail-showctl.8 > qmail-showctl.0

qmail-showctl.o: \
compile qmail-showctl.c exit.h conf-home.h fmt.h constmap.h substdio.h \
control.h subfd.h direntry.h stralloc.h gen_alloc.h
	./compile qmail-showctl.c

qmail-smtpd: \
qmail-smtpd.o ipme.o ip.o ipalloc.o control.o received.o datetime.o \
date822fmt.o signal.o now.o libopen.a libcase.a qqtalk.o libwait.a \
constmap.o libgetline.a libstralloc.a libsubstdio.a libenv.a \
liballoc.a liberror.a libstr.a libfs.a libfd.a load
	./load qmail-smtpd ipme.o ip.o ipalloc.o control.o received.o \
	datetime.o date822fmt.o signal.o now.o qqtalk.o \
	constmap.o libgetline.a libstralloc.a libsubstdio.a \
	libfd.a \
	libcase.a \
	libwait.a \
	libopen.a \
	libenv.a liballoc.a liberror.a libstr.a libfs.a

qmail-smtpd.0: \
qmail-smtpd.8
	$(NROFF) qmail-smtpd.8 > qmail-smtpd.0

qmail-smtpd.o: \
compile qmail-smtpd.c ip.h signal.h error.h stralloc.h control.h \
gen_alloc.h now.h exit.h qqtalk.h received.h substdio.h conf-home.h \
alloc.h fmt.h byte.h ipalloc.h getline.h ipme.h readwrite.h constmap.h \
str.h env.h case.h datetime.h
	./compile qmail-smtpd.c

qmail-start: \
qmail-start.o libfd.a prot.o load
	./load qmail-start \
	libfd.a \
	prot.o

qmail-start.0: \
qmail-start.8
	$(NROFF) qmail-start.8 > qmail-start.0

qmail-start.o: \
compile qmail-start.c exit.h auto-uids.h fork.h prot.h fd.h
	./compile qmail-start.c

qmail-tcpto: \
load qmail-tcpto.o now.o lock.o libopen.a ip.o liberror.a \
libsubstdio.a libfs.a libstr.a
	./load qmail-tcpto now.o lock.o ip.o \
	libopen.a \
	liberror.a libsubstdio.a libfs.a libstr.a

qmail-tcpto.0: \
qmail-tcpto.8
	$(NROFF) qmail-tcpto.8 > qmail-tcpto.0

qmail-tcpto.o: \
compile qmail-tcpto.c now.h exit.h fmt.h conf-home.h substdio.h \
error.h ip.h subfd.h datetime.h lock.h
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

qmail-users.0: \
qmail-users.5
	$(NROFF) qmail-users.5 > qmail-users.0

qmail.0: \
qmail.7
	$(NROFF) qmail.7 > qmail.0

qqtalk.o: \
compile qqtalk.c qqtalk.h qqx.h substdio.h wait.h readwrite.h fork.h \
fd.h exit.h
	./compile qqtalk.c

qreceipt: \
qreceipt.o signal.o hfield.o token822.o headerbody.o libopen.a \
libwait.a quote.o qqtalk.o libenv.a libgetline.a libstralloc.a \
libsubstdio.a liballoc.a liberror.a libstr.a libfs.a libfd.a load
	./load qreceipt signal.o hfield.o token822.o headerbody.o \
	quote.o qqtalk.o libenv.a libgetline.a libstralloc.a \
	libfd.a \
	libwait.a \
	libopen.a \
	libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qreceipt.0: \
qreceipt.1
	$(NROFF) qreceipt.1 > qreceipt.0

qreceipt.o: \
compile qreceipt.c quote.h signal.h getline.h stralloc.h env.h \
gen_alloc.h exit.h headerbody.h qqtalk.h gen_allocdefs.h open.h \
error.h substdio.h subfd.h token822.h str.h alloc.h hfield.h
	./compile qreceipt.c

qsmhook: \
qsmhook.o libwait.a libcase.a signal.o libstralloc.a libenv.a \
libgetopt.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a \
libfd.a load
	./load qsmhook signal.o libstralloc.a libenv.a \
	libfd.a \
	libcase.a \
	libwait.a \
	libgetopt.a libsubstdio.a liballoc.a liberror.a libstr.a libfs.a

qsmhook.o: \
compile qsmhook.c fork.h sgetopt.h env.h signal.h readwrite.h \
gen_alloc.h fd.h exit.h case.h substdio.h error.h subfd.h wait.h \
alloc.h byte.h str.h subgetopt.h stralloc.h
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

received.o: \
compile received.c qqtalk.h received.h fmt.h substdio.h date822fmt.h \
datetime.h now.h
	./compile received.c

remoteinfo.o: \
compile remoteinfo.c remoteinfo.h fmt.h substdio.h timeoutwrite.h ip.h \
byte.h timeoutread.h timeoutconn.h
	./compile remoteinfo.c

scan_nbblong.o: \
compile scan_nbblong.c scan.h
	./compile scan_nbblong.c

scan_ulong.o: \
compile scan_ulong.c scan.h
	./compile scan_ulong.c

seek.o: \
compile seek.c seek.h

seek_cur.o: \
compile seek_cur.c seek.h
	./compile seek_cur.c

seek_end.o: \
compile seek_end.c seek.h
	./compile seek_end.c

seek_set.o: \
compile seek_set.c seek.h
	./compile seek_set.c

seek_trunc.o: \
compile seek_trunc.c seek.h
	./compile seek_trunc.c

select.h: \
trysysel.c compile select.h1 select.h2
	( ./compile trysysel.c >/dev/null 2>&1 \
	&& cat select.h2 || cat select.h1 ) > select.h
	rm -f trysysel.o

sendmail: \
sendmail.o libenv.a libgetopt.a libsubstdio.a liballoc.a liberror.a \
libstr.a libfs.a load
	./load sendmail libenv.a libgetopt.a libsubstdio.a liballoc.a \
	liberror.a libstr.a libfs.a

sendmail.o: \
compile sendmail.c exit.h conf-home.h substdio.h subfd.h alloc.h \
subgetopt.h env.h str.h sgetopt.h
	./compile sendmail.c

setup: \
do-setup

sgetopt.o: \
compile sgetopt.c subfd.h subgetopt.h sgetopt.h substdio.h
	./compile sgetopt.c

shar: \
do-shar

signal.o: \
compile signal.c hassgprm.h hassgact.h signal.h
	./compile signal.c

slurpclose.o: \
compile slurpclose.c gen_alloc.h readwrite.h slurpclose.h stralloc.h
	./compile slurpclose.c

spawn.o: \
chkspawn compile spawn.c getline.h stralloc.h gen_alloc.h exit.h coe.h \
conf-unusual.h conf-home.h auto-uids.h open.h substdio.h error.h \
select.h wait.h str.h byte.h signal.h
	./chkspawn
	./compile spawn.c

splogger: \
splogger.o libsubstdio.a liberror.a libstr.a libfs.a load
	./load splogger \
	libsubstdio.a liberror.a libstr.a libfs.a

splogger.0: \
splogger.8
	$(NROFF) splogger.8 > splogger.0

splogger.o: \
compile splogger.c scan.h fmt.h substdio.h error.h subfd.h str.h \
exit.h
	./compile splogger.c

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
compile strerr_sys.c strerr.h error.h
	./compile strerr_sys.c

subfderr.o: \
compile subfderr.c readwrite.h substdio.h subfd.h
	./compile subfderr.c

subfdin.o: \
compile subfdin.c readwrite.h substdio.h subfd.h
	./compile subfdin.c

subfdins.o: \
compile subfdins.c readwrite.h substdio.h subfd.h
	./compile subfdins.c

subfdout.o: \
compile subfdout.c readwrite.h substdio.h subfd.h
	./compile subfdout.c

subfdouts.o: \
compile subfdouts.c readwrite.h substdio.h subfd.h
	./compile subfdouts.c

subgetopt.o: \
compile subgetopt.c subgetopt.h
	./compile subgetopt.c

substdi.o: \
compile substdi.c byte.h substdio.h error.h
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
tcp-env.o dns.o ip.o ipalloc.o remoteinfo.o timeoutconn.o \
timeoutread.o timeoutwrite.o libcase.a signal.o ndelay.o libgetopt.a \
libenv.a libsubstdio.a libstralloc.a liballoc.a liberror.a libstr.a \
libfs.a loaddns
	./loaddns tcp-env dns.o ip.o ipalloc.o remoteinfo.o ndelay.o \
	timeoutconn.o timeoutread.o timeoutwrite.o \
	signal.o libgetopt.a libenv.a libsubstdio.a libcase.a \
	libstralloc.a liballoc.a liberror.a libstr.a libfs.a

tcp-env.0: \
tcp-env.1
	$(NROFF) tcp-env.1 > tcp-env.0

tcp-env.o: \
compile tcp-env.c remoteinfo.h stralloc.h signal.h gen_alloc.h exit.h \
scan.h fmt.h case.h ip.h byte.h subgetopt.h env.h dns.h str.h
	./compile tcp-env.c

tcp-environ.0: \
tcp-environ.5
	$(NROFF) tcp-environ.5 > tcp-environ.0

tcpto.o: \
compile tcpto.c lock.h now.h open.h ip.h tcpto.h byte.h readwrite.h \
datetime.h seek.h
	./compile tcpto.c

tcpto_clean.o: \
compile tcpto_clean.c tcpto.h readwrite.h open.h substdio.h
	./compile tcpto_clean.c

timeoutconn.o: \
compile timeoutconn.c ip.h error.h select.h readwrite.h timeoutconn.h \
ndelay.h
	./compile timeoutconn.c

timeoutread.o: \
compile timeoutread.c select.h readwrite.h timeoutread.h error.h
	./compile timeoutread.c

timeoutwrite.o: \
compile timeoutwrite.c select.h readwrite.h timeoutwrite.h error.h
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
compile tokenize.c error.h substdio.h subfd.h token822.h alloc.h \
getline.h stralloc.h gen_alloc.h
	./compile tokenize.c

trigger.o: \
compile trigger.c open.h select.h auto-hasnpbg1.h trigger.h
	./compile trigger.c

triggerpull.o: \
compile triggerpull.c open.h ndelay.h triggerpull.h
	./compile triggerpull.c

uint32.h: \
tryulong32.c compile load uint32.h1 uint32.h2
	( ( ./compile tryulong32.c && ./load tryulong32 && ./tryulong32 ) >/dev/null 2>&1 \
	&& cat uint32.h2 || cat uint32.h1 ) > uint32.h
	rm -f tryulong32.o tryulong32

wait_nohang.o: \
compile wait_nohang.c haswaitp.h
	./compile wait_nohang.c

wait_pid.o: \
compile wait_pid.c error.h
	./compile wait_pid.c
