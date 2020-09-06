#include "TTree.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
#include <sstream>
#include <iostream>
#include <string.h>
#include "TH2D.h"
#include "/afs/ihep.ac.cn/users/l/luoxj/workfs_juno_5G/root_tool/include/plot.hh"
//#include "/home/luoxj/root_tool/include/plot.hh"
//#include "plot.hh"
#include <vector>
#include "pars_waves.h"
#include "check_whether_real_signal.h"
void fillRawWavesIntoTH2D(TString name_File_NoSuffix ,pars_waves pars){
    using namespace std ;
    bool debug = false;
    TString name_FileFullPath =pars.name_RootFilePath+name_File_NoSuffix;
    if ( debug == true )
    {
        name_FileFullPath+="_Debug_divide.root";
    }
    else
    {
        name_FileFullPath+="_divide.root";
    }
    
    
    TFile* f=new TFile( name_FileFullPath ,"read" );
    TTree* t_waves=(TTree*) f->Get("waves");
    TTree* t_BigPeak=(TTree*) f->Get("signalBigPeak");
    TTree* t_SmallPeak=(TTree*) f->Get("signalSmallPeak");
    TTree* t_noise=(TTree*) f->Get("noise");
    TH1D* waveform=NULL;
    TH1D* h_BigPeak = NULL;
    TH1D* h_SmallPeak = NULL;
    TH1D* h_noise = NULL;
    t_waves->SetBranchAddress("waves",&waveform);
    t_BigPeak->SetBranchAddress("signalBigPeak", &h_BigPeak );
    t_SmallPeak->SetBranchAddress("signalSmallPeak", &h_SmallPeak );
    t_noise->SetBranchAddress("noise", &h_noise);

    //Initilization of TH2D
    TH2D* h2D_waves = new TH2D("h2D_waves", "h2D_waves", pars.nDimension, 0, pars.nDimension,
                                            pars.waves_amplitude, -10, pars.waves_amplitude );
    TH2D* h2D_BigPeak =new TH2D("h2D_BigPeak", "h2D_BigPeak", pars.nDimension, 0, pars.nDimension,
                                pars.waves_amplitude, -10, pars.waves_amplitude );
    TH2D* h2D_SmallPeak =new TH2D("h2D_SmallPeak", "h2D_SmallPeak", pars.nDimension, 0, pars.nDimension,
                                70 , -10, 70 );
    TH2D* h2D_noise =new TH2D("h2D_noise", "h2D_noise", pars.nDimension, 0, pars.nDimension,
                                70 , -10, 70 );
    for(int i=0; i<t_waves->GetEntries(); i++ )
    {
        t_waves->GetEntry(i);
        double baseline = get_baseline(waveform, pars.n_bin_getBaseline);
        // if ( debug==true && i<5 )
        // {
        //     TString name_c_waves="c_waves_";
        //     TCanvas* c_waves = new TCanvas(name_c_waves+i, name_c_waves+i, 800, 600);
        //     waveform->DrawCopy();
        // }
        for(int j=0; j<pars.nDimension; j++)
        {
            h2D_waves->Fill(j, baseline-waveform->GetBinContent(j) );
        }
    }
    TCanvas* c_waves_h2D = new TCanvas("h2D_waves", "h2D_waves", 800, 600);
    c_waves_h2D ->SetLogz();
    h2D_waves->DrawCopy("colz");

    for(int i=0; i<t_BigPeak->GetEntries();i++)
    {
        t_BigPeak->GetEntry(i);
        double baseline = get_baseline(h_BigPeak, pars.n_bin_getBaseline);
        if ( debug==true && i<5 )
        {
            TString name_c_waves="c_waves_";
            TCanvas* c_waves = new TCanvas(name_c_waves+i, name_c_waves+i, 800, 600);
            h_BigPeak->DrawCopy();
        }
        for(int j=0; j<pars.nDimension; j++)
        {
            h2D_BigPeak->Fill(j, baseline-h_BigPeak->GetBinContent(j) );
        }
    }
    TCanvas* c_BigPeak_h2D = new TCanvas("h2D_BigPeak", "h2D_BigPeak", 800, 600);
    c_BigPeak_h2D->SetLogz();
    h2D_BigPeak->DrawCopy("colz");

    for(int i=0; i<t_SmallPeak->GetEntries();i++)
    {
        t_SmallPeak->GetEntry(i);
        double baseline = get_baseline(h_SmallPeak, pars.n_bin_getBaseline);
        for(int j=0; j<pars.nDimension; j++)
        {
            h2D_SmallPeak->Fill(j, baseline-h_SmallPeak->GetBinContent(j) );
        }
    }
    TCanvas* c_SmallPeak_h2D = new TCanvas("h2D_SmallPeak", "h2D_SmallPeak", 800, 600);
    c_SmallPeak_h2D->SetLogz();
    h2D_SmallPeak->DrawCopy("colz");

    for(int i=0; i<t_noise->GetEntries();i++)
    {
        t_noise->GetEntry(i);
        double baseline = get_baseline(h_noise, pars.n_bin_getBaseline);
        for(int j=0; j<pars.nDimension; j++)
        {
            h2D_noise->Fill(j, baseline-h_noise->GetBinContent(j) );
        }
    }
    TCanvas* c_noise_h2D = new TCanvas("h2D_noise", "h2D_noise", 800, 600);
    c_noise_h2D->SetLogz();
    h2D_noise->DrawCopy("colz");
    
    system("mkdir -p "+pars.name_PdfDir+"h2D_waves/");
    // plot_into_pdf(h2D_waves, pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_waves.pdf");
    // plot_into_pdf(h2D_BigPeak, pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_BigPeak.pdf");
    // plot_into_pdf(h2D_SmallPeak, pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_SmallPeak.pdf");
    // plot_into_pdf(h2D_noise, pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_noise.pdf");
    c_waves_h2D->SaveAs(pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_waves.png");
    c_BigPeak_h2D->SaveAs(pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_BigPeak.png");
    c_SmallPeak_h2D->SaveAs(pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_SmallPeak.png");
    c_noise_h2D->SaveAs(pars.name_PdfDir+"h2D_waves/"+name_File_NoSuffix+"h2D_noise.png");
    
    TString name_SaveTH2D=pars.name_RootFilePath+name_File_NoSuffix+"_wavesTH2D.root";
    TFile* f_saveTH2D=new TFile( name_SaveTH2D ,"recreate" );
    h2D_BigPeak->Write();
    h2D_noise->Write();
    h2D_SmallPeak->Write();
    h2D_waves->Write();
    f_saveTH2D->Close();
    f->Close();
}
