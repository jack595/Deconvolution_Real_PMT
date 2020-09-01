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
void rearrange(TString name){
	using namespace std ;
	bool debug=false;
	bool plot_into_Canvas=false;
	bool plot_ResultIntoPdf=true; 
	pars_waves pars;
	double n_bin_getBaseline=pars.n_bin_getBaseline;
	double loc_Bin_align=pars.loc_Bin_align;
	// bool debug=false;
	TString dir="";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
	// ostringstream in;
	//  in<<"new"<<number<<"divide.root";
	//  ostringstream out;
	//  out<<"new"<<number<<"rearrged.root";
	if (debug==true)
	{
		dir.Append("_Debuge");
	}
	TString name1=dir.Append("_divide.root");
	TString name2=name0.Append("_rearrange.root");
	if (debug==true)
	{
		name2="Debug_"+name2;
		cout<< name2 <<endl;
	}
	TFile* f=new TFile(name1,"read");
	//TFile* f=new TFile("new1divide.root","read");
	TTree* t=(TTree*) f->Get("waves");
	TH1D* waveform=NULL;
	TH1D* averageHistotrans=NULL;
	t->SetBranchAddress("waves",&waveform);
	int entries=t->GetEntries();
	//int entries=10;
	if (debug==true ) name2="debug_"+name2;
    TFile* g=new TFile(name2,"recreate");
	TTree* str=new TTree("waves","waves");
	TH1D* averageHistorearrage=NULL;
	str->Branch("waves","TH1D",&averageHistorearrage);
	//TH1D* averageHistotrans=new TH1D("averageHistotrans","averageHistotrans",nBin,0,nBin);
        //TH1D* averageHistorearrage=new TH1D("averageHistorearrage","averageHistorearrage",nBin,0,nBin);
	t->GetEntry(1);
	const double nBin = waveform->GetXaxis() ->GetNbins();
	cout<< "nBin:    :"<<nBin<<endl;
    TH1D* startposition= new TH1D("startposition","startposition",nBin,0,nBin);
	int sum=0;
	int maximum=0;
	int maxpoint=0;
	int compare=0;
	if (debug==true) entries=400;

	vector<TH1D*> v1D_TH1D;
	
	for (int k=0;k<entries;k++){
		t->GetEntry(k);

		 averageHistotrans=new TH1D(Form("trans%i",k),"averageHistotrans",nBin,0,nBin);

		sum=0;
		for (int j=0;j<n_bin_getBaseline;j++){
			sum+=waveform->GetBinContent(j+1);
		}
		sum=sum/n_bin_getBaseline;
		for (int i=0;i<(nBin-1);i++){
			//因为电子学的信号是负的，所以这里的sum代表的就是baseline，而且是用baseline去减waveform
			averageHistotrans->SetBinContent(i+1,sum-waveform->GetBinContent(i+1));
		}
		averageHistotrans->SetBinContent(nBin,sum-waveform->GetBinContent((nBin-1)));
		

		if (debug == true)
		{
			TCanvas *c2=new TCanvas("c"+(TString)n2str(k),"c"+(TString)n2str(k),800,600);
			averageHistotrans->DrawCopy();
		}
		

		//找每个波形的最大值以及最大值对应的bin
		maximum=0;
		maxpoint=0;
		for (int i=0;i<nBin;i++){
			if (averageHistotrans->GetBinContent(i+1)>maximum){
				maximum=averageHistotrans->GetBinContent(i+1);
				maxpoint=i+1;
			}
		}
		
		maximum=maximum/6;
		//compare这个参数是为了看波形什么时候过阈值，即过阈的bin在哪个位置
		compare=0;
		for (int i=maxpoint;i>0;i--){
			if (averageHistotrans->GetBinContent(i+1)<maximum && compare==0){
				compare=i+1;
			}
		}
		//if (compare==0) compare=512;
		//startposition是为了看trigger的位置的分布
		startposition->Fill(compare);
	//	cout<<"sum="<<sum<<" maxpoint="<<maxpoint<<" compare="<<compare<<" maximum="<<maximum<<endl;
		 averageHistorearrage=new TH1D(Form("arrage%i",k),"averageHistorearrage",nBin,0,nBin);
		/*for (int s=0;s<nBin;s++) {
                        averageHistorearrage->SetBinContent(s+1,0);
                }*/
		if (compare<loc_Bin_align && maximum>0){
			for (int j=loc_Bin_align-compare;j<nBin;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(compare-loc_Bin_align+j+1));
			}
			for (int j=0;j<loc_Bin_align-compare;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(1));
			}

		}
		if (compare>=loc_Bin_align && maximum>0){
			for (int j=0;j<nBin-compare+loc_Bin_align;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(compare-loc_Bin_align+j+1));
			}
			for (int j=nBin-compare+loc_Bin_align;j<nBin;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(nBin));
			}

		}
		if (debug==true)
		{
			TCanvas *c1=new TCanvas("c_waves"+(TString)n2str(k),"c_waves"+(TString)n2str(k),800,600);
			averageHistorearrage->DrawCopy();
			// waveform->DrawCopy();
		}
		if ( plot_ResultIntoPdf == true && k<120 )
		{
			v1D_TH1D.push_back( (TH1D*) averageHistorearrage ->Clone( "waves_rearanged" ));
		}
		

        if (k==0){
			TCanvas* can1=new TCanvas("c2","c2",800,600);
   		        can1->cd();
  			averageHistorearrage->Draw();
		}
		
		g->cd();
		str->Fill();
		delete averageHistotrans;
	    delete averageHistorearrage;
		if (k%10000==0) cout<<k<<" finished!"<<endl;
	}       
	g->cd();
	if (debug==true)
		{
			TCanvas *c1=new TCanvas("c_startposition","c_waves",800,600);
			startposition->DrawCopy();
		}
	if ( plot_ResultIntoPdf == true )
	{
		plot_into_pdf(v1D_TH1D, "./output_pdf/"+newname+"rearanged_waves.pdf");
	}
	
	startposition->Write();
	str->Write();
	g->Close();
	f->Close();
	//str->GetEntry(1);
	/*TH1D* test=NULL;
	str->SetBranchAddress("waves",test);
        str->GetEntry(1);
	TCanvas* can=new TCanvas("c1","c1",800,600);
	  can->cd();
	  test->Draw();
*/	  
}
