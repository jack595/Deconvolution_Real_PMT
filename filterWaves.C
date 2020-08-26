#include "/afs/ihep.ac.cn/users/z/zhangxt/include/myincludes.h"
// void filterWaves()
void filterWaves(TString name)
{
  TH1D* spere=NULL;
  TH1D* speim=NULL;
  gStyle->SetOptStat(0);
 // bool* hmmtpmt = pmtflag();
  // TFile* spef = new TFile("/junofs/users/zhangxt/20inch/rec/deconvolution/runs/20191009sample/Laser/center/photon_22000/SPE_v19.test.root", "read");
  // TFile* spef = new TFile("/junofs/users/zhangxt/20inch/rec/deconvolution/runs/20191009sample/ACU/Ge68/Ge68_0_0_0000/SPE_v19.test.root", "read");
 // TFile* spef = new TFile("/junofs/users/zhangxt/20inch/rec/deconvolution/runs/20200504sample-J20v1r0-Pre0/wfExt/SPE_v20.root", "read");
TString name0=name;
TString name1=name;
TString name2=name;
TFile* spef=new TFile(name0.Append("_out.root"),"read"); 
 //TString spename;
/*
  for (int i = 0; i < 17613; i++) {
    spename = Form("PMTID_%d_SPERE", i);
    spere[i] = (TH1D*)spef->Get(spename);
    spere[i]->SetDirectory(0);
   spename = Form("PMTID_%d_SPEIM", i);
    speim[i] = (TH1D*)spef->Get(spename);
    speim[i]->SetDirectory(0);
    if (i % 5000 == 0) cout << i << " finished!" << endl;
  }
spere
*/
spere=(TH1D*)spef->Get("SPERE");
spere->SetDirectory(0);
speim=(TH1D*)spef->Get("SPEIM");
speim->SetDirectory(0);
  spef->Close();
  cout << "SPE spectra loaded successfully!" << endl;

//  TFile* ffile = new TFile("/junofs/users/zhangxt/20inch/rec/deconvolution/runs/anaAlg/filtercompare/scriptv2/newfilter/filter3_m.root", "read");
TFile* ffile=new TFile(name.Append("_filter.root"),"read");
  // TH1D* h_filter0 = filter(50, 350, 30);
  // h_filter0->SetNameTitle("f0", "f0");
  TH1D* h_filter0 = new TH1D("f0", "f0", 1024, 0, 1024);
  for (int i = 0; i < 1024; i++) h_filter0->SetBinContent(i + 1, 1.);

  // nnvt
  // TH1D* h_filter1 = filter(100, 150, 15);
  // TH1D* h_filter1 = filter(10, 188, 83);
  // TH1D* h_filter1 = (TH1D*)ffile->Get("fn1");
  TH1D* h_filter1 = (TH1D*)ffile->Get("filter");
  // h_filter1->SetNameTitle("f1", "f1");

  // hmmt
  // TH1D* h_filter2 = filter(80, 130, 15);
  // TH1D* h_filter2 = filter(10, 200, 63);
  // TH1D* h_filter2 = (TH1D*)ffile->Get("fh1");
  TH1D* h_filter2 = (TH1D*)ffile->Get("filter");
 
  
	// h_filter2->SetNameTitle("f2", "f2");

  cout << "Filters Generated Successfully!" << endl;
  TFile* f=new TFile(name1.Append("_rearrange.root")); 
  //TFile* f = new TFile("waves.root", "read");
  TH1D* w_tmp = 0;
  int id = 0;
 // TString name;
  
  TTree* tr = (TTree*)f->Get("waves");
  int entry=tr->GetEntries();
  tr->SetBranchAddress("waves", &w_tmp);
 tr->GetEntry(2);
 /* TCanvas* can=new TCanvas("c1","c1",800,600);
    can->cd();
    w_tmp->Draw();
*/
//  tr->SetBranchAddress("pmtid", &id);
  cout << "Total Events: " << tr->GetEntries() << endl;
//return 0;
  // TFile* sf = new TFile("waves_trans.root", "recreate");
 TFile* sf=new TFile(name2.Append("_trans.root"),"recreate");
 // TFile* sf = new TFile(savename, "recreate");
  sf->cd();
  TH1D* rf = 0;
  TH1D* ff = 0;
  TH1D* ft = 0;
  TH1D* df = 0;
  TH1D* w = 0;
  TH1D* spe2raw = 0;
  double arr[1024];
  TTree* str = new TTree("waves_trans", "waves_trans");
  str->Branch("rawfmag", "TH1D", &rf);
  str->Branch("rawh", "TH1D", &w_tmp);
  str->Branch("filteredfmag", "TH1D", &ff);
  str->Branch("filteredt", "TH1D", &ft);
  str->Branch("dividedfmag", "TH1D", &df);
  str->Branch("dividedt", "TH1D", &w);
  str->Branch("spe2raw", "TH1D", &spe2raw);
//  str->Branch("pmtid", &id);

  int start =0;
  int end = entry;
  // start *= 4;
  // end *= 4;
  cout << "Start: " << start << ", End: " << end - 1 << ", SaveName: " << name2 << endl;
  int k = 0;
  wavedec* wd = new wavedec();
  // for (int i = 0; i < tr->GetEntries(); i++) {
  for (int i = start; i < end; i++) {
    tr->GetEntry(i);
    //if (true) {
      // wd->set_input(w_tmp, h_filter0, spere[id], speim[id]);
      wd->set_input(w_tmp, h_filter2, spere, speim);
      wd->transform();
      rf = wd->get_rawfmag();
      ff = wd->get_filteredfmag();
      ft = wd->get_filteredt();
      df = wd->get_dividedfmag();
      w = wd->get_dividedt();
      spe2raw = (TH1D*)h_filter2->Clone();
      spe2raw->Divide(w_tmp);
      spe2raw->SetNameTitle("spe2raw", "spe2raw");
      k = 0;
      for (int j = 924; j < 1024; j++) arr[k++] = w->GetBinContent(j + 1);
      for (int j = 0; j < 924; j++) {
        arr[k++] = w->GetBinContent(j + 1);
        w->SetBinContent(j + 1, arr[j]);
      }
      for (int j = 924; j < 1024; j++) w->SetBinContent(j + 1, arr[j]);
    
/* 
else {
      // wd->set_input(w_tmp, h_filter0, spere[id], speim[id]);
      wd->set_input(w_tmp, h_filter1, spere[id], speim[id]);
      wd->transform();
      rf = wd->get_rawfmag();
      ff = wd->get_filteredfmag();
      ft = wd->get_filteredt();
      df = wd->get_dividedfmag();
      w = wd->get_dividedt();
      spe2raw = (TH1D*)h_filter1->Clone();
      spe2raw->Divide(w_tmp);
      spe2raw->SetNameTitle("spe2raw", "spe2raw");
      k = 0;
      for (int j = 1150; j < 1250; j++) arr[k++] = w->GetBinContent(j + 1);
      for (int j = 0; j < 1150; j++) {
        arr[k++] = w->GetBinContent(j + 1);
        w->SetBinContent(j + 1, arr[j]);
      }
      for (int j = 1150; j < 1250; j++) w->SetBinContent(j + 1, arr[j]);
    }
*/
    w->SetNameTitle(w_tmp->GetName(), w_tmp->GetTitle());
    str->Fill();
    if (i % 1000 == 0) cout << i << " waveforms finished!" << endl;
    if (i==-1){
   	TCanvas* can=new TCanvas("c1","c1",800,600);
    can->cd();
    w->Draw();

}
	}
  sf->cd();
  str->Write();
}
