#include "/afs/ihep.ac.cn/users/z/zhangxt/include/myincludes.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "pars_waves.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH1.h"
void getFilterSetting3_m( TString name_include_path )
{
  bool debug = true ;
  bool retain_FilterPeakOnBrae=false;
  bool retain_FilterPeak = false;
	pars_waves pars;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
	const int nDimension = pars.nDimension;
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
  TH1D* filter_m = new TH1D("filter", "filter", nDimension, 0, nDimension );
  rawh2D->Smooth(3);
  if ( debug == true )
  {
      TCanvas *c_2D=new TCanvas("raw2D","raw2D",800,600);	
			rawh2D ->DrawCopy("colz");
  }
  

  for (int i = 0; i < 600; i++) {
    name = "projyh_";
    name += i;
    projh[i] = rawh2D->ProjectionY(name, i + 1, i + 1, "");
    projh[i]->Smooth(3);
    // meanh->SetBinContent(i + 1, pow(projh[i]->GetBinCenter(projh[i]->GetMaximumBin()), 2));
    meanh->SetBinContent(i + 1, projh[i]->GetBinCenter(projh[i]->GetMaximumBin()));
    // cout<< projh[i]->GetBinCenter(projh[i]->GetMaximumBin())<<endl;
  }

  if ( debug == true )
  {
    // for (int i = 0; i < 10; i++)
    // {
    //   TCanvas *c1=new TCanvas("projy"+(TString)n2str(i),"projy"+(TString)n2str(i),800,600);	
		// 	projh[i]->DrawCopy();
    // }
      TCanvas *c2=new TCanvas("meanh","meanh",800,600);	
			meanh ->DrawCopy();
  }
  
  double baseline_frequency = 0;
  int n_sumBin=100;
  for (int k = nDimension/2-n_sumBin ; k < nDimension/2; k++)
  {
    baseline_frequency += meanh->GetBinContent(k);
  }
  baseline_frequency /= n_sumBin ;
  // cout << "baseline frequency:   "<< baseline_frequency <<endl;

  for (int i = 0; i < nDimension/2 ; i++)
  {
    filter_m->SetBinContent( i,
     ( pow(meanh->GetBinContent(i),2)-pow(baseline_frequency,2))/pow(meanh->GetBinContent(i),2) );
  }
  
  //Get the filter baseline's maximum
  double max_FilterBaseline=-1000000;
  for (int i = nDimension/2-50 ; i < nDimension/2 ; i++)
  {
    if( filter_m->GetBinContent(i) > max_FilterBaseline ) max_FilterBaseline=filter_m->GetBinContent(i);
  }
  // cout << "filter baseline :  "<<max_FilterBaseline<<endl;

  if ( debug == true )
  {
      TCanvas *c4=new TCanvas("c_filter_before_smooth","c_filter_before_smooth",800,600);	
			filter_m ->DrawCopy(); 
  }
  
  if ( retain_FilterPeak == true)
  {
      bool is_peakBehind=false;
      for (int i = 0; i < nDimension/2 ; i++)
      {
        if ( filter_m->GetBinContent(i)<=max_FilterBaseline )
        {
          for (int j = i; j < i+10; j++)
          {
            if ( filter_m->GetBinContent(j)>max_FilterBaseline ) is_peakBehind=true;
            // cout<< "check the peak behind!"<<endl;
          }
          if (is_peakBehind==false)
          {
            filter_m->SetBinContent( i, 0 );
            is_peakBehind=false;
          }
        }
        is_peakBehind=false; 
      }
  }
  else
  {
    bool below_threshold=false;
    for (int i = 4; i < nDimension/2 ; i++)
      {
        if ( filter_m->GetBinContent(i)<max_FilterBaseline+0.5 ) below_threshold=true;
        // cout<< i << "  filter_value: " << filter_m->GetBinContent(i)<<"    baseline:    "<<max_FilterBaseline <<endl;
        if (below_threshold==true)
        {
          filter_m->SetBinContent( i, 0 );
        }
      }
  }

  bool found_firstExtremum =false;
  double min_firstExtremum = 100000;
  int bin_minFirstExtremum= 0 ;
  bool found_firstDescend =false;
  int bin_firstDescend = 0;
  double value_firstDescend = 0;
  TF1* f_toSmearPeak=NULL;
  if ( retain_FilterPeakOnBrae==false )
  {
    for (int i=3; i<nDimension/2;i++)
    {
      if ( found_firstExtremum==false && filter_m->GetBinContent(i)<= min_firstExtremum )
      {
        min_firstExtremum = filter_m->GetBinContent(i);
        bin_minFirstExtremum = i;
        // cout<< "filter value: ( "<<i<<" , "<<filter_m->GetBinContent(i)<<" ) "<<endl;
        if ( filter_m->GetBinContent(i+1)>=filter_m->GetBinContent(i) && filter_m->GetBinContent(i+2)>filter_m->GetBinContent(i+1) )
        {
          found_firstExtremum=true;
        }
      }
      else if( found_firstExtremum == true )
      {
        break;
      }
      if ( found_firstDescend==false )
      {
      // cout<< "filter value: ( "<<filter_m->GetBinContent(i+2)-filter_m->GetBinContent(i)<<" ) "<<endl;
        if ( abs(filter_m->GetBinContent(i+2)-filter_m->GetBinContent(i)) > 0.03 && 
        filter_m->GetBinContent(i+1)<filter_m->GetBinContent(i) && filter_m->GetBinContent(i+2)<filter_m->GetBinContent(i+1) )
        {
          // if ( filter_m->GetBinContent(i+2)-filter_m->GetBinContent(i) < 0.03 )
          {
            found_firstDescend=true;
            bin_firstDescend = i ;
            value_firstDescend = filter_m->GetBinContent(i);
          }
        }
      }
    }
      cout<<"the first minimum = ("<<bin_minFirstExtremum<< " , "<<min_firstExtremum<<" )"<<endl;
      cout<<"the first descend = ("<<bin_firstDescend<< " , "<<value_firstDescend<<" )"<<endl;

      TF1* fun1=new TF1("fun1","pol2",bin_firstDescend,bin_minFirstExtremum);
	    filter_m->Fit("fun1","R");
	    double p0 =fun1->GetParameter(0);
	    double p1 =fun1->GetParameter(1);
	    double p2 =fun1->GetParameter(2);
      cout<<"p0="<<p0<<"  p1="<<p1<<endl;
      f_toSmearPeak= new TF1("f_toSmearPeak","pol2",bin_firstDescend,bin_firstDescend+100);
      f_toSmearPeak->SetParameters(p0,p1,p2);
      for (int k = bin_minFirstExtremum ; k < nDimension/2; k++)
      {
        if ( filter_m->GetBinContent(k)>0 )
        {
          filter_m->SetBinContent( k, f_toSmearPeak->Eval(k) );
        }
        else
        {
          break;
        }
      }
      
  }

  
  // TH1D* h_pol2_SmearPeak =(TH1D*) f_toSmearPeak-> GetHistogram ();

  filter_m->SetBinContent(0 , 1);
  filter_m->SetBinContent(1 , 1);
  
  if ( debug == true )
  {
      TCanvas *c3=new TCanvas("c_filter","c_filter",800,600);	
			filter_m ->DrawCopy(); 
      f_toSmearPeak->DrawCopy("same");
      // h_pol2_SmearPeak->DrawCopy("SameRed");
  }
  
		TFile* g=new TFile(newname+"_filter.root","recreate");
    cout << "Saving filter into "+newname+"_filter.root"<<endl;
		g->cd();
		filter_m->Write();
    g->Close();
    spehf->Close();



  // TH1D* backh = (TH1D*)meanh->Clone();

  // for (int i = 0; i < 600; i++) {
  //   meanh->AddBinContent(i + 1, -200 * 200);
  // }
  // meanh->Divide(backh);
  // // meanh->Draw();
  // // meann->Draw("same");

  // // smooth
  // TGraph* tfh = new TGraph();
  // for (int i = 1; i < 600; i++) {
  //   tfh->SetPoint(i - 1, meanh->GetBinCenter(i + 1), meanh->GetBinContent(i + 1));
  // }
  // TGraphSmooth* gsh = new TGraphSmooth("normal");
  // TGraph* grouth = gsh->SmoothSuper(tfh, "", 3);
  // // TGraph* grouth = gsh->SmoothLowess(tfh, "", 0.2);

  // TH1D* fh0 = new TH1D("fh0", "fh0", 1250, 0, 1250);
  // double xx, yy;
  // fh0->SetBinContent(1, 0);
  // int ih = 0;
  // for (int i = 1; i < 1250; i++) {
  //   grouth->GetPoint(i - 1, xx, yy);
  //   if (yy < 0) {
  //     ih = i;
  //     break;
  //   }
  //   fh0->SetBinContent(i + 1, yy);
  // }
  // for (int i = 0; i < 1250; i++) {
  //   fh0->SetBinContent(i + 1, (fh0->GetBinContent(i + 1)));
  // }

  // TCanvas* ch = new TCanvas("hc", "hc", 800, 600);
  // TList* lsh = new TList();
  // TString cname[2] = {"Av_f", "Filter"};
  // lsh->Add(meanh);
  // lsh->Add(fh0);
  // compare(ch->cd(), lsh, 2, cname, 0);

  // TFile* savef = new TFile(dir+"filter3_m.root", "recreate");
  // savef->cd();
  // rawh2D->Write();
  // meanh->Write();
  // fh0->Write();
  // savef->Close();
}
