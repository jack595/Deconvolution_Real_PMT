#include "/afs/ihep.ac.cn/users/z/zhangxt/include/myincludes.h"
void getFilterSetting3_m( TString name_include_path )
{
 	TString dir="";
	int Length=name_include_path.Length();
	TString newname=name_include_path(55,Length);
	dir.Append(newname);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TFile* spehf = new TFile( dir+"_FFT_allWaves.root", "read");
  TH2D* rawh2D = (TH2D*)spehf->Get("h2D_raw");
  TH1D* projh[600];
  TH1D* meanh = new TH1D("meanh", "meanh", 600, 0, 600);
  TString name;
  for (int i = 0; i < 600; i++) {
    name = "projyh_";
    name += i;
    projh[i] = rawh2D->ProjectionY(name, i + 1, i + 1, "");
    //projh[i]->Smooth(3);
    meanh->SetBinContent(i + 1, pow(projh[i]->GetBinCenter(projh[i]->GetMaximumBin()), 2));
    cout<< projh[i]->GetBinCenter(projh[i]->GetMaximumBin())<<endl;
  }

  TH1D* backh = (TH1D*)meanh->Clone();

  for (int i = 0; i < 600; i++) {
    meanh->AddBinContent(i + 1, -200 * 200);
  }
  meanh->Divide(backh);
  // meanh->Draw();
  // meann->Draw("same");

  // smooth
  TGraph* tfh = new TGraph();
  for (int i = 1; i < 600; i++) {
    tfh->SetPoint(i - 1, meanh->GetBinCenter(i + 1), meanh->GetBinContent(i + 1));
  }
  TGraphSmooth* gsh = new TGraphSmooth("normal");
  TGraph* grouth = gsh->SmoothSuper(tfh, "", 3);
  // TGraph* grouth = gsh->SmoothLowess(tfh, "", 0.2);

  TH1D* fh0 = new TH1D("fh0", "fh0", 1250, 0, 1250);
  double xx, yy;
  fh0->SetBinContent(1, 0);
  int ih = 0;
  for (int i = 1; i < 1250; i++) {
    grouth->GetPoint(i - 1, xx, yy);
    if (yy < 0) {
      ih = i;
      break;
    }
    fh0->SetBinContent(i + 1, yy);
  }
  for (int i = 0; i < 1250; i++) {
    fh0->SetBinContent(i + 1, (fh0->GetBinContent(i + 1)));
  }

  TCanvas* ch = new TCanvas("hc", "hc", 800, 600);
  TList* lsh = new TList();
  TString cname[2] = {"Av_f", "Filter"};
  lsh->Add(meanh);
  lsh->Add(fh0);
  compare(ch->cd(), lsh, 2, cname, 0);

  TFile* savef = new TFile(dir+"filter3_m.root", "recreate");
  savef->cd();
  rawh2D->Write();
  meanh->Write();
  fh0->Write();
}
