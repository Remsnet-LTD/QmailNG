echo The qmail control files are stored in HOME/control.

cd HOME
cd control

for i in *
do
  echo ''
  echo "control/$i:"

  case $i in
  bouncefrom)
    contents=`head -1 $i`
    echo "Bounce user name is $contents."
    ;;
  bouncehost)
    contents=`head -1 $i`
    echo "Bounce host name is $contents."
    ;;
  concurrencylocal)
    contents=`head -1 $i`
    echo "Local concurrency is $contents."
    ;;
  concurrencyremote)
    contents=`head -1 $i`
    echo "Remote concurrency is $contents."
    ;;
  defaultdomain)
    contents=`head -1 $i`
    echo "Default domain name is $contents."
    echo "So joe@host will be interpreted as joe@host.$contents."
    ;;
  defaulthost)
    contents=`head -1 $i`
    echo "Default host name is $contents."
    echo "So joe will be interpreted as joe@$contents."
    ;;
  doublebouncehost)
    contents=`head -1 $i`
    echo "Double bounce recipient host name is $contents."
    ;;
  doublebounceto)
    contents=`head -1 $i`
    echo "Double bounce recipient user name is $contents."
    ;;
  envnoathost)
    contents=`head -1 $i`
    echo "Presumed domain name is $contents."
    echo "This is the default host name for .qmail files."
    ;;
  helohost)
    contents=`head -1 $i`
    echo "SMTP client HELO host name is $contents."
    ;;
  idhost)
    contents=`head -1 $i`
    echo "Message-ID host name is $contents."
    ;;
  localiphost)
    contents=`head -1 $i`
    echo "Local IP address replacement host name is $contents."
    ;;
  locals)
    while read domain
    do
      echo "Messages for $domain are delivered locally."
    done < $i
    ;;
  me)
    contents=`head -1 $i`
    echo "My name is $contents."
    ;;
  percenthack)
    while read domain
    do
      echo "The percent hack is allowed for user%host@$domain."
    done < $i
    ;;
  plusdomain)
    contents=`head -1 $i`
    echo "Plus domain name is $contents."
    echo "So joe@host+ will be interpreted as joe@host.$contents."
    ;;
  queuelifetime)
    contents=`head -1 $i`
    echo "Message lifetime in the queue is $contents seconds."
    ;;
  rcpthosts)
    while read domain
    do
      echo "SMTP clients may send messages to recipients at $domain."
    done < $i
    ;;
  smtpgreeting)
    contents=`head -1 $i`
    echo "SMTP greeting: 220 $contents"
    ;;
  smtproutes)
    while read line
    do
      echo "SMTP route: $line"
    done < $i
    ;;
  timeoutconnect)
    contents=`head -1 $i`
    echo "SMTP client connection timeout is $contents seconds."
    ;;
  timeoutremote)
    contents=`head -1 $i`
    echo "SMTP client data timeout is $contents seconds."
    ;;
  timeoutsmtpd)
    contents=`head -1 $i`
    echo "SMTP server data timeout is $contents seconds."
    ;;
  usermap)
    while read line
    do
      echo "User map: $line"
    done < $i
    ;;
  virtualdomains)
    while read line
    do
      echo "Virtual domain: $line"
    done < $i
    ;;
  *)
    echo 'I have no idea what this control file does.'
    ;;
  esac

done | cat -v
