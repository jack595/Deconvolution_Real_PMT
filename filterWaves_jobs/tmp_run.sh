#!/bin/bash
hostname
#source /cvmfs/juno.ihep.ac.cn/sl6_amd64_gcc494/Pre-Release/J19v1r0-Pre3/setup.sh

#source /cvmfs/juno.ihep.ac.cn/centos7_amd64_gcc830/Pre-Release/J20v1r0-Pre2/setup.sh
source /afs/ihep.ac.cn/users/l/luoxj/junofs_500G/juno-dev/bashrc
#pushd /junofs/users/zhangxt/20inch/rec/deconvolution/runs/20191009sample/figs/filtercompare/scriptv2
pushd PWD
#root -l -b -q 'filterWaves.C(START, START+1, "NAME")'
root -l -b -q '../filterWaves.C("TARGET_FILE",START, START+1, "NAME")'
popd
