// Frequency analysis: ROOT from csv file

#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

void freq_rootfile(string filecsv, TString output="output.root"){

  // ---------- csv FILE INFO
  /* e.g. filename: s1_150_3_sr90.csv
     s: scintillator configuration
     150: trigger level (threshold)
     3: time window (minutes)
     100: gate time (ms)
     sr90: type of radioactive source
  */
  string filename = fs::path(filecsv).filename().string();
  stringstream ss(filename);
  string s, tl, tw, g, rads;
  getline(ss, s, '_');
  getline(ss, tl, '_');
  getline(ss, tw, '_');
  getline(ss, g, '_');
  getline(ss, rads, '.');
  cout<<endl<<"---------- FILE INFO ----------"<<endl
      <<"Type of radioactive source: "<<rads<<endl
      <<"Scintillators configuration: "<<s<<endl
      <<"Trigger level (mV): "<<tl<<endl
      <<"Time window acquisition (min): "<<tw<<endl
      <<"Gate time (ms): "<<g<<endl;
  int colour;
  if(stoi(tl)==100) colour=kPink-8;
  else if(stoi(tl)==150) colour=kViolet-2;
  else if(stoi(tl)==200) colour=kAzure+2;
  else colour=kTeal+3;

  // ---------- ROOT file creation
  if(output=="output.root") output=fs::path(filecsv).parent_path().string()+"/"+fs::path(filecsv).stem().string()+".root";
  TFile *froot = new TFile(output, "RECREATE");

  double freq;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("FREQ", &freq, "freq/D");
  
  ifstream file;
  file.open(filecsv);
  if (!file.is_open()) cerr<<endl<<"ERROR -- Could not open the file!"<<endl<<endl;
  else {
    string riga;
    while(getline(file,riga)){
      freq = stod(riga);
      dd->Fill();
    }
    cout<<endl<<"--> Tree creation completed!"<<endl<<endl;
  }
  file.close();

  int n=dd->GetEntries();

  // ---------- Frequency HIST
  TCanvas *r = new TCanvas();
  TH1F *fhist = new TH1F("fhist", "Frequency Histogram;Frequency (Hz);Entries",400,-10,160);
  fhist->SetLineColor(colour); 
  fhist->SetLineWidth(3);
  fhist->SetFillStyle(3004);
  fhist->SetFillColor(colour);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    fhist->Fill(freq);
  }
  fhist->Draw();
  
  double f=fhist->GetMean(), f_err=fhist->GetMeanError();
 
  TLegend *leg = new TLegend(0.25, 0.75, 0.88, 0.88);
  leg->SetTextSize(0.03);
  leg->AddEntry(fhist, Form("%s (%s) - tl = %s mV; tw = %s min, gate = %s ms", rads.c_str(), s.c_str(), tl.c_str(), tw.c_str(), g.c_str()));
  leg->AddEntry(fhist, Form("f_{%s(%s),%s} = %.2f #pm %.2f Hz", rads.c_str(), s.c_str(), tl.c_str(), f, f_err), "");
  leg -> Draw();
  r->Update();

  froot->cd();
  leg->Write();
  fhist->Write();
  froot->Write();
}

