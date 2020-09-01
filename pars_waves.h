struct pars_waves
{
    const static int n_bin_getBaseline=50;
    const static int loc_Bin_align=100;
    const static int nDimension=512;


    //filter Settings
    const static bool  retain_FilterPeakOnBrae_removeWithPol2 =true;
    const static bool  retain_FilterPeak =false;
    const static bool  chopThePeak =false; //打开这个开关前必须先把retain_FilterPeakOnBrae调成false

    
};
