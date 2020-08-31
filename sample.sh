#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
#pushd //junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data

#  root -x -q 'divide.C("DIR/RUN")'
# root -x -q 'data_to_root.C("DIR/RUN")'
# root -x -q 'rearrange.C("DIR/RUN")'
# root -x -q 'simplef.C("DIR/RUN")'
root -x -q 'getCutRange_and_isSPE.C("DIR/RUN")'
# root -x -q 'average.C("DIR/RUN")'
# root -x -q 'FFT_All_waves.C("DIR/RUN")'
# root -x -q 'getFilterSetting3_m.C("DIR/RUN")'

# root -x -q 'script.C("DIR/RUN")'

#rm -f RUN_divide.root RUN_rearrange.root RUN_check.root RUN_average.root

#popd
