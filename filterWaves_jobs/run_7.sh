#!/bin/bash
hostname
#source /cvmfs/juno.ihep.ac.cn/sl6_amd64_gcc494/Pre-Release/J19v1r0-Pre3/setup.sh

#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
source /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/juno-dev/bashrc
#pushd /junofs/users/zhangxt/20inch/rec/deconvolution/runs/20191009sample/figs/filtercompare/scriptv2
pushd /afs/ihep.ac.cn/users/l/luoxj/Deconvolution_Real_PMT/filterWaves_jobs
#root -l -b -q 'filterWaves.C(7, 7+1, "waves_tran_7.root")'
root -l  '../filterWaves.C("/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/test_20200825_120223HAM_0.05mHVBRASS60_NSPB50_pinghan_60ouxianSPE",7, 7+1, "waves_tran_7.root")'
popd
