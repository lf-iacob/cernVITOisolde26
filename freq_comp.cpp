// Frequency analysis: comparison between different configurations

#include <iostream>
#include <filesystem>
#include <vector>
using namespace std;
namespace fs = filesystem;

void freq_comp(const vector<string> &files, TString output = "comparison/output.root") {

  // ---------- ROOT files Info
  size_t nFiles = files.size();
  if (nFiles == 0) cerr << "ERROR -- No input files provided!" << endl;
  vector<string> s(nFiles), tl(nFiles), tw(nFiles), rads(nFiles), g(nFiles), filenames(nFiles);
  cout << endl << "---------- FILE INFO ----------" << endl;
  for (size_t i = 0; i < nFiles; ++i) {
    filenames[i] = fs::path(files[i]).filename().string();
    stringstream ss_name(filenames[i]);
    getline(ss_name, s[i], '_');
    getline(ss_name, tl[i], '_');
    getline(ss_name, tw[i], '_');
    getline(ss_name, g[i], '_');
    getline(ss_name, rads[i], '.');
  }
  for (size_t i = 0; i < nFiles; ++i) {
    cout<<"File " << i + 1 << ":"<<endl
	<<"  - Radioactive source: "<<rads[i]<<endl
	<<"  - Scintillator config: "<<s[i]<<endl
	<<"  - Trigger level (mV): "<<tl[i]<<endl
	<<"  - Gate time (s): "<<g[i]<<endl;
  }
  cout<<endl;
  vector<Color_t> colours={kPink-8, kViolet-3, kAzure+2, kTeal+3, kOrange-3, kSpring-5};

  // ---------- new ROOT file
  if (output=="comparison/output.root") {
    string s_comp="", tl_comp="", rad_comp="", g_comp="";
    for (size_t i=0; i<nFiles; ++i) {
      s_comp+=s[i];
      tl_comp+=tl[i];
      rad_comp+=rads[i];
      g_comp+=g[i];
    }
    output = "comparison/"+s_comp+"_"+tl_comp+"_"+g_comp+"_"+rad_comp+".root";
  }
  TFile *froot = new TFile(output, "RECREATE");
    
  // ---------- Frequency HIST comparison
  TCanvas *c = new TCanvas();
  bool firstDrawn = false;
  vector<TFile*> fl(nFiles);
  vector<TH1F*> h(nFiles);
  for (size_t i = 0; i < nFiles; ++i) {
    fl[i] = TFile::Open(files[i].c_str(), "READ");
    h[i] = (TH1F*)fl[i]->Get("fhist");
    Color_t col=(i<colours.size())?colours[i]:(kBlack + i);
    h[i]->SetLineColor(col);
    h[i]->SetLineWidth(3);
    h[i]->SetFillStyle(3004);
    h[i]->SetFillColor(col);
    if (!firstDrawn) {
      h[i]->Draw("HIST F");
      firstDrawn = true;
    } else h[i]->Draw("HIST F SAME");
  }

  TLegend *leg = new TLegend(0.25, 0.55, 0.88, 0.88);
  leg->SetTextSize(0.03);
  for (size_t i = 0; i < nFiles; ++i) {
    double mean = h[i]->GetMean();
    double mean_err = h[i]->GetMeanError();
    leg->AddEntry(h[i], Form("%s (%s) - tl = %s mV (tw = %s min, g = %s ms)",
			     rads[i].c_str(), s[i].c_str(), tl[i].c_str(), tw[i].c_str(), g[i].c_str()));
    leg->AddEntry(h[i], Form("f_{%s(%s),%s} = %.2f #pm %.2f Hz",
			     rads[i].c_str(), s[i].c_str(), tl[i].c_str(), mean, mean_err), "");
  }
  leg->Draw();
  c->Update();

  cout << "Comparison shown in the plot." << endl << endl;

  froot->cd();
  c->Write();
  froot->Write();
}

