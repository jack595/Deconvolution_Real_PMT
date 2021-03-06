#include "pars_waves.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
void average(TString name, pars_waves pars){
	// pars_waves pars;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
	const int nDimension = pars.nDimension;
	bool useThreshold50 = pars.useThreshold50;
	 TString dir="";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
	TString name1=dir;
	// ostringstream in;
	//  in<<"new"<<number<<"rearrged.root";
	// ostringstream in2;
	//  in2<<"new"<<number<<"check.root";
	//  ostringstream out;
	//  out<<"new"<<number<<"average.root";
	TFile* f=new TFile(pars.name_RootFilePath+dir.Append("_rearrange_BigPeak.root"),"read");
	//TFile* f=new TFile("new2rearrged.root","read");
	TTree* t=(TTree*) f->Get("waves_BigPeak");
	int entries=t->GetEntries();
	int alterentries=entries;
	TH1D* waveform=NULL;
	t->SetBranchAddress("waves_BigPeak",&waveform);
	int sum[nDimension]={0};
	//f->Close();
	TFile* f2=new TFile(pars.name_RootFilePath+name0.Append("_check_useThreshold50.root"),"read");
	int specount=0;
	TH1D* spe=NULL;
	spe=(TH1D*)f2->Get("isSPE");
	for (int i=0;i<entries;i++){
		t->GetEntry(i);
		if (spe->GetBinContent(i+1)==1){
			//cout<<i<<endl;
			specount++;
			for (int j=0;j<nDimension;j++){
			//	cout<<waveform->GetBinContent(j+1)<<endl;
				sum[j]+=waveform->GetBinContent(j+1);
			// cout<< "specount:" <<specount<<endl;	
			}
		//cout<<sum[400]<<endl;
			//if (waveform->GetBinContent(400)==0 &&waveform->GetBinContent(500)==0 &&waveform->GetBinContent(600)==0) alterentries--;
		}
	}
	TH1F* averageHist=new TH1F("averageHist","averageHist",nDimension,0,nDimension);
	for (int k=0;k<nDimension;k++){
		averageHist->SetBinContent(k+1,(double)sum[k]/specount);
	}
	TFile* g=new TFile(pars.name_RootFilePath+name1.Append("_average_BigPeak.root"),"recreate");
	g->cd();
	averageHist->Write();
	TCanvas* can1=new TCanvas("c2","c2",800,600);
	can1->cd();
	averageHist->DrawCopy();
	TString name_option="";
	if ( useThreshold50==true )
	{
		name_option.Append("_useThreshold50");
	}
	system("mkdir -p "+pars.name_PdfDir+"average_waves/");
	averageHist->SetTitle("Average Waves,entries:"+n2str(entries)+",Spe waves:"+n2str(specount));
	averageHist->SetXTitle("Time (ns)");
	averageHist->SetYTitle("ADC ");
	plot_into_pdf((TH1D*)averageHist, pars.name_PdfDir+"average_waves/"+newname+"_average_BigPeak"+name_option+".pdf");
	g->Close();
	f->Close();
	f2->Close();
	cout<<"entries:"<<entries<<" spe:"<<specount<<endl;
}
