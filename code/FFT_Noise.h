#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "pars_waves.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "check_whether_real_signal.h"
void FFT_Noise(TString name_fileFullPath, pars_waves pars)
{
	bool debug = false;
	bool plot_FFT_aWaveIntoPdf=true;
	//bool debug = false;
    TString dir="";
	system("mkdir -p "+pars.name_PdfDir+"FFT_Noise/");
	//ostringstream in;
	// in<<"new"<<number<<"average.root";
	// ostringstream out;
	// out<<"SPE"<<number<<"out.root";
	int length=name_fileFullPath.Length();
	TString newname=name_fileFullPath(55,length);
	dir.Append(newname);  
	TString name0=dir;
	TH1* m_tempH;
	cout<<"check"<<endl;


	TFile* Tf=new TFile( pars.name_RootFilePath+dir.Append("_divide.root"),"read"); 

	// TFile* Tf = new TFile("new1average.root", "read");
	// TFile* Tf = new TFile("temp.root", "update");
    TTree* tr_waves= (TTree*)Tf->Get("noise");
	int entries=tr_waves->GetEntries();
	int alterentries=entries;
	TH1D* waveform=NULL;
	tr_waves->SetBranchAddress("noise",&waveform);
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
	TFile* outfile = new TFile( pars.name_RootFilePath+newname.Append("_FFT_noise.root"), "recreate");
	cout<< "Saving data to "+newname<<endl;
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
							length_waveformTH1D,0,pow(10,3)*1.5);

	// for (int i = 0; i < length_waveformTH1D ; i++)
	// {
	// 	re_full[i]=waveform->GetBinContent(i+1);
	// 	im_full[i]=waveform->GetBinContent(i+1);
	// 	mo_full[i]=waveform->GetBinContent(i+1);

	// }
	// tree->Fill();

	TString name_saveFFT_aWavePdf=pars.name_PdfDir+"FFT_Noise/"+newname+"_FFT_noise.pdf";
	TCanvas *c1=new TCanvas("c_FFT_noise","c_FFT_noise",800*3,600);
	c1->Divide(3,1);
	c1->Print(name_saveFFT_aWavePdf+"[");

	if(debug==true) entries=10000;
	if (entries>2*pow(10,5))
	{
		entries=2*pow(10,5);
	}
	
	int countSpe=0;
    for (int i = 0; i < entries; i++)
    {	
		countSpe++;
		if(i%1000==0)cout<< i<< " waveforms finished! " <<endl;
        tr_waves->GetEntry(i);
		
		double baseline = get_baseline(waveform, pars.n_bin_getBaseline);
		for (int j = 0; j < pars.nDimension; j++)
		{
			waveform->SetBinContent(j, baseline-waveform->GetBinContent(j) );
		}
		// cout<<i<<endl;

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
			mo_full[j]=sqrt(re_full[j]*re_full[j]+im_full[j]*im_full[j]);
            h2D_raw->Fill( j , mo_full[j]) ;	
		}
		tree->Fill();
		if (plot_FFT_aWaveIntoPdf==true && countSpe<=15 )
		{
			for (int z = 0; z < length_waveformTH1D ; z++)
			{
				h_re->SetBinContent(z+1,re_full[z]);
				h_im->SetBinContent(z+1,im_full[z]);
				h_mo->SetBinContent(z+1,mo_full[z]);
			}

			c1->cd(1);
			h_re->DrawCopy();
			c1->cd(2);
			h_im->DrawCopy();
			c1->cd(3);
			h_mo->DrawCopy();
			cout<<countSpe<<endl;
			c1->Print(name_saveFFT_aWavePdf);
			c1->Clear();
			c1->Divide(3,1);
		}
		
    }
		c1->Print(name_saveFFT_aWavePdf+"]");
	//if (debug==true)
	
        h2D_raw->SetBinContent(1,1,0);
		TCanvas *c2=new TCanvas("c_raw2D","c_raw2D",800,600);
		// c2->SetLogy();
		h2D_raw->SetTitle("h2D_Modulus_Noise, Entries: "+n2str(countSpe));
		h2D_raw->SetXTitle("Frequency (1/ns) ");
		h2D_raw->DrawCopy("colz");
		c2->SaveAs(pars.name_PdfDir+"FFT_Noise/"+newname+"_FFT_noise_TH2D.png");
	
	//Save data and Terminate the script
	h2D_raw->Write();
	tree->Write();
	outfile->Close();
	Tf->Close();
    
}
