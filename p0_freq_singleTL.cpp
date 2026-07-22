// Frequency analysis: chain for ROOT files (same trigger level)

#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;
int const M=5; //number of files

void freq_singleTL(string file1, string file2, string file3, string file4, string file5,
		   TString output="output.root"){

  // ---------- ROOT FILES INFO
  string filename = fs::path(file1).filename().string();
  stringstream ss(filename);
  string t, s, tl, tw, rads;
  getline(ss, t, '_');
  getline(ss, s, '_');
  getline(ss, tl, '_');
  getline(ss, tw, '_');
  getline(ss, rads, '.');
  cout<<endl<<"---------- FILE INFO ----------"<<endl
      <<"Type of radioactive source: "<<rads<<endl
      <<"Scintillators configuration: "<<s<<endl
      <<"Trigger level (mV): "<<tl<<endl<<endl;
    int colour;
  if(stoi(tl)==100) colour=kPink-8;
  else if(stoi(tl)==150) colour=kViolet-2;
  else colour=kAzure+2;

  // ---------- new ROOT file creation
  if(output=="output.root") output=fs::path(file1).parent_path().string()+"/"+s+"_"+tl+"_"+rads+".root";
  TFile *froot = new TFile(output, "RECREATE");

  double freq;
  TChain* ddchain = new TChain("dd","dd");
  TString files[M] = {file1, file2, file3, file4, file5};
  for(int i=0; i<M; i++){
    if(files[i]=="") continue;
    ddchain->AddFile(files[i]);
  }
  //ddchain->GetListOfFiles()->Print();
  cout<<endl;
  ddchain->SetBranchAddress("FREQ", &freq);

  int n = ddchain->GetEntries();

  // ---------- Frequency HIST (total)
  TCanvas *r = new TCanvas();
  TH1F *fhist_t = new TH1F("fhist_t", "Frequency Histogram;Frequency (Hz);Entries",150,-10,160);
  fhist_t->SetLineColor(colour); 
  fhist_t->SetLineWidth(3);
  fhist_t->SetFillStyle(3004);
  fhist_t

    ->SetFillColor(colour);
  for(int i=0; i<n; i++) {
    ddchain->GetEntry(i);
    fhist_t->Fill(freq);
  }
  fhist_t->Draw();

  double f=fhist_t->GetMean(), f_err=fhist_t->GetMeanError();

  TLegend *leg = new TLegend(0.55, 0.75, 0.88, 0.88);
  leg->SetTextSize(0.03);
  leg->AddEntry(fhist_t, Form("%s (%s) - tl = %s mV", rads.c_str(), s.c_str(), tl.c_str()));
  leg->AddEntry(fhist_t, Form("f_{%s(%s),%s} = %.2f #pm %.2f Hz", rads.c_str(), s.c_str(), tl.c_str(), f, f_err), "");
  leg -> Draw();
  r->Update();
  
  froot->cd();
  leg->Write();
  fhist_t->Write();
  froot->Write();
}
