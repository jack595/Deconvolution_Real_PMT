#!/bin/bash
cd /junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/
echo -e  `ls test_20200825*SPE.root`|tr ' ' '\n'>/afs/ihep.ac.cn/users/l/luoxj/Deconvolution_Real_PMT/elec_SPE.list
cd -

