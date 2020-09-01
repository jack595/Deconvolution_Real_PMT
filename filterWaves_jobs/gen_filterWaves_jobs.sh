#!/bin/bash
rm -rf sub.sh
if [ ! -n "$1" ]; then
  echo "FATAL:  One parameter are supposed to be input !!"
  exit 1
fi

i=0

sed -e "s/START/$i/g" \
  -e "s#PWD#${PWD}#g" \
  -e "s/NAME/$1_waves_tran_${i}.root/g" \
  -e "s#TARGET_FILE#$1#g"  tmp_run.sh > $1_run${i}.sh
echo "hep_sub -os CentOS7 $1_run${i}.sh " >> sub.sh

chmod +x *sh
