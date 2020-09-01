#!/bin/bash
rm -rf sub.sh
if [ ! -n "$1" ]; then
  echo "FATAL:  One parameter are supposed to be input !!"
  exit 1
fi
for ((i=7;i<8;i++))
do
  sed -e "s/START/$i/g" \
    -e "s#PWD#${PWD}#g" \
    -e "s/NAME/waves_tran_${i}.root/g" \
    -e "s#TARGET_FILE#$1#g"  tmp_run.sh > run_${i}.sh
  echo "hep_sub -os CentOS7 run_${i}.sh " >> sub.sh
done
chmod +x *sh
