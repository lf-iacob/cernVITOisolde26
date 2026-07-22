// Frequency analysis: comparison between different configurations

#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

void freq_comp(string file1, string file2, string file3, TString output="comparison/output.root"){
  
  // ---------- root FILE INFO
  string filename1 = fs::path(file1).filename().string();
  stringstream ss1(filename1);
  string filename2 = fs::path(file2).filename().string();
  stringstream ss2(filename2);
  string filename3 = fs::path(file3).filename().string();
  stringstream ss3(filename3);
  string s1, s2, s3, tl1, tl2, tl3, rads1, rads2, rads3;
  getline(ss1, s1, '_');
  getline(ss1, tl1, '_');
  getline(ss1, rads1, '.');
  getline(ss2, s2, '_');
  getline(ss2, tl2, '_');
  getline(ss2, rads2, '.');
  getline(ss3, s3, '_');
  getline(ss3, tl3, '_');
  getline(ss3, rads3, '.');
  cout<<endl<<"---------- FILE INFO ----------"<<endl
      <<Form("Type of radioactive sources - 1: %s, 2: %s, 3: %s", rads1.c_str(), rads2.c_str(), rads3.c_str())<<endl
      <<Form("Scintillators configurations - 1: %s, 2: %s, 3: %s", s1.c_str(), s2.c_str(), s3.c_str())<<endl
      <<Form("Trigger levels (mV) - 1: %s, 2: %s, 3: %s", tl1.c_str(), tl2.c_str(), tl3.c_str())<<endl<<endl;
  int colour1=kPink-8, colour2=kViolet-2, colour3=kAzure+2;

  if(output=="comparison/output.root") "comparison/"+s1+s2+s3+"_"+tl1+tl2+tl3+"_"+rads1+rads2+rads3+".root";
  TFile *froot = new TFile(output, "RECREATE");
  
  // ---------- Frequency hist comparison
  TFile *fl1 = TFile::Open(file1.c_str(), "READ");
  TFile *fl2 = TFile::Open(file2.c_str(), "READ");
  TFile *fl3 = TFile::Open(file3.c_str(), "READ");
  if (!fl1 || !fl2 || !fl3) cerr << "ERROR -- Could not open one or more input ROOT files!" << endl;
  TH1F *h1 = (TH1F*)fl1->Get("fhist_t");
  TH1F *h2 = (TH1F*)fl2->Get("fhist_t");
  TH1F *h3 = (TH1F*)fl3->Get("fhist_t");
  h1->SetLineColor(colour1); h1->SetLineWidth(2); h1->SetFillStyle(3004); h1->SetFillColor(colour1);
  h2->SetLineColor(colour2); h2->SetLineWidth(2); h2->SetFillStyle(3005); h2->SetFillColor(colour2);
  h3->SetLineColor(colour3); h3->SetLineWidth(2); h3->SetFillStyle(3006); h3->SetFillColor(colour3);
  TCanvas *c = new TCanvas();
  gStyle->SetOptStat(0);
  h1->Draw("HIST F");
  h2->Draw("HIST F SAME");
  h3->Draw("HIST F SAME");
  double f1=h1->GetMean(), f1_err=h1->GetMeanError();
  double f2=h2->GetMean(), f2_err=h2
    ->GetMeanError();
  double f3=h3->GetMean(), f3_err=h3->GetMeanError();
  TLegend *leg = new TLegend(0.55, 0.62, 0.88, 0.88);
  leg->SetTextSize(0.03);
  leg->AddEntry(h1, Form("%s (%s) - tl = %s mV", rads1.c_str(), s1.c_str(), tl1.c_str()));
  leg->AddEntry(h1, Form("f1_{%s(%s),%s} = %.2f #pm %.2f Hz", rads1.c_str(), s1.c_str(), tl1.c_str(), f1, f1_err), "");
  leg->AddEntry(h2, Form("%s (%s) - tl = %s mV", rads2.c_str(), s2.c_str(), tl2.c_str()));
  leg->AddEntry(h1, Form("f2_{%s(%s),%s} = %.2f #pm %.2f Hz", rads2.c_str(), s2.c_str(), tl2.c_str(), f2, f2_err), "");
  leg->AddEntry(h3, Form("%s (%s) - tl = %s mV", rads3.c_str(), s3.c_str(), tl3.c_str()));
  leg->AddEntry(h1, Form("f3_{%s(%s),%s} = %.2f #pm %.2f Hz", rads3.c_str(), s3.c_str(), tl3.c_str(), f3, f3_err), "");
  leg->Draw();
  c->Update();

  cout<<"Comparison shown in the plot."<<endl<<endl;

  froot->cd();
  c->Write();
  froot->Write();
}
