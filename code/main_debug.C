#include "TString.h"
#include "pars_waves.h"
#include "divide.h"
#include "rearrange.h"
#include "getCutRange_and_isSPE.h"
#include "getCutRange_and_isSPE_BigPeak.h"
#include "average.h"
#include "FFT_All_waves.h"
#include "getFilterSetting3_m.h"
#include "FFT_averageWave.h"
#include "rearrange_BigPeak.h"
#include "FFT_BigPeak_SPE.h"
#include "average_BigPeak.h"
#include "FFT_averageWave_BigPeak.h"
void main_debug(TString name_File)
{
    pars_waves pars;
    pars.name_RawDataDir="/junofs/prototype/Data_prtJUNO/Raw_Data/ihep_test_data/";
    TString name_FullPath = pars.name_RawDataDir+name_File;
    pars.name_WorkDir="/afs/ihep.ac.cn/users/l/luoxj/junofs_500G/Deconvolution_Real_PMT/";
    pars.name_RootFilePath=pars.name_WorkDir+"RootFile/";
    pars.name_PdfDir=pars.name_WorkDir+"output_pdf/";
    if (pars.useThreshold50==true)
    {
        pars.name_RawWaveSelectOption.Append("_useThreshold");
    }
    else
    {
        pars.name_RawWaveSelectOption.Append("_NoUseThreshold");
    }
    

    // 还没有完成对所有波形的变换
    // average( name_FullPath, pars );
    // getCutRange_and_isSPE(name_FullPath, pars );
    
    
    // divide(name_FullPath, pars);
    // rearrange_BigPeak(name_FullPath, pars);
    // rearrange(name_FullPath, pars);
    // getCutRange_and_isSPE_BigPeak(name_FullPath, pars );
    // average_BigPeak( name_FullPath, pars );
    // FFT_All_waves(name_FullPath);
    // FFT_BigPeak_SPE(name_FullPath, pars );
    // FFT_averageWave_BigPeak(name_FullPath, pars);

    getFilterSetting3_m( name_FullPath, pars );

}