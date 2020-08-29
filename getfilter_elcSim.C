#include "/afs/ihep.ac.cn/users/z/zhangxt/include/myincludes.h"
void getFilterSetting3_m()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  bool* hmmtpmt = pmtflag();
  TFile* spehf = new TFile("spef_h.root", "read");
  TFile* spenf = new TFile("spef_n.root", "read");
  TH2D* rawh2D = (TH2D*)spehf->Get("raw2D");
  TH2D* rawn2D = (TH2D*)spenf->Get("raw2D");
  TH1D* projh[600];
  TH1D* projn[600];
  TH1D* meanh = new TH1D("meanh", "meanh", 600, 0, 600);
  TH1D* meann = new TH1D("meann", "meann", 600, 0, 600);
  TString name;
  for (int i = 0; i < 600; i++) {
    name = "projyh_";
    name += i;
    projh[i] = rawh2D->ProjectionY(name, i + 1, i + 1, "");
    name = "projyn_";
    name += i;
    projn[i] = rawn2D->ProjectionY(name, i + 1, i + 1, "");
    projh[i]->Smooth(3);
    projn[i]->Smooth(3);
    meanh->SetBinContent(i + 1, pow(projh[i]->GetBinCenter(projh[i]->GetMaximumBin()), 2));
    meann->SetBinContent(i + 1, pow(projn[i]->GetBinCenter(projn[i]->GetMaximumBin()), 2));
  }

  TH1D* backh = (TH1D*)meanh->Clone();
  TH1D* backn = (TH1D*)meann->Clone();

  for (int i = 0; i < 600; i++) {
    meanh->AddBinContent(i + 1, -200 * 200);
    meann->AddBinContent(i + 1, -200 * 200);
  }
  meanh->Divide(backh);
  meann->Divide(backn);
  // meanh->Draw();
  // meann->Draw("same");

  // smooth
  TGraph* tfh = new TGraph();
  TGraph* tfn = new TGraph();
  for (int i = 1; i < 600; i++) {
    tfh->SetPoint(i - 1, meanh->GetBinCenter(i + 1), meanh->GetBinContent(i + 1));
    tfn->SetPoint(i - 1, meann->GetBinCenter(i + 1), meann->GetBinContent(i + 1));
  }
  TGraphSmooth* gsh = new TGraphSmooth("normal");
  TGraph* grouth = gsh->SmoothSuper(tfh, "", 3);
  // TGraph* grouth = gsh->SmoothLowess(tfh, "", 0.2);
  TGraphSmooth* gsn = new TGraphSmooth("normal");
  TGraph* groutn = gsn->SmoothSuper(tfn, "", 3);
  // TGraph* groutn = gsn->SmoothLowess(tfn, "", 0.2);
  // grouth->Draw();
  // groutn->Draw("same");

  TH1D* fh0 = new TH1D("fh0", "fh0", 1250, 0, 1250);
  TH1D* fn0 = new TH1D("fn0", "fn0", 1250, 0, 1250);
  double xx, yy;
  fh0->SetBinContent(1, 0);
  fn0->SetBinContent(1, 0);
  int ih = 0;
  int in = 0;
  for (int i = 1; i < 1250; i++) {
    grouth->GetPoint(i - 1, xx, yy);
    if (yy < 0) {
      ih = i;
      break;
    }
    fh0->SetBinContent(i + 1, yy);
  }
  for (int i = 1; i < 1250; i++) {
    groutn->GetPoint(i - 1, xx, yy);
    if (yy < 0) {
      in = i;
      break;
    }
    fn0->SetBinContent(i + 1, yy);
  }
  for (int i = 0; i < 1250; i++) {
    fh0->SetBinContent(i + 1, (fh0->GetBinContent(i + 1)));
    fn0->SetBinContent(i + 1, (fn0->GetBinContent(i + 1)));
  }

  TCanvas* ch = new TCanvas("hc", "hc", 800, 600);
  TCanvas* cn = new TCanvas("nc", "nc", 800, 600);
  TList* lsh = new TList();
  TList* lsn = new TList();
  TString cname[2] = {"Av_f", "Filter"};
  lsh->Add(meanh);
  lsh->Add(fh0);
  lsn->Add(meann);
  lsn->Add(fn0);
  compare(ch->cd(), lsh, 2, cname, 0);
  compare(cn->cd(), lsn, 2, cname, 0);

  TFile* savef = new TFile("filter3_m.root", "recreate");
  savef->cd();
  rawh2D->Write();
  rawn2D->Write();
  meanh->Write();
  meann->Write();
  fh0->Write();
  fn0->Write();
}
