#ifndef MAIN_h
#define MAIN_h

#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <vector>
using namespace RAT::DS;
using namespace std;

const double tmin = -1000.0;
const double tmax = 16000.0 ;
const double tbins = (tmax - tmin) / 4 ;


#endif
