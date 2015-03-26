#ifndef READ_WAVE
#define READ_WAVE

#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>

void read_waveform(RAT::DS::PMT* pmt,double trig,double sampletime,TH1D* hwave);

#endif
