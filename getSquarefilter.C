#include "pars_waves.h"
void getSquarefilter(TString name){
	pars_waves pars;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
	const int nDimension = pars.nDimension;
	TString dir="";
	int length=name.Length();
	const int filter_threshold = 100;
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
//	ifstream myfile("elec.list");
	TH1F* average=NULL;
	// TH2D* mo2d=new TH2D("mo2d","mo2d",(nDimension/2),0,(nDimension/2),1000,0,500000000);
	TH1D* moaverage=new TH1D("moaverage","moaverage",nDimension/2,0,nDimension/2);
	TH1D* mo=NULL;
	TH1D* filter=new TH1D("filter","filter",nDimension,0,nDimension);
	string temp;
//	TString name;
//	TString name0;
	//int length;
	int count=0;
//	TFile* f=NULL;
//	TFile* g=NULL;
	TCanvas* outc = new TCanvas("outc", "Comparison", 800, 600);
	//  outc->Divide(3, 1);
	//  outc->Print("figout.pdf[");
//	while(getline(myfile,temp)){

		//cout<<temp<<endl;
//		name=temp;
//		length=name.Length();
//		name=name(0,length-5);
//		name0=name;
//		name=name.Append("_out.root");
		TFile* f=new TFile(dir.Append("_out.root"),"read");
		//average=(TH1F*)f->Get("averageHist");
		//average->SetAxisRange((nDimension/2),800,"X");
		mo=(TH1D*)f->Get("SPEMO");
		//for (int j=0;j<(nDimension/2);j++){

		TGraph* mograph=new TGraph;
		for (int i=0;i<(nDimension/2);i++){
			mograph->SetPoint(i,moaverage->GetBinCenter(i+1),mo->GetBinContent(i+1));
		}
		TGraphSmooth* gsh = new TGraphSmooth("normal");
		TGraph* grouth = gsh->SmoothKern(mograph, "", 1);
		mograph->Draw("");
		TCanvas* outc1 = new TCanvas("outc1", "Comparison", 800, 600);
		outc1->cd();
		grouth->Draw("");
        bool below_threshold =false;
		//filter=new TH1F("filter","filter",(nDimension/2),0,(nDimension/2));
		for (int i=0; i<nDimension;i++)
		{
			if ( grouth->GetPointY(i) > filter_threshod && below_threshold==false )
			{
				filter->SetBinContent( i, 1 );
			}
			else
			{
				filter->SetBinContent( i, 0 );
                below_threshold=true;
			}
		}
		TCanvas* outc2 = new TCanvas("outc2", "Comparison", 800, 600);
		outc2->cd();
		filter->Draw("");
		//}
		TFile* g=new TFile(name0.Append("_filter.root"),"recreate");
		g->cd();
		filter->Write();
		//mo2d->Fill(j+1,mo->GetBinContent(j+1));
		//mo->SetAxisRange(0,100,"X");
		//name0=name0(5,length-5);
		//average->SetTitle(name0);
		//mo->SetTitle(name0);
		//average->SetTitleSize(120);
		//outc->cd(count%3+1);
		//average->Draw();
		//mo->Draw();
		//if (count%3==2) {outc->Print("figout.pdf");
		//outc->Clear();
		// outc->Divide(3, 1);
		//}
		//outc->cd(2);
		//mo->Draw();

		//outc->Print("figout.pdf");

		//cout<<name<<endl;
		count++;
//	}

	/*TGraphSmooth* gsh1 = new TGraphSmooth("normal");
	  TGraph* grouth1 = gsh1->SmoothKern(mograph, "", 1);
	//mograph->Draw("");
	TCanvas* outc2 = new TCanvas("outc2", "Comparison", 800, 600);
	outc2->cd();
	grouth1->Draw("");
	*/
	//outc->Print("figout.pdf");
	//outc->Print("figout.pdf]");
		f->Close();
		g->Close();
}


