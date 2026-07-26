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

  // ---------- Charge Histogram ----------
  TCanvas *cc = new TCanvas;
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Entries",200,0.02,0.5);
  hc->SetLineColor(kRed-3); 
  hc->SetLineWidth(3);
  hc->SetFillStyle(3004);
  hc->SetFillColor(kRed-3);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }
  hc->Draw();
  
  double c_mean=hc->GetMean(), c_err=hc->GetMeanError(), c_std=hc->GetStdDev();

  TLegend *leg_c = new TLegend(0.45, 0.7, 0.88, 0.88);
  leg_c->SetTextSize(0.03);
  leg_c->AddEntry(hc, Form("%s - Trigger Level = %s mV", rads.c_str(), tl.c_str()));
  leg_c->AddEntry(hc, Form("#splitline{Charge_{%s,%s} = %.4f #pm %.4f Hz}{(#sigma = %.2f Hz)}",
			   rads.c_str(), tl.c_str(), c_mean, c_err, c_std), "");
  leg_c->Draw();
  cc->Update();
  
  // ---------- Amplitude Histogram ----------
  TCanvas *ca = new TCanvas;
  TH1F *hamp = new TH1F("hamp", "Amplitude Histogram;Amplitude (ADC);Entries",200,0.05,0.6);
  hamp->SetLineColor(kAzure+2); 
  hamp->SetLineWidth(3);
  hamp->SetFillStyle(3004);
  hamp->SetFillColor(kAzure+2);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hamp->Fill(amp);
  }
  hamp->Draw();
  
  double amp_mean=hamp->GetMean(), amp_err=hamp->GetMeanError(), amp_std=hamp->GetStdDev();
  
  TLegend *leg_amp = new TLegend(0.45, 0.7, 0.88, 0.88);
  leg_amp->SetTextSize(0.03);
  leg_amp->AddEntry(hamp, Form("%s - Trigger Level = %s mV", rads.c_str(), tl.c_str()));
  leg_amp->AddEntry(hamp, Form("#splitline{Amplitude_{%s,%s} = %.4f #pm %.4f Hz}{(#sigma = %.2f Hz)}",
			       rads.c_str(), tl.c_str(), amp_mean, amp_err, amp_std), "");
  leg_amp->Draw();
  ca->Update();
  
  

  froot->Write();
}
