#pushd /junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data
rm -rf sub.sh
dir=/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data
for filename in `cat elec1.list`;
do
  run=${filename:0:-5}
  echo ${run}, ${dir}
  sed -e "s/RUN/${run}/g" \
     -e "s#DIR#${dir}#g" sample.sh > $run.sh
  echo "hep_sub ${run}.sh -g juno" >>sub.sh
done

chmod +x *.sh
#popd
