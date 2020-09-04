
double findPeak(TH1D* h_waves, pars_waves pars, double baseline)
{
	//注意这里波形是负的，所以积分是把baseline减去波形的值相加
	bool found_Peak_start = false;
	int bin_PeakStart=50;
	bool found_Peak_trigger=false;
	int bin_PeakTrigger=50;
	bool found_Peak_end=false;
	int bin_PeakEnd=pars.n_bin_getBaseline+100;
	double threshold_trigger=30;
	//找峰的触发位置
	for (int i = pars.n_bin_getBaseline ; i < pars.nDimension; i++)
	{
		if ( found_Peak_trigger==false && baseline-h_waves->GetBinContent(i)>=threshold_trigger && i<=(pars.n_bin_getBaseline+50) )
		{
			found_Peak_trigger=true;
			bin_PeakTrigger=i;
		}
		else if( found_Peak_trigger==false && i==pars.n_bin_getBaseline+50)
		{
			found_Peak_trigger=false;
		}
	}
	if (found_Peak_trigger==true )
	{
		//从找到的触发位置往回找峰的起始点
		for (int i = bin_PeakTrigger; i >= pars.n_bin_getBaseline; i--)
		{
			if( found_Peak_start==false && baseline-h_waves->GetBinContent(i)<=0 )
			{
				found_Peak_start=true;
				bin_PeakStart=i;
			}
			else if ( found_Peak_start==false && i==pars.n_bin_getBaseline )
			{
				bin_PeakStart=pars.n_bin_getBaseline;
				cout<<"Failing to find the start bin of peak, set the start as "<<pars.n_bin_getBaseline <<endl;
			}
			
		}
	

		//找峰的终点
		for (int i = bin_PeakTrigger; i < pars.n_bin_getBaseline+100; i++)
		{
			if (found_Peak_end==false && baseline-h_waves->GetBinContent(i)<=0)	
			{
				found_Peak_end=true;
				bin_PeakEnd=i;
			}
		}
	}
	else
	{
		cout<< "Failing to find trigger peak, so set the range of Integral as "<< pars.n_bin_getBaseline<<" to "<<pars.n_bin_getBaseline+100;
		bin_PeakEnd=pars.n_bin_getBaseline+100;
		bin_PeakTrigger=pars.n_bin_getBaseline;
		
	}
	
	//积分得到单峰电荷
	double Integral=0;
	for (int i = bin_PeakStart; i < bin_PeakEnd; i++)
	{
		Integral+=h_waves->GetBinContent(i);
	}
	cout<< "found start:  "<<found_Peak_start<<"  found end:  "<<found_Peak_end<<endl;
	cout << "Start:  "<<bin_PeakStart<<"  End:  "<<bin_PeakEnd<<endl;
	return Integral;
}