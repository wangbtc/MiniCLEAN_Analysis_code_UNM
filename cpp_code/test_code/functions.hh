#ifndef FUNCTION_h
#define FUNCTION_h

#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>
#include <TH1F.h>
#include <TH2F.h>
using namespace RAT::DS;
using namespace std;

const int tmin = -1000.0;
const int tmax = 16000.0 ;
const int tbins = (tmax - tmin) / 4 ;


void read_waveform(PMT* pmt,double trig,double sampletime,TH1D* hwave);
TH2D* Create2DHist(string h2name,string Xname,string Yname, int Xbins, int Xbin_min, int Xbin_max,int Ybins, int Ybin_min, int Ybin_max);
TH1D* Create1DHist(string h1name,string Xname,string Yname, int Xbins, int Xbin_min, int Xbin_max,int color);
TH1D* CreateSumw2Hist(string h1name,string Xname,string Yname, int Xbins, int Xbin_min, int Xbin_max,int color);
void NormalizedSum(TH1D* hsum);
void pulse_info(PMT* pmt,TH2D* pt_q,double time_min=-1000.0,double time_max=16000.0);

#endif
