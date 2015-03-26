#ifndef CREATE_HIST
#define CREATE_HIST

#include <TH2F.h>
#include <TH1F.h>

TH2D* Create2DHist(string h2name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,double Ybins, double Ybin_min, double Ybin_max);
TH1D* Create1DHist(string h1name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,int color);
TH1D* CreateSumw2Hist(string h1name,string Xname,string Yname, double Xbins, double Xbin_min, double Xbin_max,int color);
void NormalizedSum(TH1D* hsum);

#endif
