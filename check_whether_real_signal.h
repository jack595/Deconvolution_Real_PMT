#include "TH1D.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/TH1_tool.hh"
#include "pars_waves.h"
double get_baseline(const TH1D* waveform, int n_sumBin=50)
{
   	double baseline=0;
	for (int j=1;j<n_sumBin+1;j++){
		baseline+=waveform->GetBinContent(j+1);
	}
    baseline/=(double)n_sumBin;
    return baseline;
}

//Attention!! baseline is above the signal.
bool check_whether_real_signal( const TH1D* waveform )
{
	pars_waves pars;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
    double baseline = get_baseline(waveform, n_bin_getBaseline );
	double max = waveform->GetMaximum();
	double min = waveform->GetMinimum();
	if ( abs(min - baseline)*0.5 > abs(max-baseline) )
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

bool find_real_photon_signal( const TH1D* waveform, double ratio_Sig2Noise=7, double start_baseline=1, double end_baseline=30 )
{
	pars_waves pars;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
	double baseline = get_baseline(waveform,n_bin_getBaseline);
	double min =waveform->GetMinimum();
	double binMin= waveform->GetMinimumBin();
	vector<float> v_Min_baseline = Min_subbin((TH1F*) waveform , start_baseline , end_baseline );
	// std::cout<< "BaseLine Min loction and value :  " << v_Min_baseline[0] << "     " << v_Min_baseline[1] <<endl;
	// if ( abs(baseline-min) > abs( baseline-v_Min_baseline[1])*ratio_Sig2Noise ) 
	if ( abs(baseline-min) > 50 ) 
	{
		return true;
	}
	else
	{
		return false;
	}
}