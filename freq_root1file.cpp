// Frequency measurements: (single) csv -> ROOT file

#include <iostream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;
#include <vector>

void freq_rootfile(string filecsv, TString output="output.root"){

  // ---------- csv FILE INFO
  /* eg filename: t03_s1_150_3_sr90.csv
     t: numbero of the test
     s: scintillator configuration
     150: trigger level (threshold)
     3: time window (minutes)
     sr90: type of radioactive source
  */
  string filename = fs::path(filecsv).filename().string();
  stringstream ss(filename);
  string t, s, tl, tw, rads;
  getline(ss, t, '_');
  getline(ss, s, '_');
  getline(ss, tl, '_');
  getline(ss, tw, '_');
  getline(ss, rads, '.');
  cout<<endl<<"---------- FILE INFO ----------"<<endl
      <<"Test number: "<<t<<endl
      <<"Type of radioactive source: "<<rads<<endl
      <<"Scintillators configuration: "<<s<<endl
      <<"Trigger level: "<<tl<<endl
      <<"Time window acquisition: "<<tw<<endl;

  // ---------- ROOT file creation
  if(output=="output.root") output = fs::path(filecsv).parent_path().string()+"/"+s+"_"+tl+"_"+tw+"_"+rads+".root";
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
  TH1F *fhist = new TH1F("fhist", "Frequency Histogram;Frequency (Hz);Entries",500,-10,150);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    fhist->Fill(freq);
  }
  fhist->Draw();

  froot->Write();
}

