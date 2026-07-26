// Energy beta spectrum: comparison between different files

#include <iostream>
#include <filesystem>
#include <vector>
using namespace std;
namespace fs = filesystem;

void spectrum_comp(const vector<string> &files, TString output = "comparison/output.root") {

  // ---------- ROOT files Info ----------
  size_t nFiles = files.size();
  if (nFiles == 0) cerr << "ERROR -- No input files provided!" << endl;
  vector<string> s(nFiles), tl(nFiles), nw(nFiles), sr(nFiles), rads(nFiles), filenames(nFiles);
  cout << endl << "---------- FILE INFO ----------" << endl;
  for (size_t i = 0; i < nFiles; ++i) {
    filenames[i] = fs::path(files[i]).filename().string();
    stringstream ss_name(filenames[i]);
    getline(ss_name, s[i], '_');
    getline(ss_name, tl[i], '_');
    getline(ss_name, nw[i], '_');
    getline(ss_name, sr[i], '_');
    getline(ss_name, rads[i], '.');
  }
  for (size_t i = 0; i < nFiles; ++i) {
    cout<<"File " << i + 1 << ":"<<endl
	<<"  - Radioactive source: "<<rads[i]<<endl
	<<"  - Scintillator config: "<<s[i]<<endl
	<<"  - Trigger level (mV): "<<tl[i]<<endl;
  }
  cout<<endl;
  vector<Color_t> colours={kPink-8, kViolet-3, kAzure+2, kTeal+3, kOrange-3, kSpring-5};

  // ---------- new ROOT file ----------
  if (output=="comparison/output.root") {
    string s_comp="", tl_comp="", rad_comp="";
    for (size_t i=0; i<nFiles; ++i) {
      s_comp+=s[i];
      tl_comp+=tl[i];
      rad_comp+=rads[i];
    }
    output = "comparison/"+s_comp+"_"+tl_comp+"_"+rad_comp+".root";
  }
  TFile *froot = new TFile(output, "RECREATE");
    
  // ---------- Charge Hist comparison ----------
  gStyle->SetOptStat(00000);
  
  TCanvas *ccc = new TCanvas();
  bool firstDrawn = false;
  vector<TFile*> fl(nFiles);
  vector<TH1F*> hc(nFiles);
  for (int i=nFiles-1; i>=0; i--) {
    fl[i] = TFile::Open(files[i].c_str(), "READ");
    hc[i] = (TH1F*)fl[i]->Get("hc");
    Color_t col=(i<colours.size())?colours[i]:(kBlack + i);
    hc[i]->SetLineColor(col);
    hc[i]->SetLineWidth(3);
    hc[i]->SetFillStyle(3004);
    hc[i]->SetFillColor(col);
    if (!firstDrawn) {
      hc[i]->Draw("HIST F");
      firstDrawn = true;
    } else hc[i]->Draw("HIST F SAME");
  }

  TLegend *legc = new TLegend(0.6, 0.45, 0.88, 0.88);
  double c_mean, c_err, c_std;
  legc->SetTextSize(0.02);
  for (size_t i=0; i<nFiles; i++) {
    c_mean = hc[i]->GetMean();
    c_err = hc[i]->GetMeanError();
    c_std = hc[i]->GetStdDev();
    legc->AddEntry(hc[i], Form("%s - Trigger Level = %s mV", rads[i].c_str(), tl[i].c_str()));
    legc->AddEntry(hc[i], Form("#splitline{Charge_{%s,%s} = %.4f #pm %.4f Hz}{(#sigma = %.2f Hz)}",
			       rads[i].c_str(), tl[i].c_str(), c_mean, c_err, c_std), "");
  }
  legc->Draw();
  ccc->Update();


  // ---------- Amplitude Hist comparison ----------
  TCanvas *aaa = new TCanvas();
  firstDrawn = false;
  vector<TH1F*> hamp(nFiles);
  for (int i=nFiles-1; i>=0; i--) {
    fl[i] = TFile::Open(files[i].c_str(), "READ");
    hamp[i] = (TH1F*)fl[i]->Get("hamp");
    Color_t col=(i<colours.size())?colours[i]:(kBlack + i);
    hamp[i]->SetLineColor(col);
    hamp[i]->SetLineWidth(3);
    hamp[i]->SetFillStyle(3004);
    hamp[i]->SetFillColor(col);
    if (!firstDrawn) {
      hamp[i]->Draw("HIST F");
      firstDrawn = true;
    } else hamp[i]->Draw("HIST F SAME");
  }

  TLegend *lega = new TLegend(0.6, 0.45, 0.88, 0.88);
  double amp_mean, amp_err, amp_std;
  lega->SetTextSize(0.02);
  for (size_t i=0; i<nFiles; i++) {
    amp_mean = hc[i]->GetMean();
    amp_err = hc[i]->GetMeanError();
    amp_std = hc[i]->GetStdDev();
    lega->AddEntry(hamp[i], Form("%s - Trigger Level = %s mV", rads[i].c_str(), tl[i].c_str()));
    lega->AddEntry(hamp[i], Form("#splitline{Amplitude_{%s,%s} = %.4f #pm %.4f Hz}{(#sigma = %.2f Hz)}",
				 rads[i].c_str(), tl[i].c_str(), amp_mean, amp_err, amp_std), "");
  }
  lega->Draw();
  aaa->Update();

  
  cout << "Comparison shown in the plots." << endl << endl;
  froot->Write();
}
