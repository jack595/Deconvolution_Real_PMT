void simplef(TString name){
	 TString dir="";
	// ostringstream in;
	// in<<"new"<<number<<"divide.root";
	//  ostringstream out;
	//  out<<"new"<<number<<"check.root";
	int length=name.Length();
	TString newname=name(55,length);
	dir.Append(newname);
	TString name0=dir;
	dir.Append("_divide.root");
	name0.Append("_check.root");
	TFile* f=new TFile(dir,"read");

	//TFile* f=new TFile("new1divide.root","read");
	TTree* tr=(TTree*)f->Get("waves"); 
	int entry=tr->GetEntries();
	TH1D* isSPE=new TH1D("isSPE","isSPE",entry,0,entry);
	TH1D* waveform=NULL;
	TH1D* chargeHist=new TH1D("chargeHist","chargeHist",200,00,20000);
	double sum=0;
	double rms=0;
	double limit=0;
	int simple=0;
	tr->SetBranchAddress("waves",&waveform);
	for (int i=0;i<entry;i++){
		tr->GetEntry(i);
		sum=0;
		rms=0;
		limit=0;
		simple=0;
		for(int j=0;j<100;j++){
			sum+=(double)waveform->GetBinContent(j+1);		
		}	
		sum=sum/100;
		for (int j=0;j<100;j++){
			rms+=(waveform->GetBinContent(j+1)-sum)*(waveform->GetBinContent(j+1)-sum);	
		}
		rms=sqrt((double)rms/100);
	//	 cout<<sum<<","<<rms<<endl;
		limit=sum+rms*2.5;
		for (int j=0;j<1024;j++){
			if ((double)waveform->GetBinContent(j+1)>limit){
				simple+=waveform->GetBinContent(j+1)-sum;
			} 


		}
		chargeHist->Fill(simple);
		//	cout<<simple<<","<<endl;
	}

	int min=0;
	int binmin=10000000;
	for (int j=0;j<20;j++){
		if (chargeHist->GetBinContent(j+1)<binmin) {
			binmin=chargeHist->GetBinContent(j+1);
			min=j+1;
		}
	}
	min=min*100;
	cout<<"min="<<min<<endl;
	int found=0;
	int min2=0;
	int binmin2=10000000;
	for (int j=20;j<200;j++){
		if (found==0 && chargeHist->GetBinContent(j+1)<binmin) {
			binmin2=chargeHist->GetBinContent(j+1);
			min2=j+1;
			found=1;
		}
	}
	min2=min2*100;
	cout<<"min2="<<min2<<endl;

	TCanvas* can8=new TCanvas("c9","c9",800,600);
	chargeHist->Fit("gaus");
	TF1* fun1=new TF1("fun1","gaus",min,min2);
	chargeHist->Fit("fun1","R");



	//chargeHist->Fit("gaus","Q");
	//TF1* fun1=chargeHist->GetFunction("gaus");
	//cout<<fun1->GetParameter(1)<<" and "<<fun1->GetParameter(2)<<endl;
	double low=fun1->GetParameter(1)-fun1->GetParameter(2);
	double high=fun1->GetParameter(1)+fun1->GetParameter(2);
	if (low<min) low=min;
	if (high<min || low>min2) cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
	if (high>min2) high=min2;
	cout<<"low="<<low<<" high="<<high<<endl;
	delete fun1;
	int countspe=0;
	for (int i=0;i<entry;i++){
		tr->GetEntry(i);
		sum=0;
		rms=0;
		limit=0;
		simple=0;
		for(int j=0;j<100;j++){
			sum+=(double)waveform->GetBinContent(j+1);
		}
		sum=sum/100;
		for (int j=0;j<100;j++){
			rms+=(waveform->GetBinContent(j+1)-sum)*(waveform->GetBinContent(j+1)-sum);
		}
		rms=sqrt((double)rms/100);
		//   cout<<sum<<","<<rms<<endl;
		limit=sum+rms*2.5;
		for (int j=0;j<1024;j++){
			if ((double)waveform->GetBinContent(j+1)>limit){
				simple+=waveform->GetBinContent(j+1)-sum;
			}


		}
		//                                                                                             chargeHist->Fill(simple);

		if (simple>low &&simple<high) {
			isSPE->SetBinContent(i+1,1);
			countspe++;
		}
		//                                                                                                      cout<<simple<<","<<endl;
	}

	//TH1D* 

	TFile* g=new TFile(name0,"recreate");
	//TFile* g=new TFile("SPEimage.root","recreate");
	g->cd();
	cout<<"entry="<<entry<<" spe="<<countspe<<endl;
	isSPE->Write();
	chargeHist->Write();
	TCanvas* can1=new TCanvas("c2","c2",800,600);
	can1->cd();
	chargeHist->Draw();
	g->Close();
	f->Close();
}
