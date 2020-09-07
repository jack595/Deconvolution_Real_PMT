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
#include "FFT_averageWave_BigPeak.h"
#include "rearrange_BigPeak.h"
#include "FFT_BigPeak_SPE.h"
#include "average_BigPeak.h"
#include "FFT_Noise.h"
void main_AnalyzeWavesTH2D(TString name_File)
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
    
    
    // divide(name_FullPath, pars);
    // fillRawWavesIntoTH2D(name_File, pars);
    
    // rearrange(name_FullPath);
    // getCutRange_and_isSPE(name_FullPath);
    // average(name_FullPath);
    // FFT_All_waves(name_FullPath);
    // FFT_averageWave_BigPeak(name_FullPath, pars);
    FFT_Noise(name_FullPath, pars);


}