// Analysis beta decay: ENERGY SPECTRUM

#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;
const int L=129;  //wf time window

void e_spectrum(string file0, TString output="output.root"){
  
  // ---------- (h5->)txt Filename ----------
  /* e.g. s1_100_10k_125_Sr90.txt
     s1: scintillator configuration
     100: trigger level (mV)
     10k: number of waveforms
     125: sampling rate (MS/s)
     Sr90: radioactive source
  */
  string filename = fs::path(file0).filename().string();
  stringstream ss(filename);
  string s, tl, nw, sr, rads;
  getline(ss, s, '_');
  getline(ss, tl, '_');
  getline(ss, nw, '_');
  getline(ss, sr, '_');
  getline(ss, rads, '.');
  cout<<endl<<"---------- FILE INFO ----------"<<endl
      <<"Type of radioactive source: "<<rads<<endl
      <<"Scintillators configuration: "<<s<<endl
      <<"Trigger level (mV): "<<tl<<endl
      <<"Number of waveforms acquired: "<<nw<<endl
      <<"Sampling rate (MS/s): "<<sr<<" -> Ticks (ns): "<<1000/stod(sr)<<endl<<endl;
  int colour=kPink-8;

  if(output=="output.root") output=fs::path(file0).stem().string()+".root";
  TFile *froot = new TFile(output, "RECREATE");

  // ---------- Read txt file -> ROOT File ----------
  double a[L];
  double b, c, amp;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[129]/D");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");
  dd->Branch("AMP", &amp, "amp/D");

  ifstream file;
  file.open(file0);
  while(file.good()){
    b=0.; c=0.; amp=0;
    for(int i=0;i<L;i++){
      file>>a[i]; 
      if(i>=50 && i<=129) b+=double(a[i])/(129-50);
      if((a[i]-b)>amp) amp = a[i]-b;
      else if(i>=12 && i<=40) c+=(a[i]-b)*(1000/stod(sr))/50.; //LSB? Dynamic range of ADC? For volts conversion from ADC
    }
    dd->Fill();
  }
  file.close();
 
  int n = dd->GetEntries();

  //---- Charge THistogram
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Entries",200,0.02,0.5);
  hc->SetLineColor(kRed-3); 
  hc->SetLineWidth(3);
  hc->SetFillStyle(3004);
  hc->SetFillColor(kRed-3);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }
  
  //---- Amplitude THistogram
  TH1F *hamp = new TH1F("hamp", "Amplitude Histogram;Amplitude (ADC);Entries",200,0.05,0.6);
  hamp->SetLineColor(kAzure+2); 
  hamp->SetLineWidth(3);
  hamp->SetFillStyle(3004);
  hamp->SetFillColor(kAzure+2);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hamp->Fill(amp);
  }
  
  TLegend *leg = new TLegend(0.25, 0.75, 0.88, 0.88);
  //.leg->SetTextSize(0.03);
  leg->AddEntry(fhist, Form("%s (%s) - tl = %s mV; tw = %s min, gate = %s ms", rads.c_str(), s.c_str(), tl.c_str(), tw.c_str(), g.c_str()));
  leg->AddEntry(fhist, Form("f_{%s(%s),%s} = %.2f #pm %.2f Hz", rads.c_str(), s.c_str(), tl.c_str(), f, f_err), "");
  leg -> Draw();
  r->Update();

  froot->Write();
}
