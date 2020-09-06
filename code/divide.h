#include "TString.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "pars_waves.h"
#include <vector>
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
#include "check_whether_real_signal.h"

void divide(TString name, pars_waves pars){
	using namespace std;
	// cout<< "struct pars_waves has been input!"<<pars.name_RawWaveSelectOption<<endl;
	bool debug = false;
	bool divide_signal_and_noise_to_pdf=true;
	// bool divide_signal_and_noise_to_pdf=false;
	bool toPDF_totally=true;
	// bool toPDF_totally=false;
	bool check_baseline=false;
	bool useThreshold50=pars.useThreshold50;
	const int n_graph_to_pdf=200;
	int n_noise=0;
	int n_signal=0;
	int n_induced_signal=0;

	TString dir="";
	//int entries=443681;
	int length=name.Length();
	TString newname=name(55,length); 
	TString input_name=newname;
	TString name_option="";
	UShort_t chData[1024];
	Int_t nDimension;
	//  ostringstream in;
	//  in<<"new"<<name<<".root";
	//  ostringstream out;
	//  out<<"new"<<name<<"divide.root";
	dir.Append(newname);
	TFile* f=new TFile(name.Append(".root"),"read");
	TTree* tr=(TTree*) f->Get("fadc_prototype");
	tr->SetBranchAddress("nDimension",&nDimension);
	tr->SetBranchAddress("chData",&chData);
	int entries=tr->GetEntries();
	cout<< "entries:   " << entries<<endl;

	// int namechar=name.First("r");
	// TString newname=name(0,namechar);
	if (debug==true)
	{
		dir.Append("_Debug");
	}
	
	dir.Append("_divide.root");
	
	TFile* g=new TFile(pars.name_RootFilePath+dir,"recreate");
	TTree* str= new TTree("waves","waves_total");
	TTree* tree_signalBigPeak = new TTree("signalBigPeak", "signalBigPeak");
	TTree* tree_noise = new TTree("noise","noise");
	TTree* tree_signalSmallPeak = new TTree("signalSmallPeak", "signalSmallPeak");
	TH1D* h_waveform=NULL;
	TH1D* h_signalBigPeak=NULL;
	TH1D* h_signalSmallPeak=NULL;
	TH1D* h_noise=NULL;
	int pmtID=1;
	str->Branch("waves","TH1D",&h_waveform);
	tree_signalBigPeak->Branch("signalBigPeak","TH1D",&h_signalBigPeak);
	tree_signalSmallPeak->Branch("signalSmallPeak","TH1D",&h_signalSmallPeak);
	tree_noise->Branch("noise","TH1D",&h_noise);
	str->Branch("pmtid",&pmtID);
	if ( debug ==true )
	{
		entries=n_graph_to_pdf;
	}
	vector<vector<TH1D*>> v2D_signal_TH1D_toPDF(entries/4+1);
	vector<vector<TH1D*>> v2D_noise_TH1D_toPDF(entries/4+1);
	vector<vector<TH1D*>> v2D_induced_signal_TH1D_toPDF(entries/4+1);
	vector<vector<TH1D*>> v2D_TH1D_toPDF(entries/4+1);
	
	for(int i=1;i<entries;i++){
	// for(int i=1;i<10000;i++){
		//for(int i=0;i<entries;i++){
		tr->GetEntry(i);
		if(i%10000==0)cout << i <<endl;

		//   cout << nDimension << ", " << chData[0] << endl;
		h_waveform = new TH1D(Form("wf%i", i), "FADC waveform", nDimension, 0, nDimension);
		for(int j=0;j<nDimension;j++){
			h_waveform->SetBinContent(j+1, chData[j]*1.0);
			if ( debug ==true && i==2 )
			{
				h_waveform->DrawCopy();
			}
			
		}
		h_waveform->SetBinContent(0,h_waveform->GetBinContent(2));
		// for (int j = 0; j < nDimension; j++)
		// {
		// 	h_waveform->SetBinContent(j, h_waveform->GetBinContent(j)-get_baseline(h_waveform, pars.n_bin_getBaseline ));
		// }
		
		if ( toPDF_totally == true && i<n_graph_to_pdf+1 )
		{
			v2D_TH1D_toPDF[(i-1)/4].push_back((TH1D*) h_waveform->Clone(  (TString)n2str(i)+"h_waveform" ));
		}

		if ( divide_signal_and_noise_to_pdf == true and i<400 )
		{
			// if( useThreshold50 == true )
				if ( check_whether_real_signal(h_waveform) == true && find_real_photon_signal(h_waveform) )
				{
					// h_waveform->GetYaxis()->SetRangeUser( h_waveform->GetMinimum(), h_waveform->GetMaximum() );
					v2D_signal_TH1D_toPDF[n_signal/4].push_back((TH1D*) h_waveform->Clone(  (TString)n2str(i)+"h_waveform" ));	
					n_signal++;
				}
				else if ( check_whether_real_signal(h_waveform) )
				{
					v2D_induced_signal_TH1D_toPDF[n_induced_signal/4].push_back( (TH1D*) h_waveform->Clone(  (TString)n2str(i)+"h_waveform" ) );
					n_induced_signal++;
					// cout<< "baseline:   "<< get_baseline(h_waveform)<<"   Minimum:   "<< h_waveform->GetMinimum()<<endl;
				}
				else
				{
					if ( n_noise<50)
					{
						v2D_noise_TH1D_toPDF[n_noise/4].push_back((TH1D*) h_waveform->Clone(  (TString)n2str(i)+"h_waveform" ));	
						n_noise++;
					}
				}
			
		}
		if (check_baseline == true )
		{
			int n_sumBin=50;
			double baseline=0;
			for (int j=0;j<n_sumBin;j++){
				baseline+=h_waveform->GetBinContent(j+1);
			}
	    	baseline/=(double)n_sumBin;
			cout<< "baseline:   " << baseline <<endl;
			cout<< "Bin 100 center:   "<< h_waveform->GetBinCenter(n_sumBin)<<endl;
			cout<< "max:  "<< h_waveform->GetMinimum()<< "MaxBin:   "<<h_waveform->GetMinimumBin()<<endl;
		}

		
		g->cd();
		// cout<<i<<"  OK here!"<<endl;
		//fill the total_waves tree
		str->Fill();
		if ( check_whether_real_signal(h_waveform) == true && find_real_photon_signal(h_waveform) )
		{
			// cout<<"check BigPeak"<<endl;
			h_signalBigPeak = new TH1D(Form("h_BigPeak%i", i), "h_BigPeak", nDimension, 0, nDimension);
			for (int j = 0; j < nDimension; j++)
			{
				h_signalBigPeak->SetBinContent(j, h_waveform->GetBinContent(j) );
			}
			tree_signalBigPeak->Fill();
			h_signalBigPeak->Delete();
		}
		else if ( check_whether_real_signal(h_waveform) )
		{
			// cout<<"check SmallPeak"<<endl;
			h_signalSmallPeak = new TH1D(Form("h_SmallPea%i", i), "h_SmallPeak", nDimension, 0, nDimension);
			for (int j = 0; j < nDimension; j++)
			{
				h_signalSmallPeak->SetBinContent(j, h_waveform->GetBinContent(j) );
			}
			tree_signalSmallPeak->Fill();
			h_signalSmallPeak->Delete();
		}
		else
		{
			// cout<<"check Noise"<<endl;
			h_noise = new TH1D(Form("h_noise%i", i), "h_noise", nDimension, 0, nDimension);
			for (int j = 0; j < nDimension; j++)
			{
				h_noise->SetBinContent(j, h_waveform->GetBinContent(j) );
			}
			tree_noise->Fill();
			h_noise->Delete();
		}
		// cout<< "check delete"<<endl;
		h_waveform->Delete();
	}
	if (toPDF_totally == true) 
	{
		cout<<"entries:   "<<entries<<endl;
		v2D_TH1D_toPDF.resize(n_graph_to_pdf/4);
		plot_into_pdf(v2D_TH1D_toPDF,pars.name_PdfDir+"waves/"+input_name+"_waves_total.pdf");
	}
	if ( divide_signal_and_noise_to_pdf == true)
	{
		// if (useThreshold50== true )
		// {
		// 	name_option.Append("_useThreshold50");
		// }
		
		cout << "n_noise:  " <<n_noise <<"    n_signal:   "<<n_signal<<endl;
		v2D_noise_TH1D_toPDF.resize(n_noise/4);
		v2D_signal_TH1D_toPDF.resize(n_signal/4);
		v2D_induced_signal_TH1D_toPDF.resize(n_induced_signal/4);
		system("mkdir -p "+pars.name_PdfDir+"waves/");
		plot_into_pdf(v2D_signal_TH1D_toPDF,pars.name_PdfDir+"waves/"+newname+"_wavesBigPeak"+name_option+".pdf");	
		plot_into_pdf(v2D_induced_signal_TH1D_toPDF,pars.name_PdfDir+"waves/"+newname+"_wavesSmallPeak"+name_option+".pdf");		
		plot_into_pdf(v2D_noise_TH1D_toPDF,pars.name_PdfDir+"waves/"+newname+"_wavesNoise"+name_option+".pdf");
	}
	str->Write();  
	tree_noise->Write();
	tree_signalSmallPeak->Write();
	tree_signalBigPeak->Write();
	g->Close();
	cout<< "Data has been saved into "+pars.name_RootFilePath+dir<<endl;
	f->Close();
	/*
	 * TCanvas* can=new TCanvas("c1","c1",800,600);
	 can->cd();
	 chData->Draw();
	 */
	}
