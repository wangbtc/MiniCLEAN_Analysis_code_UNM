#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>
#include <TH2F.h>
#include "a_pulse_info.hh"
using namespace RAT::DS;
using namespace std;

void pulse_info(RAT::DS::PMT* pmt,TH2D* pt_q,double time_min=-1000.0,double time_max=16000.0){

  for (unsigned ipulse = 0; ipulse<pmt->GetPulseCount(); ipulse++){
    Pulse* pulse = pmt->GetPulse(ipulse);
    double ptime = pulse->GetTime();
    double charge = pulse->GetCharge();
    if ((ptime>time_min) && (ptime<time_max)){
        pt_q->Fill(ptime,charge);
    }
  }

}
