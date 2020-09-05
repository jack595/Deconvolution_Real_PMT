#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
#pushd //junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data
pushd /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/Deconvolution_Real_PMT/code
source /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/juno-dev/bashrc

root -l 'main_PrepareDeconvoluton.C("RUN")'

#root -x -q 'divide.C("DIR/RUN")'
# root -x -q 'data_to_root.C("DIR/RUN")'
#root -x -q 'rearrange.C("DIR/RUN")'
# root -x -q 'simplef.C("DIR/RUN")'
#root -x -q 'getCutRange_and_isSPE.C("DIR/RUN")'
#root -x -q 'average.C("DIR/RUN")'
#root -x -q 'FFT_All_waves.C("DIR/RUN")'
#root -x -q 'getFilterSetting3_m.C("DIR/RUN")'
#root -x -q 'script.C("DIR/RUN")'
#
#cd filterWaves_jobs
#./gen_filterWaves_jobs.sh RUN
#./RUN_run0.sh
#cd -

#rm -f RUN_divide.root RUN_rearrange.root RUN_check.root RUN_average.root

popd
