#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
#pushd //junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data
source /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/juno-dev/bashrc

#root -x -q 'divide.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
# root -x -q 'data_to_root.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
#root -x -q 'rearrange.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
# root -x -q 'simplef.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
#root -x -q 'getCutRange_and_isSPE.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
#root -x -q 'average.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
#root -x -q 'FFT_All_waves.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
root -x -q 'getFilterSetting3_m.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'
root -x -q 'script.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE")'

cd filterWaves_jobs
./gen_filterWaves_jobs.sh test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE
./test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE_run0.sh
cd -

#rm -f test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE_divide.root test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE_rearrange.root test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE_check.root test_20200825_105105HAM_0.05mHVBRASS60_NSPB50_pinghan_50ouxianSPE_average.root

#popd
