# cernVITOisolde26
CERN Summer Student Program.
Data analysis for instrumentation tests at ISOLDE facility (VITO group, supervisor: Michael Pešek).

**PICOSCOPE**
-- Study of the beta amplitude and charge spectrum for different radioactive sources (@different trigger levels, @different scintillator configurations).<br>
Measurements taen with Picoscope 5442S with its software as h5 files (10k wfs, 125MS/s).<br>
Use "read_h5_txt.py" to convert the files into txt (specify the name of the input file in the script).<br>
Use "spectrum_singlefile.cpp" to gather the data from txt file into a ROOT file and have its charge and amplitude spectrum.<br>
Use "spectrum_comparison.cpp" to compare various ROOT files by plotting their histograms.<br>
[In case of wf over the dynamic range, filter the h5 file with "corrections_dynrange.py".]

[Codes stored as "test_..." are not used in the analysis, just for specific unfortunate occasions.]
