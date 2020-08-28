#!/bin/bash
hostname
#source /cvmfs/juno.ihep.ac.cn/sl6_amd64_gcc494/Pre-Release/J19v1r0-Pre3/setup.sh

#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
source /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/juno-dev/bashrc
#pushd /junofs/users/zhangxt/20inch/rec/deconvolution/runs/20191009sample/figs/filtercompare/scriptv2
pushd PWD
#root -l -b -q 'filterWaves.C(START, START+1, "NAME")'
root -l -b -q '../filterWaves.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200704_174537_2mHV_0.1mHVCB_0.2mLEMO_Ham_2100V_75ohm",START, START+1, "NAME")'
popd
