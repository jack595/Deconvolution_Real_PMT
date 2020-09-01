#include "/afs/ihep.ac.cn/users/z/zhangxt/include/myincludes.h"
#include "pars_waves.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/type_transform.hh"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
#include "TH1D.h"
#include <vector>
// void filterWaves()
//num_waves=443680

//输入的name_saveFile应该是name后面加上相应的编号，这样才能让分步计算产生的数据可以不会互相覆盖，最后方便合并
void filterWaves(TString name ,int start_wavesID10000=0, int end_wavesID10000=7, TString name_saveFile="try.root", bool debug=false)
{
	using namespace std;
	pars_waves pars;
	bool running_InStep = false;
	bool move_resultWaves= true;
	int lengthBin_toMove=50;
	int n_bin_getBaseline=pars.n_bin_getBaseline;
	const int nDimension = pars.nDimension;
	// int n_total_waves=443680;
	bool savePDF=true;
	// bool debug=false;
	// bool debug=true;
	int n_comlum_toPDF=3;
	TString dir="";
	int Length=name.Length();
	TString newname=name(55,Length);
	dir.Append(newname);
	TH1D* spere=NULL;
	TH1D* speim=NULL;
	gStyle->SetOptStat(0);

	TString name0=dir;
	TString name1=dir;
	TString name2=dir;
	name0="../"+name0;
	TFile* spef=new TFile(name0.Append("_out.root"),"read"); 

	spere=(TH1D*)spef->Get("SPERE");
	spere->SetDirectory(0);
	speim=(TH1D*)spef->Get("SPEIM");
	speim->SetDirectory(0);
	spef->Close();
	if (debug ==true)
	{
		TCanvas* c_re=new TCanvas("c_re","c_re",800,600);
		spere->DrawCopy();
		TCanvas* c_im=new TCanvas("c_im","c_im",800,600);
		speim->DrawCopy();

	}
	
	cout << "SPE spectra loaded successfully!" << endl;
	name1="../"+name1;
	//  TFile* ffile = new TFile("/junofs/users/zhangxt/20inch/rec/deconvolution/runs/anaAlg/filtercompare/scriptv2/newfilter/filter3_m.root", "read");
	TFile* ffile=new TFile(name1.Append("_filter.root"),"read");
	// TH1D* h_filter0 = filter(50, 350, 30);
	// h_filter0->SetNameTitle("f0", "f0");
	TH1D* h_filter0 = new TH1D("f0", "f0", nDimension, 0, nDimension);
	for (int i = 0; i < nDimension; i++) h_filter0->SetBinContent(i + 1, 1.);

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
	// TH1D* h_filter2 = (TH1D*)ffile->Get("filter");


	// h_filter2->SetNameTitle("f2", "f2");

	if (debug ==true)
	{
		TCanvas* c_filter=new TCanvas("c_filter","c_filter",800,600);
		h_filter1->DrawCopy();

	}

	cout << "Filters Generated Successfully!" << endl;
	name2="../"+name2;
	TFile* f=new TFile(name2.Append("_rearrange.root")); 
	//TFile* f = new TFile("waves.root", "read");
	TH1D* w_tmp = 0;
	int id = 0;
	// TString name;

	TTree* tr = (TTree*)f->Get("waves");
	int entry=tr->GetEntries();
	tr->SetBranchAddress("waves", &w_tmp);
	// tr->GetEntry(2);
	/* TCanvas* can=new TCanvas("c1","c1",800,600);
	   can->cd();
	   w_tmp->Draw();
	   */
	//  tr->SetBranchAddress("pmtid", &id);
	cout << "Total Events: " << tr->GetEntries() << endl;
	//return 0;
	// TFile* sf = new TFile("waves_trans.root", "recreate");
	// TFile* sf=new TFile(dir.Append("_trans.root"),"recreate");

	TFile* sf=new TFile(name_saveFile,"recreate");
	// TFile* sf = new TFile(savename, "recreate");
	
	sf->cd();
	TH1D* rf = 0;
	TH1D* ff = 0;
	TH1D* ft = 0;
	TH1D* df = 0;
	TH1D* w = 0;
	int waveID=0;
	TH1D* spe2raw = 0;
	double arr[nDimension];
	TTree* str = new TTree("waves_trans", "waves_trans");
	str->Branch("rawfmag", "TH1D", &rf);
	str->Branch("rawh", "TH1D", &w_tmp);
	str->Branch("filteredfmag", "TH1D", &ff);
	str->Branch("filteredt", "TH1D", &ft);
	str->Branch("dividedfmag", "TH1D", &df);
	str->Branch("dividedt", "TH1D", &w);
	str->Branch("spe2raw", "TH1D", &spe2raw);
	str->Branch("waveID", &waveID, "wavesID/I");
	//  str->Branch("pmtid", &id);
	int start =start_wavesID10000*10000;
	int end = end_wavesID10000*10000;

	if ( running_InStep == true )
	{
		if (start>entry)
		{
			cout<< " EROOR:  The start of waveID is geater than n_total_waves!!!must out of index!!" << endl;
			exit(1);
		}

		if (end>entry)
		{
			end=entry;
		}
		if ( end < start)
		{
			cout << "EROOR:  The start of wavesID is larger than the end of wavesID!!You should check the input pars start and end" <<endl;
			exit(1);
		}
	}
	else
	{
		start = 0;
		end = entry;
	}
	

	
	
	// start *= 4;
	// end *= 4;
	cout << "Start: " << start << ", End: " << end - 1 << ", SaveName: " << name_saveFile << endl;
	int k = 0;
	wavedec* wd = new wavedec();
	// for (int i = 0; i < tr->GetEntries(); i++) 
	if (debug==true)	
	{
		end=start+50;
	}
	// vector<vector<TH1D*>> v2D_TH1D_toPDF(end-start+1);
	vector<vector<TH1D*>> v2D_TH1D_toPDF(50);
	TH1D* w_Clone=NULL;
	for (int i = start; i < end; i++) {
		tr->GetEntry(i);
		//if (true) {
		// wd->set_input(w_tmp, h_filter0, spere[id], speim[id]);
		// cout<< "Check ERROR Location 1 !!!!!!" <<endl;
		wd->set_input(w_tmp, h_filter1, spere, speim);
		wd->transform();
		rf = wd->get_rawfmag();
		ff = wd->get_filteredfmag();
		ft = wd->get_filteredt();//滤波以后的波形
		df = wd->get_dividedfmag();
		w = wd->get_dividedt();//这是反卷积以后的波形

		if ( move_resultWaves==true )
		{
			// TH1D* w_Clone =(TH1D*) w->Clone( (TString) n2str(i)+"w_clone" ); 
			w_Clone =new TH1D("h_clone" +(TString) n2str(i),"h_clone"+ (TString) n2str(i),nDimension,0,nDimension); 
			for (int j = nDimension-lengthBin_toMove ; j < nDimension; j++)
			{
				w_Clone->SetBinContent(j- (nDimension-lengthBin_toMove ), w->GetBinContent(j));
			}
			for (int j = 0 ; j < nDimension-lengthBin_toMove; j++)
			{
				// cout<< j<< "    " << w->GetBinContent(j)<<endl;
				w_Clone->SetBinContent(j+lengthBin_toMove , w->GetBinContent(j));	
			}
		}
		
		// w->SetNameTitle(w_tmp->GetName(), w_tmp->GetTitle());
		waveID=i;
		str->Fill();
		if (i % 1000 == 0) cout << i << " waveforms finished!" << endl;
		if ( savePDF==true && i-start<20 )
		{
			v2D_TH1D_toPDF[i-start].push_back((TH1D*) w_tmp->Clone(  (TString)n2str(i)+"wd" ));
			v2D_TH1D_toPDF[i-start].push_back((TH1D*) ft->Clone(  (TString)n2str(i)+"ft" ));
			v2D_TH1D_toPDF[i-start].push_back((TH1D*) w->Clone( (TString) n2str(i)+"w" ));
			v2D_TH1D_toPDF[i-start].push_back((TH1D*) w_Clone->Clone( (TString) n2str(i)+"w_Clone" ));
			// v2D_TH1D_toPDF[i-start].push_back((TH1D*) ff->Clone( (TString) n2str(i)+"filterdmag" ));
			// v2D_TH1D_toPDF[i-start].push_back((TH1D*) df->Clone( (TString) n2str(i)+"dividedfmag" ));
		}
	}
	if (savePDF==true)
	{
		plot_into_pdf(v2D_TH1D_toPDF, "../output_pdf/"+newname+"_deconvolution_WavesResult.pdf");
	}
	sf->cd();
	str->Write();
	ffile->Close();
	f->Close();
	sf->Close();
	
	}
