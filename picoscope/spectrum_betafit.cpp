// Fit attempt (FAILED) of beta spectrum

#include <iostream>
using namespace std;
#include <cmath>
#include <string>

// --------------- THEORETICAL BETA SPECTRUM FUNCTION (FERMI THEORY) ---------------

// Calculates the unnormalized probability density for a kinetic energy Te [keV]
double FermiBetaSpectrum(double Te, double Q_val, int Z, int eCharge) {
    if (Te <= 0.0 || Te >= Q_val) return 0.0;

    double me_c2 = 510.99895; // Electron rest mass in keV
    
    // Kinematics
    double E_tot = Te + me_c2;                               // Total energy
    double p_e   = sqrt(Te * Te + 2.0 * Te * me_c2);         // Electron momentum
    double p_nu  = Q_val - Te;                               // Neutrino momentum (phase space)

    // Approximation of the Fermi Function F(Z, Te) for Coulomb interaction
    // Non-relativistic Primakoff-Rosen approximation: F ~ 2*pi*eta / (1 - exp(-2*pi*eta))
    double alpha = 1.0 / 137.036;
    double beta  = p_e / E_tot;
    double eta   = -1.0 * eCharge * (Z * alpha / beta); // eCharge = -1 (beta-), +1 (beta+)
 
    double F_Z = 1.0;
    if (abs(eta) > 1e-6) {
        double two_pi_eta = 2.0 * TMath::Pi() * eta;
        F_Z = two_pi_eta / (1.0 - exp(-two_pi_eta));
    }
    
    return p_e * E_tot * (p_nu * p_nu) * F_Z;  // Fermi spectrum
}

// --------------- FIT FUNCTION WITH CHARGE MAPPING AND DETECTOR RESOLUTION ---------------
/**
 * Fit Parameters (par):
 * par[0] : Normalization factor (Amplitude)
 * par[1] : Gain g (Charge per keV = pC/keV)
 * par[2] : Pedestal / Offset Q0 (Charge at zero energy)
 * par[3] : Energy resolution parameter k (sigma_Q = k * sqrt(Q_adc - Q0))
 * par[4] : Q-value of the decay in keV
 * par[5] : Atomic number Z of the daughter nucleus
 * par[6] : Charge of the emitted particle (-1 for beta-, +1 for beta+)
 */
double FitSpectrumChargeSelfContained(double *x, double *par) {
    double Q_adc   = x[0];    // Integrated charge from signal
    double Norm    = par[0];
    double g       = par[1];  // Gain
    double Q0      = par[2];  // Pedestal
    double k_res   = par[3];  // Resolution constant
    double Q_val   = par[4];  // Energy endpoint in keV
    int Z          = round(par[5]);
    int eCharge    = round(par[6]);

    // Below pedestal there is no physical signal
    if (Q_adc <= Q0) return 0.0;

    // Convert charge to equivalent kinetic energy
    double E_equiv = (Q_adc - Q0) / g;

    // Charge resolution at given energy
    double sigma_Q = k_res * std::sqrt(Q_adc - Q0);
    if (sigma_Q <= 0) sigma_Q = 1.0;

    // Integration range for Gaussian convolution (+/- 3 sigma around E_equiv)
    double E_min = TMath::Max(0.0, E_equiv - 3.0 * (sigma_Q / g));
    double E_max = TMath::Min(Q_val, E_equiv + 3.0 * (sigma_Q / g));

    if (E_min >= E_max) return 0.0;

    // Numerical integration using Midpoint Rule
    int n_steps = 300;
    double dE = (E_max - E_min) / n_steps;
    double integral_sum = 0.0;

    for (int i = 0; i < n_steps; ++i) {
        double E = E_min + (i + 0.5) * dE;
        
        // Theoretical Fermi probability at energy E
        double f_beta = FermiBetaSpectrum(E, Q_val, Z, eCharge);

        // Gaussian detector response for energy E
	double Q_expected = g*E + Q0;
	double gaussian = TMath::Gaus(Q_adc,Q_expected,sigma_Q,true);

        integral_sum += f_beta * gaussian * dE;
    }

    return Norm * integral_sum;
}


// --------------- MAIN MACRO TO EXECUTE THE FIT ON DATA ---------------
void betafit(const char* fileName, double typedecay=-1.0) {
    
    // Open ROOT file
    TFile *inputFile = TFile::Open(fileName, "READ");
    TH1F *h_charge = (TH1F*)inputFile->Get("hc");
    h_charge->SetDirectory(0);
    inputFile->Close();

    // ---- FIT CONFIGURATION ----
    double Q_min_fit = 0.02;  // Lower threshold cut to ignore noise
    double Q_max_fit = 0.3;  // Upper fit boundary

    
    TF1 *fFit = new TF1("fFit", FitSpectrumChargeSelfContained, Q_min_fit, Q_max_fit, 7);
    fFit->SetParNames("Norm", "Gain_g", "Pedestal_Q0", "Res_k", "Q_value", "Z_daughter", "eCharge");

    // Parameters
    fFit->SetParName(0, "Normalizzazione");
    fFit->SetParName(1, "Guadagno g");
    fFit->SetParName(2, "Piedistallo Q0");
    fFit->SetParName(3, "Risoluzione k");
    fFit->SetParName(4, "Valore Q");
    fFit->SetParName(5, "Z nucleo figlio");
    fFit->SetParName(6, "Carica beta");

    fFit->SetParameter(0, 20000.); 
    //fFit->SetParLimits(0, 0., 1e5);
    fFit->SetParameter(1, 0.00025);
    fFit->SetParLimits(1, 0.00005, 0.001);
    fFit->SetParameter(2, 0.039);
    fFit->SetParLimits(2, 0.03,0.05);
    fFit->SetParameter(3, 0.015);
    fFit->SetParLimits(3, 0.001, 0.1);
    fFit->FixParameter(4, 546.);
    fFit->FixParameter(5, 39.);           
    fFit->FixParameter(6, typedecay);     //Decay type (-1 = Beta-, +1 = Beta+)
    //Sr90: 4.=2280keV(not 546 because secular equilibrium), 5.=39, 6.=-1
    
    // Perform the fit
    TCanvas *c1 = new TCanvas("c1", "Beta Spectrum Fit in Charge", 800, 600);
    h_charge->SetLineColor(kPink+8);
    h_charge->SetLineWidth(2);
    h_charge->Draw("E");
    fFit->SetLineColor(kBlack);
    fFit->SetLineWidth(3);
    h_charge->Fit(fFit, "R");

    // Results
    double val, err; string name;
    cout<<endl<<"------ FIT RESULTS ------"<<endl;
    for (int i = 0; i < 4
	   ; ++i) {
        val = fFit->GetParameter(i);
        err = fFit->GetParError(i);
        name = fFit->GetParName(i);
        cout<<left<<setw(22)<<name<<" = "<<right<<setw(12)<<val<<" +/- "<<left<<setw(12)<<err<<endl;
    }
    cout<<endl;

}
