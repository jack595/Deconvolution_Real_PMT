void average(TString name){
	 TString dir="";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
	TString name1=dir;
	// ostringstream in;
	//  in<<"new"<<number<<"rearrged.root";
	// ostringstream in2;
	//  in2<<"new"<<number<<"check.root";
	//  ostringstream out;
	//  out<<"new"<<number<<"average.root";
	TFile* f=new TFile(dir.Append("_rearrange.root"),"read");
	//TFile* f=new TFile("new2rearrged.root","read");
	TTree* t=(TTree*) f->Get("waves");
	int entries=t->GetEntries();
	int alterentries=entries;
	TH1D* waveform=NULL;
	t->SetBranchAddress("waves",&waveform);
	int sum[1024]={0};
	//f->Close();
	TFile* f2=new TFile(name0.Append("_check.root"),"read");
	int specount=0;
	TH1D* spe=NULL;
	spe=(TH1D*)f2->Get("isSPE");
	for (int i=0;i<entries;i++){
		t->GetEntry(i);
		if (spe->GetBinContent(i+1)==1){
			//cout<<i<<endl;
			specount++;
			for (int j=0;j<1024;j++){
			//	cout<<waveform->GetBinContent(j+1)<<endl;
				sum[j]+=waveform->GetBinContent(j+1);
				
			}
		//cout<<sum[400]<<endl;
			//if (waveform->GetBinContent(400)==0 &&waveform->GetBinContent(500)==0 &&waveform->GetBinContent(600)==0) alterentries--;
		}
	}
	TH1F* averageHist=new TH1F("averageHist","averageHist",1024,0,1024);
	for (int k=0;k<1024;k++){
		averageHist->SetBinContent(k+1,(double)sum[k]/specount);
	}
	TFile* g=new TFile(name1.Append("_average.root"),"recreate");
	g->cd();
	averageHist->Write();
	TCanvas* can1=new TCanvas("c2","c2",800,600);
	can1->cd();
	averageHist->Draw();
	g->Close();
	f->Close();
	f2->Close();
	cout<<"entries:"<<entries<<" spe:"<<specount<<endl;
}
