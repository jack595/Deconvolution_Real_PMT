void draw(){
ifstream myfile("elec2.list");
TH1F* average=NULL;
TH1D* mo=NULL;
string temp;
TString name;
TString name0;
int length;
int count=0;
TFile* f=NULL;
 TCanvas* outc = new TCanvas("outc", "Comparison", 2400, 600);
  outc->Divide(3, 1);
  outc->Print("figout2.pdf[");
while(getline(myfile,temp)){

//cout<<temp<<endl;
name=temp;
length=name.Length();
name=name(0,length-5);
name0=name;
name=name.Append("_out.root");
f=new TFile(name,"read");
average=(TH1F*)f->Get("averageHist");
average->SetAxisRange(0,512,"X");
mo=(TH1D*)f->Get("SPEMO");
mo->SetAxisRange(0,100,"X");
name0=name0(5,length-5);
average->SetTitle(name0);
mo->SetTitle(name0);
//average->SetTitleSize(120);
outc->cd(count%3+1);
//average->Draw();
mo->Draw();
if (count%3==2) {outc->Print("figout2.pdf");
outc->Clear();
 outc->Divide(3, 1);
}
//outc->cd(2);
//mo->Draw();

//outc->Print("figout.pdf");

//cout<<name<<endl;
count++;
}
outc->Print("figout2.pdf");
outc->Print("figout2.pdf]");
}

