#include "/afs/ihep.ac.cn/users/z/zhangxt/include/myincludes.h"
void draw2()
{
  bool* hmmtpmt = pmtflag();
  TH1D* spemag[1];
  TFile* spef = new TFile("test_20200704_174034_2mHV_0.1mHVCB_0.2mLEMO_MCP_1800V_75ohm_rearrage.root", "read");
  TString spename;
  TTree* waveforms=(TTree*)spef->Get("waves");
TH1D* waveform=NULL;
waveforms->SetBranchAddress("waves",&waveform);
  // TFile* f = new TFile("newfilter/waves_trans_Ge68_newfilter.root", "read");
  // TFile* f = new TFile("newfilter/waves_trans_Ge68_newfilter2.root", "read");
  // TFile* f = new TFile("newfilter/waves_trans_Ge68_newfilter2_0.root", "read");
  // TFile* f = new TFile("newfilter/waves_trans_Ge68_newfilter3.root", "read");
  
  TFile* f = new TFile("test_20200704_174034_2mHV_0.1mHVCB_0.2mLEMO_MCP_1800V_75ohm_trans.root", "read");
  // TFile* f = new TFile("waves_trans_Ge68_n.root", "read");
  TTree* t1 = (TTree*)f->Get("waves_trans");
  TH1D* rawfmag = 0;
  TH1D* rawh = 0;
  TH1D* filteredfmag = 0;
  TH1D* filteredt = 0;
  TH1D* dividedfmag = 0;
  TH1D* dividedt = 0;
  TH1D* spe2raw = 0;
  TH2D* spe2raw2D = new TH2D("spe2raw2D", "SPE / Raw Distribution", 600, 0, 600, 500, 0, 2);
  TH2D* raw2D = new TH2D("raw2D", "Raw Distribution", 600, 0, 600, 5000, 0, 5000);
  TH2D* raw2D2 = new TH2D("raw2D2", "Raw Squre Distribution", 600, 0, 600, 5000, 0, 25000000);
  int id = 0;
  t1->SetBranchAddress("rawfmag", &rawfmag);
  t1->SetBranchAddress("rawh", &rawh);
  t1->SetBranchAddress("filteredfmag", &filteredfmag);
  t1->SetBranchAddress("filteredt", &filteredt);
  t1->SetBranchAddress("dividedfmag", &dividedfmag);
  t1->SetBranchAddress("dividedt", &dividedt);
  t1->SetBranchAddress("spe2raw", &spe2raw);
 // t1->SetBranchAddress("pmtid", &id);
  cout << "Total Events: " << t1->GetEntries() << endl;

  TList* magls = new TList();
  TList* tls = new TList();
  TString magn[4] = {"Raw", "Filtered", "Final", "SPE"};
  TString tn[3] = {"Raw", "Filtered", "Final"};
  TCanvas* outc = new TCanvas("outc", "Comparison", 1600, 600);
  outc->Divide(2, 1);
  outc->Print("figout.pdf[");
  // for (int ent = 0; ent < t1->GetEntries(); ent++) {
  for (int ent = 0; ent < 100; ent++) {
    t1->GetEntry(ent);
	
	 // draw examples
    if (ent < 100) {
      magls->Clear();
      magls->Add(rawfmag);
      magls->Add(filteredfmag);
      magls->Add(dividedfmag);
      //spename = Form("PMTID_%d_SPEMEG", id);
 //    TH1D* waveforms= (TH1D*)spef->Get("waves");
//	 magls->Add((TH1D*)spef->Get("waves"));
  magls->Add(waveform);    	
if (ent==-1)    {dividedt->Draw();
                        return 0;
}
	
	compare(outc->cd(1), magls, 4, magn, 1);
      rawfmag->GetYaxis()->SetRangeUser(0.01, 10000);
      tls->Clear();
      tls->Add(rawh);
      tls->Add(filteredt);
      dividedt->Scale(200);
      tls->Add(dividedt);
      compare(outc->cd(2), tls, 3, tn, 0);
      outc->Print("figout.pdf");
    }

    for (int i = 0; i < 200; i++) {
      spe2raw2D->Fill(dividedfmag->GetBinCenter(i + 1), 1. / dividedfmag->GetBinContent(i + 1));
      raw2D->Fill(rawfmag->GetBinCenter(i + 1), rawfmag->GetBinContent(i + 1));
      raw2D2->Fill(rawfmag->GetBinCenter(i + 1), pow(rawfmag->GetBinContent(i + 1), 2));
    }
    if (ent % 10000 == 0) cout << ent << " waveforms finished!" << endl;
  }
  outc->Print("figout.pdf]");
  // TCanvas* spe2raw2Dc = new TCanvas("spe2raw2Dc", "spe2raw2Dc", 800, 600);
  // spe2raw2Dc->cd();
  // spe2raw2D->Draw("colz");
  //// TFile* sf = new TFile("spef.root", "recreate");
  // TFile* sf = new TFile("spef_h.root", "recreate");
  //// TFile* sf = new TFile("spef_newfilter2.root", "recreate");
  // sf->cd();
  // spe2raw2D->Write();
  // raw2D->Write();
  // raw2D2->Write();
}
