#!/bin/bash
rm -rf sub.sh
for ((i=0;i<45;i++))
do
  sed -e "s/START/$i/g" \
    -e "s#PWD#${PWD}#g" \
    -e "s/NAME/waves_tran_${i}.root/g" tmp_run.sh > run_${i}.sh
  echo "hep_sub -os CentOS7 run_${i}.sh " >> sub.sh
done
chmod +x *sh