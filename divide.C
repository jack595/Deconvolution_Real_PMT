#include "TString.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "pars_waves.h"
#include <vector>
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/Deconvolution_Real_PMT/check_whether_real_signal.h"

void divide(TString name){
	using namespace std;
	pars_waves pars;
	bool debug = false;
	bool divide_signal_and_noise_to_pdf=true;
	bool toPDF_totally=false;
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
	cout<< "entries" << entries<<endl;

	// int namechar=name.First("r");
	// TString newname=name(0,namechar);
	if (debug==true)
	{
		dir.Append("_Debug");
	}
	
	dir.Append("_divide.root");
	
	TFile* g=new TFile(dir,"recreate");
	TTree* str= new TTree("waves","waves");
	TH1D* h_waveform=NULL;
	int pmtID=1;
	str->Branch("waves","TH1D",&h_waveform);
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
		}
		h_waveform->SetBinContent(0,h_waveform->GetBinContent(2));
		if ( toPDF_totally == true && i<n_graph_to_pdf+1 )
		{
			v2D_TH1D_toPDF[(i-1)/4].push_back((TH1D*) h_waveform->Clone(  (TString)n2str(i)+"h_waveform" ));
		}
		if ( divide_signal_and_noise_to_pdf == true and i<400 )
		{
			if( useThreshold50 == true )
			{
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
			else
			{
				if ( check_whether_real_signal(h_waveform) == true 	)
				{
					v2D_signal_TH1D_toPDF[n_signal/4].push_back((TH1D*) h_waveform->Clone(  (TString)n2str(i)+"h_waveform" ));	
					n_signal++;
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
		if (useThreshold50==true)
		{
		if ( check_whether_real_signal(h_waveform) == true && find_real_photon_signal(h_waveform) )
		{
			// cout<<"Filled"<<endl;
			str->Fill();
		}
		}
		else
		{
			if ( check_whether_real_signal(h_waveform) == true )
			{
				// cout<<"Filled"<<endl;
				str->Fill();
			}
		}
		
		if (debug==true && i==entries-5)
		{
			tr->GetEntry(1);
			cout<<  "BinWith:  "<<h_waveform->GetBinWidth(5)<<endl;
			cout<<  "h_wave x range:"<< nDimension <<endl;
		}
		delete h_waveform;
	}
	if (toPDF_totally == true) 
	{
		cout<<"entries:   "<<entries<<endl;
		v2D_TH1D_toPDF.resize(entries/4);
		plot_into_pdf(v2D_TH1D_toPDF,input_name+"waves_SPE.pdf");
	}
	if ( divide_signal_and_noise_to_pdf == true)
	{
		if (useThreshold50== true )
		{
			name_option.Append("_useThreshold50");
		}
		
		cout << "n_noise:  " <<n_noise <<"    n_signal:   "<<n_signal<<endl;
		v2D_noise_TH1D_toPDF.resize(n_noise/4);
		v2D_signal_TH1D_toPDF.resize(n_signal/4);
		v2D_induced_signal_TH1D_toPDF.resize(n_induced_signal/4);
		plot_into_pdf(v2D_signal_TH1D_toPDF,"./output_pdf/"+newname+name_option+"_waves_signal_SPE.pdf");	
		if ( useThreshold50 == true )
		{
			plot_into_pdf(v2D_induced_signal_TH1D_toPDF,"./output_pdf/"+newname+name_option+"_waves_induced_signal_SPE.pdf");
		}
		
		plot_into_pdf(v2D_noise_TH1D_toPDF,"./output_pdf/"+newname+name_option+"_waves_noise_SPE.pdf");
	}
	str->Write();  
	g->Close();
	f->Close();
	/*
	 * TCanvas* can=new TCanvas("c1","c1",800,600);
	 can->cd();
	 chData->Draw();
	 */
	}
