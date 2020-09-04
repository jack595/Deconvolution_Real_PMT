#include "TH1D.h"
#include "TString.h"
#include "TF1.h"
#include "pars_waves.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/TH1_tool.hh"

double findPeakAndIntegral_waves(TH1D* h_waves, pars_waves pars, double baseline)
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
double IntegralRange(TH1D* h_wave, const int bin_start, const int bin_end, double baseline)
{
	double Integral=0;
	for (int i = bin_start; i < bin_end; i++)
	{
		Integral+=baseline-h_wave->GetBinContent(i);
	}
	return Integral;
	
}
void simplef(TString name, const double times_rms=2.5){
	bool debug=false;
	bool plot_waves_divide=false;
	bool plot_check_IsSPE_to_pdf=true;
	pars_waves pars;
	bool useThereshold50 = pars.useThreshold50;
	TString name_option="";
	bool single_peak_fit=true;
	bool Integral_onePeak=true;

	if ( useThereshold50 == true )
	{
		single_peak_fit=true;
	}
	else
	{
		single_peak_fit=false;
	}
	
	TString dir="";

	int n_bin_getBaseline=pars.n_bin_getBaseline;
	const int nDimension = pars.nDimension;
	int tmp_sigma_selectSinglePhoton = pars.sigma_selectSinglePhoton;
	double sigma_selectSinglePhoton = (double)tmp_sigma_selectSinglePhoton/10.;
	// ostringstream in;
	// in<<"new"<<number<<"divide.root";
	//  ostringstream out;
	//  out<<"new"<<number<<"check.root";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
	dir.Append("_divide.root");
	name0.Append("_check.root");
	TFile* f=new TFile(dir,"read");
	

	//TFile* f=new TFile("new1divide.root","read");
	TTree* tr=(TTree*)f->Get("waves"); 
	int entry=tr->GetEntries();
	TH1D* isSPE=new TH1D("isSPE","isSPE",entry,0,entry);
	TH1D* waveform=NULL;
	TH1D* chargeHist=new TH1D("chargeHist","chargeHist",200,00,20000);
	// TH1D* chargeHist=new TH1D("chargeHist","chargeHist",200,00,5000);
	double baseline=0;
	double rms=0;
	double limit=0;
	int Integral=0;
	tr->SetBranchAddress("waves",&waveform);

	if ( plot_waves_divide==true )
	{
		for (int k = 0; k < 10; k++)
		{
			tr->GetEntry(k);
			TCanvas *c1=new TCanvas("waves"+(TString)n2str(k),"waves"+(TString)n2str(k),800,600);	
			waveform->DrawCopy();
		}
	}
	if ( debug==true )	entry=10;
	vector<double> v_integral;
	
	for (int i=0;i<entry;i++){
		tr->GetEntry(i);
		// waveform->Scale(-1.);
		if (debug ==true && i<10)
		{
			TCanvas *c1=new TCanvas("waves"+(TString)n2str(i),"waves"+(TString)n2str(i),800,600);	
			waveform->DrawCopy();
		}
		baseline=0;
		rms=0;
		limit=0;
		Integral=0;
		for(int j=0;j<n_bin_getBaseline;j++){
			baseline+=(double)waveform->GetBinContent(j+1);		
		}	
		baseline=baseline/n_bin_getBaseline;

		if(Integral_onePeak==false)
		{
			for (int j=0;j<n_bin_getBaseline;j++){
				rms+=(waveform->GetBinContent(j+1)-baseline)*(waveform->GetBinContent(j+1)-baseline);	
			}
			rms=sqrt((double)rms/n_bin_getBaseline);
		//	 cout<<baseline<<","<<rms<<endl;
			// cout<< "Max:  " <<waveform->GetMaximum() << " Min: " <<waveform->GetMinimum()<<endl;	
			// if ( abs(waveform->GetMaximum()-baseline) >50 ) continue; 
			limit=baseline+rms*times_rms;
			// limit=baseline+50;

			//注意！！！这里的waveform是电子学的读出没有转化，所以得到波形的峰是向下的，因此这里一定一定要注意，不是用波形减去基线，而是基线减去波形
			for (int j=0;j<nDimension;j++)
			{
				if (2*baseline-(double)waveform->GetBinContent(j+1)>limit){
					Integral+=baseline-waveform->GetBinContent(j+1);
				} 

			}
			// cout<< i << "    baseline:   "<< baseline <<endl;
			//	cout<<Integral<<","<<endl;
			
		}
		else
		{
			Integral=IntegralRange(waveform, pars.n_bin_getBaseline-10, pars.n_bin_getBaseline+50,baseline);
			// cout<<i<<"  Integral=   "<< Integral<<endl;
		}
			chargeHist->Fill(Integral);
			// cout<<Integral<<endl;
			v_integral.push_back(Integral);
	}

		

	if ( debug==true )
	{
		TCanvas *c1=new TCanvas("chargeHist","chargeHist",800,600);
		chargeHist->DrawCopy();
	}
	

	int min=0;
	int binmin=10000000;
	for (int j=0;j<20;j++){
		if (chargeHist->GetBinContent(j+1)<binmin) {
			binmin=chargeHist->GetBinContent(j+1);
			min=j+1;
		}
	}
	min=min*100;
	cout<<"min="<<min<<endl;
	int found=0;
	int min2=0;
	int binmin2=10000000;
	for (int j=20;j<200;j++){
		if (found==0 && chargeHist->GetBinContent(j+1)<binmin) {
			binmin2=chargeHist->GetBinContent(j+1);
			min2=j+1;
			found=1;
		}
	}
	min2=min2*100;
	cout<<"min2="<<min2<<endl;


	TCanvas* can8=new TCanvas("SPE"+(TString) n2str(times_rms),"SPE"+(TString) n2str(times_rms),800,600);
	//chargeHist->Fit("gaus");
	TF1* fun1=NULL;

	double low=0;
	double high=0;
	if ( single_peak_fit == false )
	{
		fun1=new TF1("fun1","gaus",min,min2);
		// fun1=new TF1("fun1","gaus",500,2000);
		chargeHist->Fit("fun1","R");
		low=fun1->GetParameter(1)- sigma_selectSinglePhoton*fun1->GetParameter(2);
		high=fun1->GetParameter(1)+ sigma_selectSinglePhoton* fun1->GetParameter(2);
	}
	else
	{
		cout<< "binCenter:   " << chargeHist->GetBinCenter(3)<<"   "<<chargeHist->GetBinCenter(200)<<endl;
		// TH1D* chargeHist_tmp=new TH1D("h_tmp","h_tmp",200,0,20000);
		// for(int j=4;j<10;j++)
		// {
		// 	chargeHist_tmp->SetBinContent(j,chargeHist_tmp->GetBinContent(j));
		// }
		// TCanvas *c_tmp=new TCanvas("waves_tmp","charge_tmp",800,600);
		// chargeHist_tmp->DrawCopy();
		chargeHist->SetBinContent(0,0);
		chargeHist->SetBinContent(1,0);
		chargeHist->SetBinContent(2,0);
		double max_h = chargeHist->GetBinCenter(chargeHist->GetMaximumBin());
		// double max_h = Max_subbin(chargeHist, 2, 50)[0];
		cout<<"max loc:  "<<max_h<< "  value:   "<<chargeHist->GetBinContent(chargeHist->GetMaximumBin())<<endl;
		fun1=new TF1("fun1","gaus",max_h-500, max_h+500 );
		chargeHist->Fit("fun1","R");
		low=fun1->GetParameter(1)-sigma_selectSinglePhoton * fun1->GetParameter(2);
		high=fun1->GetParameter(1)+sigma_selectSinglePhoton *fun1->GetParameter(2);
		cout<< "high=  "<<high<<endl;
	}
	
	chargeHist->DrawCopy();
	fun1->DrawCopy("same");

	//chargeHist->Fit("gaus","Q");
	//TF1* fun1=chargeHist->GetFunction("gaus");
	//cout<<fun1->GetParameter(1)<<" and "<<fun1->GetParameter(2)<<endl;
	if ( single_peak_fit == false )
	{
		if (low<min) low=min;
		if (high<min || low>min2) cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
		if (high>min2) high=min2;
	}
	cout<<"low="<<low<<" high="<<high<<endl;
	delete fun1;
	int countspe=0;
	for (int i=0;i<entry;i++){
		
		Integral = v_integral[i];
		// cout<< "Integral: "<<Integral<<endl;
		// cout<< "high and low:  "<<high<<"   "<<low<<endl;
		if (Integral>low &&Integral<high) {
			isSPE->SetBinContent(i+1,1);
			countspe++;
		}
		//                                                                                                      cout<<Integral<<","<<endl;
	}
	if (plot_check_IsSPE_to_pdf == true)
	{
		const int n_line_to_pdf = 10;
		vector<vector<TH1D*>> v2D_signal_TH1D_toPDF( n_line_to_pdf );
		int n_to_pdf_waves=0;
		for (int i = 0; i < entry ; i++)
		{	
			tr->GetEntry(i);	
			//注意！！！使用isSPE时，记住它是从1开始的，所以时刻记住要用（i+1）去提取数据
			if( isSPE->GetBinContent(i+1) ==1 )
			{
				// cout<<i<<endl;
			v2D_signal_TH1D_toPDF[n_to_pdf_waves/4].push_back(
				(TH1D*) waveform->Clone( (TString)n2str(i)+"h_waveform_isSPE="+(TString)n2str( isSPE->GetBinContent(i+1) ) ));
			n_to_pdf_waves++;
			if(n_to_pdf_waves==n_line_to_pdf*4)break;
			}
		}

		if ( useThereshold50 == true )
		{
			name_option.Append("_useThreshold50");
		}
		else
		{
			name_option.Append("NoUseThreshold50");
		}
		
		name_option.Append("_"+(TString)n2str(sigma_selectSinglePhoton)+"SelectSigma");
		
		plot_into_pdf(v2D_signal_TH1D_toPDF, "./output_pdf/"+newname+"/"+newname+"_checkSinglePhotonWaves"+name_option+".pdf");
		for (int z = 0; z < v2D_signal_TH1D_toPDF.size(); z++)
		{
			//释放内存
			vector<TH1D*>().swap(v2D_signal_TH1D_toPDF[z]);
		}
		
	}
	
	TFile* g=new TFile(name0,"recreate");
	//TFile* g=new TFile("SPEimage.root","recreate");
	g->cd();
	cout<<"entry="<<entry<<" spe="<<countspe<<endl;
	isSPE->Write();
	chargeHist->Write();
	chargeHist->SetTitle("ChargeHist_SelectRange:"+n2str(low)+"---"+n2str(high));
	plot_into_pdf( chargeHist ,"./output_pdf/"+newname+"/"+newname+"_ChargeHist"+name_option+".pdf");
	// TCanvas* can1=new TCanvas("c2","c2",800,600);
	// can1->cd();
	// chargeHist->DrawCopy();

	g->Close();
	f->Close();
}
void getCutRange_and_isSPE(TString name)
{
	// for (double times_rms=1; times_rms<2.5; times_rms+=0.5)
	// for (double times_rms=3.5; times_rms>2.5; times_rms-=0.5)
	double times_rms = 2.5;
	{
		simplef( name , times_rms );
	}
}
