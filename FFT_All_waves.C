#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
void FFT_All_waves(TString name_fileFullPath)
{
	// bool debug = true;
	bool debug = false;
    TString dir="";
	//ostringstream in;
	// in<<"new"<<number<<"average.root";
	// ostringstream out;
	// out<<"SPE"<<number<<"out.root";
	int length=name_fileFullPath.Length();
	TString newname=name_fileFullPath(55,length);
	dir.Append(newname);  
	TString name0=dir;
	TH1* m_tempH;



	TFile* Tf=new TFile(dir.Append("_rearrange.root"),"read"); 

	// TFile* Tf = new TFile("new1average.root", "read");
	// TFile* Tf = new TFile("temp.root", "update");
    TTree* tr_waves= (TTree*)Tf->Get("waves");
	int entries=tr_waves->GetEntries();
	int alterentries=entries;
	TH1D* waveform=NULL;
	tr_waves->SetBranchAddress("waves",&waveform);
	tr_waves->GetEntry(1);
	const int length_waveformTH1D=waveform->GetNbinsX()/2;
	cout << "length of waveform： "<< length_waveformTH1D <<endl;

	//Define the output variable in order to set the Branches
	std::vector<double> re_full_vec(length_waveformTH1D);
	std::vector<double> im_full_vec(length_waveformTH1D);
	double re_full[length_waveformTH1D];
	double im_full[length_waveformTH1D]; 
	double mo_full[length_waveformTH1D];
	ostringstream out_str_re_full;
	out_str_re_full<<"re_full["<<length_waveformTH1D<<"]/D";
	ostringstream out_str_im_full;
	out_str_im_full<<"im_full["<<length_waveformTH1D<<"]/D";
	ostringstream out_str_mo_full;
	out_str_mo_full<<"mo_full["<<length_waveformTH1D<<"]/D";

	//Initial the output File and set the branches
	TFile* outfile = new TFile(name0.Append("_FFT_allWaves.root"), "recreate");
	TTree* tree = new TTree("data","data");
	tree->Branch("re_full",re_full,out_str_re_full.str().c_str());
	tree->Branch("im_full",im_full,out_str_im_full.str().c_str());
	tree->Branch("mo_full",mo_full,out_str_mo_full.str().c_str());
	
	//Initial the TH1Ds which are to draw the re_full and im_full 
	TH1D* h_re=new TH1D("h_re","h_re",length_waveformTH1D,0,length_waveformTH1D);
	TH1D* h_im=new TH1D("h_im","h_im",length_waveformTH1D,0,length_waveformTH1D);
	TH1D* h_mo=new TH1D("h_mo","h_mo",length_waveformTH1D,0,length_waveformTH1D);
	TH2D* h2D_raw=new TH2D("h2D_raw","h2D_raw",
							length_waveformTH1D,0,length_waveformTH1D,
							length_waveformTH1D,0,pow(10,7)*1.5);

	// for (int i = 0; i < length_waveformTH1D ; i++)
	// {
	// 	re_full[i]=waveform->GetBinContent(i+1);
	// 	im_full[i]=waveform->GetBinContent(i+1);
	// 	mo_full[i]=waveform->GetBinContent(i+1);

	// }
	// tree->Fill();

	if(debug==true) entries=10000;

    for (int i = 0; i < entries; i++)
    {	
		if(i%1000==0)cout<< i<< " waveforms finished! " <<endl;
        tr_waves->GetEntry(i);
		delete TVirtualFFT::GetCurrentTransform();
		TVirtualFFT::SetTransform(0);
		m_tempH = waveform->FFT(NULL, "MAG");
		delete m_tempH;
		m_tempH = NULL;

		for (int j = 0; j < length_waveformTH1D; j++) 
		{
			re_full[j] = 0;
			im_full[j] = 0;
		}
		TVirtualFFT* fft = TVirtualFFT::GetCurrentTransform();   
		fft->GetPointsComplex(re_full, im_full);
		for (int j = 0; j < length_waveformTH1D ; j++)
		{
			mo_full[j]=re_full[j]*re_full[j]+im_full[j]*im_full[j];
			h2D_raw->Fill( j , mo_full[j] );	
		}
		tree->Fill();
		if (debug==true && i<8 )
		{
			for (int z = 0; z < length_waveformTH1D ; z++)
			{
				h_re->SetBinContent(z+1,re_full[z]);
				h_im->SetBinContent(z+1,im_full[z]);
				h_mo->SetBinContent(z+1,mo_full[z]);
			}
			TCanvas *c1=new TCanvas("c_re"+(TString)n2str(i),"c_re"+(TString)n2str(i),800*3,600);
			c1->Divide(3,1);
			c1->cd(1);
			h_re->DrawCopy();
			c1->cd(2);
			h_im->DrawCopy();
			c1->cd(3);
			h_mo->DrawCopy();
			
		}
		
    }
	if (debug==true)
	{
		TCanvas *c2=new TCanvas("c_raw2D","c_raw2D",800,600);
		h2D_raw->DrawCopy("colz");
	}
	
	h2D_raw->Write();
	tree->Write();
	outfile->Close();
	Tf->Close();
    
    // //-------------The rest is waiting for edit-------------//
    
    
	// TH1F* v_waves[20000];
	
	// TH1D* m_Counter;
	// int m_totalPMT = 1;
	// int m_length = 10204;
	// TH1* m_tempH;
	// TH1D* m_SPERE[20000];
	// TH1D* m_SPEIM[20000];
	// TH1D* m_SPEMO; 
	// for (int i = 0; i < m_totalPMT; i++) {
	// 	ostringstream out1;
	// 	out1 << "SPERE";
	// 	m_SPERE[i] = new TH1D(out1.str().c_str(), out1.str().c_str(), 400, 0, 400);  // specified for J16v2
	// 	ostringstream out2;
	// 	out2 << "SPEIM";
	// 	m_SPEIM[i] = new TH1D(out2.str().c_str(), out2.str().c_str(), 400, 0, 400);  // specified for J16v2
	// 	ostringstream out3;
	// 	m_SPEMO=new TH1D("SPEMO","SPEMO",200,0,200);
	// 	out3 << "PMTID_" << i << "_MeanSpec";
	// 	// v_waves[i]=(TH1D*)Tf->Get(out3.str().c_str());

	// 	v_waves[i] = (TTree*)Tf->Get("waves");


	// 	//  for(int j=0;j<250;j++) {v_waves[i]->SetBinContent(j+1,v_waves[i]->GetBinContent(250));}
	// 	//  for(int j=650;j<1024;j++){v_waves[i]->SetBinContent(j+1,v_waves[i]->GetBinContent(650));}

	// }
	// ostringstream out4;
	// out4 << "PMT_Counter";
	// // m_Counter = (TH1D*)Tf->Get(out4.str().c_str());

	// for (int i = 0; i < m_totalPMT; i++) {
	// 	//   v_waves[i]->Scale(1. / m_Counter->GetBinContent(i + 1));
	// 	delete TVirtualFFT::GetCurrentTransform();
	// 	TVirtualFFT::SetTransform(0);
	// 	m_tempH = v_waves[i]->FFT(NULL, "MAG");
	// 	delete m_tempH;
	// 	m_tempH = NULL;
	// 	std::vector<double> re_full_vec(m_length);
	// 	std::vector<double> im_full_vec(m_length);
	// 	double* re_full = &re_full_vec.front();
	// 	double* im_full = &im_full_vec.front();
	// 	for (int j = 0; j < m_length; j++) {
	// 		re_full[j] = 0;
	// 		im_full[j] = 0;
	// 	}
	// 	/*
	// 	   TVirtualFFT* fft ;
	// 	   if (NULL!=TVirtualFFT::GetCurrentTransform()){
	// 	   fft = TVirtualFFT::GetCurrentTransform();
	// 	   } 
	// 	   */
	// 	TVirtualFFT* fft = TVirtualFFT::GetCurrentTransform();   
	// 	fft->GetPointsComplex(re_full, im_full);
	// 	for (int j = 0; j < 400 && j < m_length; j++) {
	// 		m_SPERE[i]->SetBinContent(j + 1, re_full[j]);
	// 		m_SPEIM[i]->SetBinContent(j + 1, im_full[j]);
	// 		m_SPEMO->SetBinContent(j+1,re_full[j]*re_full[j]+im_full[j]*im_full[j]); 
	// 	}
	// }
	// TFile* outfile = new TFile(name0.Append("_out.root"), "recreate");
	// outfile->cd();
	// for (int i = 0; i < m_totalPMT; i++) {
	// 	v_waves[i]->Write();
	// 	m_SPERE[i]->Write();
	// 	m_SPEIM[i]->Write();
	// 	m_SPEMO->Write(); 
	// 	TCanvas* can=new TCanvas("c1","c1",800,600);
	// 	can->cd();
	// 	m_SPERE[i]->SetLineColor(2);
	// 	m_SPEIM[i]->SetLineColor(3);
	// 	m_SPERE[i]->DrawCopy("l");
	// 	m_SPEIM[i]->DrawCopy("lsame");
	// 	TCanvas* can1=new TCanvas("c2","c2",800,600);
	// 	m_SPEMO->DrawCopy("l");	
		
	// }

    
}