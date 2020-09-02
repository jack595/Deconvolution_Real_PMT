#pragma once
#include "TString.h"
struct pars_waves
{
    const static int n_bin_getBaseline=50;
    const static int loc_Bin_align=50;
    const static int nDimension=512;

    //divide.C Setting
    const static bool useThreshold50 = true;

    //getCutRange.C Setting
    const static int sigma_selectSinglePhoton = 5;//注意这里因为没办法初始化float的const static，所以将0.5换成5，从而传入其中，一定要注意这是乘以10后的数

    //filter Settings
    const static bool  retain_FilterPeakOnBrae_removeWithPol2 =false;
    const static bool  retain_FilterPeak =false;
    const static bool  chopThePeak =true; //打开这个开关前必须先把retain_FilterPeakOnBrae调成false

};