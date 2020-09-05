#include "TH1D.h"
#include "pars_waves.h"
void script(TString name)
{
	pars_waves pars;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
	const int nDimension = pars.nDimension;
	 TString dir="";
	//ostringstream in;
	// in<<"new"<<number<<"average.root";
	// ostringstream out;
	// out<<"SPE"<<number<<"out.root";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);  
	TString name0=dir;
	TFile* f=new TFile(dir.Append("_average.root"),"read"); 

	// TFile* f = new TFile("new1average.root", "read");
	// TFile* f = new TFile("temp.root", "update");
	TH1F* m_meanWaveform[20000];
	TH1D* m_Counter;
	int m_totalPMT = 1;
	int m_length = 10204;
	TH1* m_tempH;
	TH1D* m_SPERE[20000];
	TH1D* m_SPEIM[20000];
	TH1D* m_SPEMO; 
	for (int i = 0; i < m_totalPMT; i++) {
		ostringstream out1;
		out1 << "SPERE";
		m_SPERE[i] = new TH1D(out1.str().c_str(), out1.str().c_str(), nDimension/2, 0, nDimension/2);  // specified for J16v2
		ostringstream out2;
		out2 << "SPEIM";
		m_SPEIM[i] = new TH1D(out2.str().c_str(), out2.str().c_str(), nDimension/2, 0, nDimension/2);  // specified for J16v2
		ostringstream out3;
		m_SPEMO=new TH1D("SPEMO","SPEMO",nDimension/2,0,nDimension/2);
		out3 << "PMTID_" << i << "_MeanSpec";
		// m_meanWaveform[i]=(TH1D*)f->Get(out3.str().c_str());

		m_meanWaveform[i] = (TH1F*)f->Get("averageHist");


		//  for(int j=0;j<250;j++) {m_meanWaveform[i]->SetBinContent(j+1,m_meanWaveform[i]->GetBinContent(250));}
		//  for(int j=650;j<nDimension;j++){m_meanWaveform[i]->SetBinContent(j+1,m_meanWaveform[i]->GetBinContent(650));}

	}
	ostringstream out4;
	out4 << "PMT_Counter";
	// m_Counter = (TH1D*)f->Get(out4.str().c_str());

	for (int i = 0; i < m_totalPMT; i++) {
		//   m_meanWaveform[i]->Scale(1. / m_Counter->GetBinContent(i + 1));
		delete TVirtualFFT::GetCurrentTransform();
		TVirtualFFT::SetTransform(0);
		m_tempH = m_meanWaveform[i]->FFT(NULL, "MAG");
		delete m_tempH;
		m_tempH = NULL;
		std::vector<double> re_full_vec(m_length);
		std::vector<double> im_full_vec(m_length);
		double* re_full = &re_full_vec.front();
		double* im_full = &im_full_vec.front();
		for (int j = 0; j < m_length; j++) {
			re_full[j] = 0;
			im_full[j] = 0;
		}
		/*
		   TVirtualFFT* fft ;
		   if (NULL!=TVirtualFFT::GetCurrentTransform()){
		   fft = TVirtualFFT::GetCurrentTransform();
		   } 
		   */
		TVirtualFFT* fft = TVirtualFFT::GetCurrentTransform();   
		fft->GetPointsComplex(re_full, im_full);
		for (int j = 0; j < nDimension/2 && j < m_length; j++) {
			m_SPERE[i]->SetBinContent(j + 1, re_full[j]);
			m_SPEIM[i]->SetBinContent(j + 1, im_full[j]);
			m_SPEMO->SetBinContent(j+1,re_full[j]*re_full[j]+im_full[j]*im_full[j]); 
		}
	}
	TFile* outfile = new TFile(name0.Append("_out.root"), "recreate");
	outfile->cd();
	for (int i = 0; i < m_totalPMT; i++) {
		m_meanWaveform[i]->Write();
		m_SPERE[i]->Write();
		m_SPEIM[i]->Write();
		m_SPEMO->Write(); 
		TCanvas* can=new TCanvas("c1","c1",800,600);
		can->cd();
		m_SPERE[i]->SetLineColor(2);
		m_SPEIM[i]->SetLineColor(3);
		m_SPERE[i]->DrawCopy("l");
		m_SPEIM[i]->DrawCopy("lsame");
		TCanvas* can1=new TCanvas("c2","c2",800,600);
		m_SPEMO->DrawCopy("l");	
		
	}
}
