cd HOME
cd queue
for dir in mess info local remote
do
  ( cd $dir; find . -type f -print ) | (
    cd $dir
    while read path
    do
      id=`basename "$path"`
      sub=`expr "$id" % CONF-SPLIT`
      mv "$path" "$sub"/"$id"
    done
  )
done
