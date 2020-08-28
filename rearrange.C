void rearrange(TString name){
	TString dir="";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
	// ostringstream in;
	//  in<<"new"<<number<<"divide.root";
	//  ostringstream out;
	//  out<<"new"<<number<<"rearrged.root";
	TString name1=dir.Append("_divide.root");
	TString name2=name0.Append("_rearrange.root");
	TFile* f=new TFile(name1,"read");
	//TFile* f=new TFile("new1divide.root","read");
	TTree* t=(TTree*) f->Get("waves");
	TH1D* waveform=NULL;
	TH1D* averageHistotrans=NULL;
	t->SetBranchAddress("waves",&waveform);
	int entries=t->GetEntries();
	//int entries=10;
        TFile* g=new TFile(name2,"recreate");
	TTree* str=new TTree("waves","waves");
	TH1D* averageHistorearrage=NULL;
	str->Branch("waves","TH1D",&averageHistorearrage);
	//TH1D* averageHistotrans=new TH1D("averageHistotrans","averageHistotrans",1024,0,1024);
        //TH1D* averageHistorearrage=new TH1D("averageHistorearrage","averageHistorearrage",1024,0,1024);
        TH1D* startposition= new TH1D("startposition","startposition",1024,0,1024);
	int sum=0;
	int maximum=0;
	int maxpoint=0;
	int compare=0;
	for (int k=0;k<entries;k++){
		t->GetEntry(k);
		/*
		   TCanvas* can1=new TCanvas("c","c",800,600);
		   can1->cd();
		   waveform->Draw();
		   */

		 averageHistotrans=new TH1D(Form("trans%i",k),"averageHistotrans",1024,0,1024);
		/*for (int s=0;s<1024;s++) {
			averageHistotrans->SetBinContent(s+1,0);
		}*/
                //waveform=(TH1F*)f->Get("averageHisto");
		sum=0;
		for (int j=0;j<100;j++){
			sum+=waveform->GetBinContent(j+1);
		}
		sum=sum/100;
		for (int i=0;i<1023;i++){
			averageHistotrans->SetBinContent(i+1,sum-waveform->GetBinContent(i+1));
		}
		averageHistotrans->SetBinContent(1024,sum-waveform->GetBinContent(1023));

		maximum=0;
		maxpoint=0;
		for (int i=0;i<1024;i++){
			if (averageHistotrans->GetBinContent(i+1)>maximum){
				maximum=averageHistotrans->GetBinContent(i+1);
				maxpoint=i+1;
			}
		}
		maximum=maximum/6;
		compare=0;
		for (int i=maxpoint;i>0;i--){
			if (averageHistotrans->GetBinContent(i+1)<maximum && compare==0){
				compare=i+1;
			}
		}
		//if (compare==0) compare=512;
		startposition->Fill(compare);
	//	cout<<"sum="<<sum<<" maxpoint="<<maxpoint<<" compare="<<compare<<" maximum="<<maximum<<endl;
		 averageHistorearrage=new TH1D(Form("arrage%i",k),"averageHistorearrage",1024,0,1024);
		/*for (int s=0;s<1024;s++) {
                        averageHistorearrage->SetBinContent(s+1,0);
                }*/
		if (compare<400 && maximum>0){
			for (int j=400-compare;j<1024;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(compare-400+j+1));
			}
			for (int j=0;j<400-compare;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(1));
			}

		}
		if (compare>=400 && maximum>0){
			for (int j=0;j<1024-compare+400;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(compare-400+j+1));
			}
			for (int j=1024-compare+400;j<1024;j++){
				averageHistorearrage->SetBinContent(j+1,averageHistotrans->GetBinContent(1024));
			}

		}
/*
		        for (int s=0;s<1024;s++) {
                        	averageHistorearrage->SetBinContent(s+1,averageHistorearrage->GetBinContent(s+1));
                	}
*/ 
               if (k==0){
			TCanvas* can1=new TCanvas("c2","c2",800,600);
   		        can1->cd();
  			averageHistorearrage->Draw();
		}
		
		g->cd();
		str->Fill();
		delete averageHistotrans;
	        delete averageHistorearrage;
		if (k%100==0) cout<<k<<" finished!"<<endl;
	}       
	g->cd();
	startposition->Write();
	str->Write();
	g->Close();
	f->Close();
	//str->GetEntry(1);
	/*TH1D* test=NULL;
	str->SetBranchAddress("waves",test);
        str->GetEntry(1);
	TCanvas* can=new TCanvas("c1","c1",800,600);
	  can->cd();
	  test->Draw();
*/	  
}
