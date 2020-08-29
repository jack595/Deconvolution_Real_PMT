#include "TH1D.h"
double get_baseline(const TH1D* waveform)
{
	int n_sumBin=50;
   	double baseline=0;
	for (int j=0;j<n_sumBin;j++){
		baseline+=waveform->GetBinContent(j+1);
	}
    baseline/=(double)n_sumBin;
    return baseline;
}
bool check_whether_real_signal( const TH1D* waveform )
{
    double baseline = get_baseline(waveform);
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