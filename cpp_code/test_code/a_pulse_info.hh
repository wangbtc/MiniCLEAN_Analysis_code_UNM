#ifndef PULSE_INFO
#define PULSE_INFO

#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>
#include <TH2F.h>

void pulse_info(RAT::DS::PMT* pmt,TH2D* pt_q,double time_min,double time_max);

#endif
