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
#include <functions.hh>
using namespace RAT::DS;

void read_waveform(PMT* pmt,double trig,double sampletime,TH1D* hwave){
  int tmin = -1000.0;
  int tmax = 16000.0 ;
  int tbins = (tmax - tmin) / 4 ;
  double zero = hwave->FindBin(0.0);
  for (int ibl=0;ibl<pmt->GetBlockCount();ibl++){
   Block* block = pmt->GetBlock(ibl);
   vector<double> samples = block->GetSamples();
     int offset = block->GetOffset();
     offset -= (int) (trig / sampletime);

     for (unsigned isample=0;isample<samples.size();isample++){
       int bin = isample + zero + offset;
       if(bin >= 1 && bin <=tbins){
         double binVal = hwave->GetBinContent(bin);
         hwave->SetBinContent(bin, binVal + samples[isample]);
       }//sample if loop
     }//sample loop
   }//block loop

}
