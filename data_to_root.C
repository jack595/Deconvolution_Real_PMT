void data_to_root(TString name){
	TString dir="";
	//int entries=443681;
	int length=name.Length();
	TString newname=name(55,length); 
	UShort_t chData[1024];
	Int_t nDimension;
	//  ostringstream in;
	//  in<<"new"<<name<<".root";
	//  ostringstream out;
	//  out<<"new"<<name<<"divide.root";
	dir.Append(newname);
	TFile* f=new TFile(name.Append(".root"),"read");
	TTree* tr=(TTree*) f->Get("fadc_prototype");
	tr->SetBranchAddress("nDimension",&nDimension);
	tr->SetBranchAddress("chData",&chData);
	int entries=tr->GetEntries();
	cout<<entries<<endl;

	// int namechar=name.First("r");
	// TString newname=name(0,namechar);
	dir.Append("_divide.root");
	TFile* g=new TFile(dir,"recreate");
	TTree* str= new TTree("waves","waves");
	TH1D* h_waveform=NULL;
	int pmtID=1;
	str->Branch("waves","TH1D",&h_waveform);
	str->Branch("pmtid",&pmtID);
	for(int i=1;i<entries;i++){
		//for(int i=0;i<entries;i++){
		tr->GetEntry(i);
		//   cout << nDimension << ", " << chData[0] << endl;
		h_waveform = new TH1D(Form("wf%i", i), "FADC waveform", nDimension, 0, nDimension);
		for(int j=0;j<nDimension;j++){
			h_waveform->SetBinContent(j+1, chData[j]*1.0);
		}

		g->cd();
		str->Fill();
		delete h_waveform;
	}
	str->Write();  
	g->Close();
	f->Close();
	/*
	 * TCanvas* can=new TCanvas("c1","c1",800,600);
	 can->cd();
	 chData->Draw();
	 */
	}
