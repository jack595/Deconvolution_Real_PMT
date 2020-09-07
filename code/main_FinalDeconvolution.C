#include "filterWaves.h"
#include "pars_waves.h"
void main_FinalDeconvolution(TString name_File, int start_wavesID10000=0, int end_wavesID10000=7, TString name_saveFile="try.root", bool debug=true)
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
    // getFilterSetting3_m( name_FullPath, pars );

    filterWaves( name_FullPath, pars, start_wavesID10000, end_wavesID10000, name_saveFile, debug);

}