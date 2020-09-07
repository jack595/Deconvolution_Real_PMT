#include "TTree.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include <sstream>
#include <string.h>
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
#include <vector>
#include "pars_waves.h"
void rearrange_BigPeak(TString name, pars_waves pars){
	using namespace std ;
	bool debug=false;
	bool plot_into_Canvas=false;
	bool plot_ResultIntoPdf=true; 
	double n_bin_getBaseline=pars.n_bin_getBaseline;
	double loc_Bin_align=pars.loc_Bin_align;
	// bool debug=false;
	TString dir="";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;

	// if (debug==true)
	// {
	// 	dir.Append("_Debuge");
	// }
	TString name1=dir.Append("_divide.root");
	TString name2=name0.Append("_rearrange_BigPeak.root");
	// if (debug==true)
	// {
	// 	name2="Debug_"+name2;
	// 	cout<< name2 <<endl;
	// }
	TFile* f=new TFile(pars.name_RootFilePath+name1,"read");
	//TFile* f=new TFile("new1divide.root","read");
	TTree* t=(TTree*) f->Get("signalBigPeak");
	// TTree* t=(TTree*) f->Get("waves");
    // TTree* t_BigPeak=(TTree*) f->Get("signalBigPeak");
    // TH1D* h_BigPeak = NULL;
	TH1D* waveform=NULL;
	TH1D* waveform_AlignedBaseline=NULL;
	t->SetBranchAddress("signalBigPeak", &waveform );
	// t->SetBranchAddress("waves", &waveform );
    // t_BigPeak->SetBranchAddress("signalBigPeak", &h_BigPeak );
	int entries=t->GetEntries();
	//int entries=10;
	if (debug==true ) name2="debug_"+name2;
    TFile* g=new TFile(pars.name_RootFilePath+name2,"recreate");
	TTree* str=new TTree("waves_BigPeak","waves_BigPeak");
	TH1D* waveform_Rearrange=NULL;
	str->Branch("waves_BigPeak","TH1D",&waveform_Rearrange);
	//TH1D* waveform_AlignedBaseline=new TH1D("waveform_AlignedBaseline","waveform_AlignedBaseline",nBin,0,nBin);
        //TH1D* waveform_Rearrange=new TH1D("waveform_Rearrange","waveform_Rearrange",nBin,0,nBin);
	t->GetEntry(1);
	const double nBin = waveform->GetXaxis() ->GetNbins();
	cout<< "nBin:    :"<<nBin<<endl;
	cout<< "Entries of BigPeak:  "<< entries<<endl;
    TH1D* startposition= new TH1D("startposition","startposition",nBin,0,nBin);
	int sum=0;
	int maximum=0;
	int maxpoint=0;
	int bin_JustOverThreshold=0;
	if (debug==true) entries=400;
	int n_WavesAborted=0;

	vector<TH1D*> v1D_TH1D;
	
	for (int k=0;k<entries;k++){
		t->GetEntry(k);
		// int n_BinMoveWave=0;
		 waveform_AlignedBaseline=new TH1D(Form("trans%i",k),"waveform_AlignedBaseline",nBin,0,nBin);

		sum=0;
		for (int j=0;j<n_bin_getBaseline;j++){
			sum+=waveform->GetBinContent(j+1);
		}
		sum=sum/n_bin_getBaseline;
		for (int i=0;i<(nBin-1);i++){
			//因为电子学的信号是负的，所以这里的sum代表的就是baseline，而且是用baseline去减waveform
			waveform_AlignedBaseline->SetBinContent(i+1,sum-waveform->GetBinContent(i+1));
		}
		waveform_AlignedBaseline->SetBinContent(nBin,sum-waveform->GetBinContent((nBin-1)));
		

		if (debug == true)
		{
			TCanvas *c2=new TCanvas("c"+(TString)n2str(k),"c"+(TString)n2str(k),800,600);
			waveform_AlignedBaseline->DrawCopy();
		}
		

		//找每个波形的最大值以及最大值对应的bin
		maximum=0;
		maxpoint=0;
		for (int i=0;i<nBin;i++){
			if (waveform_AlignedBaseline->GetBinContent(i+1)>maximum){
				maximum=waveform_AlignedBaseline->GetBinContent(i+1);
				maxpoint=i+1;
			}
		}
		
		maximum=maximum/6;
		//compare这个参数是为了看波形什么时候过阈值，即过阈的bin在哪个位置
		bin_JustOverThreshold=0;
		for (int i=maxpoint;i>0;i--){
			if (waveform_AlignedBaseline->GetBinContent(i+1)<maximum && bin_JustOverThreshold==0){
				bin_JustOverThreshold=i+1;
			}
		}
		//if (compare==0) compare=512;
		//startposition是为了看trigger的位置的分布
		startposition->Fill(bin_JustOverThreshold);
	//	cout<<"sum="<<sum<<" maxpoint="<<maxpoint<<" compare="<<compare<<" maximum="<<maximum<<endl;
		 waveform_Rearrange=new TH1D(Form("arrage%i",k),"waveform_Rearrange",nBin,0,nBin);
		/*for (int s=0;s<nBin;s++) {
                        waveform_Rearrange->SetBinContent(s+1,0);
                }*/
		if (bin_JustOverThreshold<loc_Bin_align && maximum>0){
			for (int j=loc_Bin_align-bin_JustOverThreshold;j<nBin;j++){
				waveform_Rearrange->SetBinContent(j+1,waveform_AlignedBaseline->GetBinContent(bin_JustOverThreshold-loc_Bin_align+j+1));
			}

			for (int j=0;j<loc_Bin_align-bin_JustOverThreshold;j++){
				waveform_Rearrange->SetBinContent(j+1,waveform_AlignedBaseline->GetBinContent(1));
				// n_BinMoveWave++;
			}

		}
		if (bin_JustOverThreshold>=loc_Bin_align && maximum>0){
			for (int j=0;j<nBin-bin_JustOverThreshold+loc_Bin_align;j++){
				waveform_Rearrange->SetBinContent(j+1,waveform_AlignedBaseline->GetBinContent(bin_JustOverThreshold-loc_Bin_align+j+1));
			}
			for (int j=nBin-bin_JustOverThreshold+loc_Bin_align;j<nBin;j++){
				waveform_Rearrange->SetBinContent(j+1,waveform_AlignedBaseline->GetBinContent(nBin));
				// n_BinMoveWave++;
			}

		}

		//如果波形被移动了超过整个entries的一半，那么就不把这个波形保存到rearrange文件中，从而不会用来计算平均波形
		// if ( n_BinMoveWave > pars.nDimension/2 )
		// {
		// 	// cout<< "The Wave has been moved too far so it is aborted."<<endl;
		// 	n_WavesAborted++;
		// 	continue;
		// }
		
		if (debug==true)
		{
			TCanvas *c1=new TCanvas("c_waves"+(TString)n2str(k),"c_waves"+(TString)n2str(k),800,600);
			waveform_Rearrange->DrawCopy();
			// waveform->DrawCopy();
		}
		if ( plot_ResultIntoPdf == true && k<120 )
		{
			v1D_TH1D.push_back( (TH1D*) waveform_Rearrange ->Clone( "waves_rearanged" ));
		}
		

        if (k==0){
			TCanvas* can1=new TCanvas("c2","c2",800,600);
   		        can1->cd();
  			waveform_Rearrange->Draw();
		}
		
		g->cd();
		str->Fill();
		delete waveform_AlignedBaseline;
	    delete waveform_Rearrange;
		if (k%10000==0) cout<<k<<" finished!"<<endl;
	}       
	g->cd();


	if ( plot_ResultIntoPdf == true )
	{
		system("mkdir -p "+pars.name_PdfDir+"rearange_waves/");
		plot_into_pdf(v1D_TH1D, pars.name_PdfDir+"rearange_waves/"+newname+"_rearanged_waves_BigPeak"+pars.name_RawWaveSelectOption+".pdf");
		TCanvas *c1=new TCanvas("c_startposition","c_waves",800,600);
		startposition->DrawCopy();
		c1->SaveAs(pars.name_PdfDir+"rearange_waves/"+newname+"_StartPointsHist_BigPeak"+pars.name_RawWaveSelectOption+".png");
	}
	
	// cout<< "n_WavesOriginal:    "<<entries<<endl;
	// cout<< "n_WavesAborted:   "<<n_WavesAborted<<endl;
	// cout<< "n_Waves_remained:   "<<str->GetEntries()<<endl;
	startposition->Write();
	str->Write();
	g->Close();
	f->Close();
  
}
