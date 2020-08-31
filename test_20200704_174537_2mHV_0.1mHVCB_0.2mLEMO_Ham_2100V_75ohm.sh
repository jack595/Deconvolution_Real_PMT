#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
#pushd //junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data


root -x -q 'divide.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm")'
# root -x -q 'rearrange.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm")'
# root -x -q 'simplef.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm")'
# root -x -q 'average.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm")'
# root -x -q 'script.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm")'
#rm -f test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm_divide.root test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm_rearrange.root test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm_check.root test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm_average.root

#popd
