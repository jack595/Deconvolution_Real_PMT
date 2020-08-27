#include "TH1D.h"
#include "TString.h"
#include "TF1.h"
void simplef(TString name){
	bool debug=false;
	 TString dir="";
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
	double baseline=0;
	double rms=0;
	double limit=0;
	int Integral=0;
	tr->SetBranchAddress("waves",&waveform);
	for (int i=0;i<entry;i++){
		tr->GetEntry(i);
		baseline=0;
		rms=0;
		limit=0;
		Integral=0;
		for(int j=0;j<100;j++){
			baseline+=(double)waveform->GetBinContent(j+1);		
		}	
		baseline=baseline/100;
		for (int j=0;j<100;j++){
			rms+=(waveform->GetBinContent(j+1)-baseline)*(waveform->GetBinContent(j+1)-baseline);	
		}
		rms=sqrt((double)rms/100);
	//	 cout<<baseline<<","<<rms<<endl;
		limit=baseline+rms*2.5;
		for (int j=0;j<1024;j++){
			if ((double)waveform->GetBinContent(j+1)>limit){
				Integral+=waveform->GetBinContent(j+1)-baseline;
			} 


		}
		chargeHist->Fill(Integral);
		//	cout<<Integral<<","<<endl;
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

	TCanvas* can8=new TCanvas("c9","c9",800,600);
	//chargeHist->Fit("gaus");
	TF1* fun1=new TF1("fun1","gaus",min,min2);
	chargeHist->Fit("fun1","R");
	chargeHist->DrawCopy();
	fun1->DrawCopy("same");
	



	//chargeHist->Fit("gaus","Q");
	//TF1* fun1=chargeHist->GetFunction("gaus");
	//cout<<fun1->GetParameter(1)<<" and "<<fun1->GetParameter(2)<<endl;
	double low=fun1->GetParameter(1)-fun1->GetParameter(2);
	double high=fun1->GetParameter(1)+fun1->GetParameter(2);
	if (low<min) low=min;
	if (high<min || low>min2) cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
	if (high>min2) high=min2;
	cout<<"low="<<low<<" high="<<high<<endl;
	delete fun1;
	int countspe=0;
	for (int i=0;i<entry;i++){
		tr->GetEntry(i);
		baseline=0;
		rms=0;
		limit=0;
		Integral=0;
		for(int j=0;j<100;j++){
			baseline+=(double)waveform->GetBinContent(j+1);
		}
		baseline=baseline/100;
		for (int j=0;j<100;j++){
			rms+=(waveform->GetBinContent(j+1)-baseline)*(waveform->GetBinContent(j+1)-baseline);
		}
		rms=sqrt((double)rms/100);
		//   cout<<baseline<<","<<rms<<endl;
		limit=baseline+rms*2.5;
		for (int j=0;j<1024;j++){
			if ((double)waveform->GetBinContent(j+1)>limit){
				Integral+=waveform->GetBinContent(j+1)-baseline;
			}


		}
		//                                                                                             chargeHist->Fill(Integral);

		if (Integral>low &&Integral<high) {
			isSPE->SetBinContent(i+1,1);
			countspe++;
		}
		//                                                                                                      cout<<Integral<<","<<endl;
	}

	//TH1D* 

	TFile* g=new TFile(name0,"recreate");
	//TFile* g=new TFile("SPEimage.root","recreate");
	g->cd();
	cout<<"entry="<<entry<<" spe="<<countspe<<endl;
	isSPE->Write();
	chargeHist->Write();
	TCanvas* can1=new TCanvas("c2","c2",800,600);
	can1->cd();
	chargeHist->DrawCopy();
	g->Close();
	f->Close();
}
