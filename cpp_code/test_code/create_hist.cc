#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <time.h>
//#include <libRATEvent.so>
#include <TVector3.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1F.h>
#include <TAxis.h>
#include <TKey.h>
#include <TF1.h>
#include <TChain.h>
#include <TProfile.h>
#include <TSystem.h>
#include <TH2F.h>
#include <TMath.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>
#include <create_hist.hh>
using namespace RAT::DS;
using namespace std;

TH2D* Create2DHist(string h2name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,double Ybins, double Ybin_min, double Ybin_max){
  TH2D* h = new TH2D(h2name.c_str(),("#font[132]{" + h2name + "}").c_str(),Xbins,Xbin_min,Xbin_max,Ybins,Ybin_min,Ybin_max);
  h->SetXTitle(("#font[132]{" + Xname + "}").c_str());
  h->SetYTitle(("#font[132]{" + Yname + "}").c_str());
  h->GetXaxis()->SetLabelFont(132);
  h->GetYaxis()->SetLabelFont(132);
  return h;

}

TH1D* Create1DHist(string h1name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,int color){
  TH1D* h = new TH1D(h1name.c_str(),("#font[132]{" + h1name + "}").c_str(),Xbins,Xbin_min,Xbin_max);
  h->SetXTitle(("#font[132]{" + Xname + "}").c_str());
  h->SetYTitle(("#font[132]{" + Yname + "}").c_str());
  h->GetXaxis()->SetLabelFont(132);
  h->GetYaxis()->SetLabelFont(132);
  h->SetMarkerColor(color);
  h->SetLineColor(color);
  return h;

}
TH1D* CreateSumw2Hist(string hname,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,int color){
  TH1D* h = new TH1D(hname.c_str(),("#font[132]{" + hname + "}").c_str(),Xbins,Xbin_min,Xbin_max);
  h->SetXTitle(("#font[132]{" + Xname + "}").c_str());
  h->SetYTitle(("#font[132]{" + Yname + "}").c_str());
  h->GetXaxis()->SetLabelFont(132);
  h->GetYaxis()->SetLabelFont(132);
  h->SetMarkerColor(color);
  h->SetLineColor(color);
  h->Sumw2();
  return h;

}
void NormalizedSum(TH1D* hsum){

    double norm = hsum->Integral(1,hsum->GetNbinsX(),"width");
    hsum->Scale(1 / norm , "width");

}
