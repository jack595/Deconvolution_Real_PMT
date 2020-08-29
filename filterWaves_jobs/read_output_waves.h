//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Aug 28 17:01:16 2020 by ROOT version 6.20/02
// from TTree waves_trans/waves_trans
// found on file: waves_tran_44.root
//////////////////////////////////////////////////////////

#ifndef read_output_waves_h
#define read_output_waves_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TH1.h"

class read_output_waves {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   TH1D            *rawfmag;
   TH1D            *rawh;
   TH1D            *filteredfmag;
   TH1D            *filteredt;
   TH1D            *dividedfmag;
   TH1D            *dividedt;
   TH1D            *spe2raw;
   Int_t           waveID;

   // List of branches
   TBranch        *b_rawfmag;   //!
   TBranch        *b_rawh;   //!
   TBranch        *b_filteredfmag;   //!
   TBranch        *b_filteredt;   //!
   TBranch        *b_dividedfmag;   //!
   TBranch        *b_dividedt;   //!
   TBranch        *b_spe2raw;   //!
   TBranch        *b_wavesID;   //!

   read_output_waves(TTree *tree=0);
   virtual ~read_output_waves();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef read_output_waves_cxx
read_output_waves::read_output_waves(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("waves_tran_44.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("waves_tran_44.root");
      }
      f->GetObject("waves_trans",tree);

   }
   Init(tree);
}

read_output_waves::~read_output_waves()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t read_output_waves::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t read_output_waves::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void read_output_waves::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   rawfmag = 0;
   rawh = 0;
   filteredfmag = 0;
   filteredt = 0;
   dividedfmag = 0;
   dividedt = 0;
   spe2raw = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("rawfmag", &rawfmag, &b_rawfmag);
   fChain->SetBranchAddress("rawh", &rawh, &b_rawh);
   fChain->SetBranchAddress("filteredfmag", &filteredfmag, &b_filteredfmag);
   fChain->SetBranchAddress("filteredt", &filteredt, &b_filteredt);
   fChain->SetBranchAddress("dividedfmag", &dividedfmag, &b_dividedfmag);
   fChain->SetBranchAddress("dividedt", &dividedt, &b_dividedt);
   fChain->SetBranchAddress("spe2raw", &spe2raw, &b_spe2raw);
   fChain->SetBranchAddress("waveID", &waveID, &b_wavesID);
   Notify();
}

Bool_t read_output_waves::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void read_output_waves::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t read_output_waves::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef read_output_waves_cxx
