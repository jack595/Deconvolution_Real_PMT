#pushd /junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data
rm -rf sub.sh
echo "pushd /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/Deconvolution_Real_PMT/deconvolution_prepareJobs" >sub.sh
dir=/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data
for filename in `cat elec_SPE.list`;
do
  run=${filename:0:-5}
  echo ${run}, ${dir}
  sed -e "s/RUN/${run}/g" \
     -e "s#DIR#${dir}#g" sample.sh > ./deconvolution_prepareJobs/$run.sh
  echo "hep_sub ${run}.sh -g juno" >>sub.sh
done

chmod +x ./deconvolution_prepareJobs/*.sh

echo "popd" >>sub.sh
chmod 755 ./sub.sh
#popd
