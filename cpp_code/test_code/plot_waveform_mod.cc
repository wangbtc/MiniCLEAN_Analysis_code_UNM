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
#include "plot_waveform_mod.hh"
#include "create_hist.hh"
#include "alpha_cut.hh"
#include "alpha_cut_results.hh"
#include "a_pulse_info.hh"
#include "read_waveform.hh"

vector<TH1D*> hwavesum;
TH1D* waveform;
TH1D* projection_x;


/*
usage : ./ana run_type run subrun special_events

run_type: vacuum/LED  --- specified this will get you right trigger data
run: run number(e.g. 555 or 316)  will automatically look for the rootfile in the disk will predefined path
subrun : how many subrun would you like to run
special_event:  if 1 will give you alpha_events and 0 will just store basic information about the run

example :  ./ana vacuum 555 10 0
*/
using namespace std;
using namespace RAT::DS;

int main(int argc, char *arg[]){

  // take care of arguments
  int run = atoi (arg[2]) ;
  int subrun_range = atoi (arg[3]) ;
  int condition ;
  char sub[50];
  int T_type;
  bool special_event_analysis = false;

  string str = arg[1];
  int selec = atoi (arg[4]);
  //cout<<" What is the arg[0] "<<str<<endl;
  if (str == "vacuum") {
    cout<<" We are now processing vacuum data for Run "<<run<<endl;
    T_type = 1;//EVENT Trigger (NHits)
  }
  if (str == "LED"){
    cout<<" We are now processing LED data for Run "<<run<<endl;
    T_type = 5;//CAL 1 Trigger, trigger signal is from FPGA of Kapustinsky' like pulser
  }
  if (selec == 1){
    special_event_analysis = true;
  }
  //////////////////////////////////////////////////////////////////////////////
  char rootfilename[30];
  sprintf(rootfilename,"waveform_test_%d_sub_%d.root",run,subrun_range);
  TString dirname = TString("/Users/wangbtc/Google Drive/cryopitdata/data_root_file/");
  //TString dirname = TString("/data1/miniclean/");

  RAT::DetectorConfig* dc = RAT::DetectorConfig::GetDetectorConfig("MiniCLEAN", "MiniCLEAN/MiniCLEAN.geo",502);
  double delta_time =0;
  double pulse_inj_time = 0 ;
  double event_time = 0;
  double max_q =0 ;
  double max_q_41 = 0 ;
  double max_pmt;
  double max_ptime = 0;
  double max_ptime_41 = 0;
  double time_diff_inj = 0;
  bool pulse_inj_event = false ;
  double flag_time =0 ;
  double flag_time2 =0 ;
  int inj_count = 0 ;
  //Create bunch of histohram to store information
  TH2D* fp_q = Create2DHist("fprompt_charge_total"," fprompt","Charge",100,0,1,2000,0,500);
  TH2D* fp_r = Create2DHist("fprompt_centroid_total"," fprompt","(R/R_{tpb})^{3}",100,0,1,100,0,1);
  TH2D* q_r = Create2DHist("charge_centroid_total","charge","(R/R_{tpb})^{3}",2000,0,500,100,0,1);

  TH2D* fp_q_equal = Create2DHist("fprompt_charge_equal"," fprompt","Charge",100,0,1,2000,0,500);
  TH2D* fp_r_equal = Create2DHist("fprompt_centroid_equal"," fprompt","(R/R_{tpb})^{3}",100,0,1,100,0,1);
  TH2D* q_r_equal = Create2DHist("charge_centroid_equal","charge","(R/R_{tpb})^{3}",2000,0,500,100,0,1);

  TH2D* fp_q_equal_41 = Create2DHist("fprompt_charge_equal_41"," fprompt","Charge",100,0,1,2000,0,500);
  TH2D* fp_r_equal_41 = Create2DHist("fprompt_centroid_equal_41"," fprompt","(R/R_{tpb})^{3}",100,0,1,100,0,1);
  TH2D* q_r_equal_41 = Create2DHist("charge_centroid_equal_41","charge","(R/R_{tpb})^{3}",2000,0,500,100,0,1);

  TH2D* fp_q_normal = Create2DHist("fprompt_charge_normal"," fprompt","Charge",100,0,1,2000,0,500);
  TH2D* fp_r_normal = Create2DHist("fprompt_centroid_normal"," fprompt","(R/R_{tpb})^{3}",100,0,1,100,0,1);
  TH2D* q_r_normal = Create2DHist("charge_centroid_normal","charge","(R/R_{tpb})^{3}",2000,0,500,100,0,1);

  TH2D* cos_phi = Create2DHist("Cos_Phi","#phi","Cos(#Theta)",100,-3.5,3.5,100,-1,1);
  TH2D* cos_phi_chorenkov = Create2DHist("Cos_Phi Chorenkov","#phi","Cos(#Theta)",100,-3.5,3.5,100,-1,1);
  TH2D* cos_phi_alpha = Create2DHist("Cos_Phi Alpha","#phi","Cos(#Theta)",100,-3.5,3.5,100,-1,1);
  TH2D* cos_phi_equal = Create2DHist("Cos_Phi Equal","#phi","Cos(#Theta)",100,-3.5,3.5,100,-1,1);
  TH2D* cos_phi_normal = Create2DHist("Cos_Phi Normal","#phi","Cos(#Theta)",100,-3.5,3.5,100,-1,1);

  TH2D* pulseTime_q = Create2DHist("Pulse_time Q"," Time (ns) ","Charge (PE)",tbins,tmin,tmax,1000,0,200);
  TH2D* pulseTime_q_41 = Create2DHist("Pulse_time Q for PMT 41"," Time (ns) ","Charge (PE)",tbins,tmin,tmax,1000,0,200);
  TH1D* pmt_count = Create1DHist("pmt_count"," PMT Channel "," Nomalized Count ",92,0,92,1);
  TH1D* time_h = Create1DHist("Time difference between two consecutive events"," dt ( us) "," counts ",2000,-10,1990,1);

  TH2D* qmax_ptime_41 = Create2DHist("max_charge_pulse_time_41"," Time","Charge",tbins,tmin,tmax,1000,0,200);
  //TH2D* qmax_ipmt_41 = Create2DHist("fprompt_centroid_equal_41"," fprompt","(R/R_{tpb})^{3}",100,0,1,100,0,1);
  //TH2D* q_r_equal_41 = Create2DHist("charge_centroid_equal_41","charge","(R/R_{tpb})^{3}",2000,0,500,100,0,1);


  TH2D* qmax_ptime = Create2DHist("max_charge_pulse_time"," Time","Charge",tbins,tmin,tmax,1000,0,200);
  TH2D* qmax_ipmt = Create2DHist("qmax_pmt"," pmt","charge",92,0,92,1000,0,200);
  //TH2D* q_r_equal_41 = Create2DHist("charge_centroid_equal_41","charge","(R/R_{tpb})^{3}",2000,0,500,100,0,1);

  TH2D* qmax_charge = Create2DHist("max_charge_charge_event","Chare of event","charge of max pulse",1000,0,200,1000,0,200);
  TH2D* qmax_charge_41 = Create2DHist("max_charge_charge_event_41","Chare of event","charge of max pulse",1000,0,200,1000,0,200);

  TH1D* event_inj_diff = Create1DHist("Time diff between selected events and pulse_inj"," dt (us)","counts",8000,0,2000,1);
  event_inj_diff->SetBit(TH1::kCanRebin);
  //TH1D* trigger_log = Create1DHist("Trigger Pattern","trigger bit","counts",8,0,8,1);
  projection_x = Create1DHist("Projection from 2D Hist","Fprompt","counts",100,0,1,1);
  //TH2D* TriggerPattern = Create2DHist("TriggerPattern","Trigger Bit","counts",8,0,8,10000,0,100000);
  //
  int total_count = 0;
  double zeroBin ;
  //vector<int> pmt_hit(92);
  //TH1F* pmt_counts = new TH1F("pmt_counts","pmt_counts",92,0,92);
  bool first_hit = false ;
  bool second_hit = false ;
  bool third_hit = false ;
  bool initHist = false ;
  bool ESR;
  bool ESR_41;
  double run_start = 0;
  double run_end = 0 ;
  double t = 0 ;
  TTimeStamp utctime;

  // Initializing Histogram for waveform

  if (!initHist){

    if (special_event_analysis){
      ESR = false ;
      ESR_41 = false ;
      string histname[5] = {"Total_Chorenkov","Total_alpha","Equal_rest_of_PMT","Equal_PMT_41","None_ESR"};
      for (int ih=0;ih<5;ih++){
        hwavesum.push_back(CreateSumw2Hist(("hwavesume_" + histname[ih])," Time (ns) ","Normalized event",tbins,tmin,tmax,2));
      }
    }
    else{
      waveform = CreateSumw2Hist(" SumWaveForm"," Time (ns) ","Normalized event",tbins,tmin,tmax,2);
    }
    initHist = true ;
  }
  ////////////////////////////////////////////////////////////////////////////////////////

  // Runing through every subrun specified in the code for each Run
  for (int subrun = 0; subrun < subrun_range ; subrun++){
    //Chain each subrun together
    sprintf(sub,"MCL_%06u_%06u.root",run,subrun);
    //sprintf(sub,"MCL_%06u_%06u.root",run,subrun);
    TString readfilename = dirname + sub ;

    printf(" Input filename for Run %d is %s \n",run,readfilename.Data());
    TChain* tree = new TChain("T","T");
		tree->Add(readfilename);
    //

    RAT::DS::Root* ds = new RAT::DS::Root();
    tree->SetBranchAddress("ds", &ds);
    int ev_count = tree->GetEntries();


    total_count += ev_count;
    pulse_inj_event = false ;
    //event loop
    for (int ie=0 ; ie < ev_count ; ie++){
      if (ie%1000==0){cout<<" Finished "<<ie<<" events ! "<<endl;}
      tree->GetEvent(ie);
      //skip bad event
      if (!ds->ExistEV()) continue;
      if (ds->GetEVCount()!=1) continue;
      //
      RAT::DS::EV* ev = ds->GetEV(0);

      // get some fundamental parameters

      double trig = ev->GetCalibratedTriggerTime();
      double trigger_type = ev->GetTriggerType();
      double charge = ev -> GetQPE();
      double timing = ev->GetDeltaT();
      double fprompt = ev->GetFprompt();


      //use centroid fit

      double Theta = ev->GetCentroid()->GetPosition().Theta();
      double costheta = ev->GetCentroid()->GetPosition().CosTheta();
      double phi = ev->GetCentroid()->GetPosition().Phi();
      double x = ev->GetCentroid()->GetPosition().x();
      double y = ev->GetCentroid()->GetPosition().y();
      double z = ev->GetCentroid()->GetPosition().z();
      double rc = ev->GetCentroid()->GetPosition().Mag();
      double w=pow(rc/435,3.0);//normalized radius against TPB radius

      if (ie==0){
        if ((ie == 0) &&(subrun == 0) ){
          run_start = ev->GetUTC().GetSec();
        }

        utctime = ev->GetUTC();
        cout<<" The oringinal time is "<<utctime.AsString("c")<<endl;
      }
      if ((ie== ev_count - 1) && (subrun == subrun_range-1 )){
        run_end = ev->GetUTC().GetSec() ;
        t = run_end - run_start ;

        utctime = ev->GetUTC();
        cout<<" The oringinal time is "<<utctime.AsString("c")<<endl;
        cout<<" The total runtime is "<<t<<endl;
        cout<<" inj rate for run"<<run<<" is "<<(float)inj_count/t<<endl;
      }

      //for (int it =0;it<8;it++){
      //  TriggerPattern->Fill(it,ev->GetTriggerCount(it));
      //}
      /*
      if (ev->GetTriggerCount(4) > 0){
        cout<<" Before the cut "<<endl;
        cout<<" Trigger count for NHIT: "<<ev->GetTriggerCount(1)<<endl;
        cout<<" Trigger count for PINJ: "<<trigger_type<<endl;
        cout<<" Trigger count for PERI: "<<ev->GetTriggerCount(7)<<endl;
        cout<<" Trigger count for generic: "<<ev->GetTriggerCount()<<endl;
        cout<<" GetTriggerOverlapType "<<ev->GetTriggerOverlapType()<<endl;
      }

      trigger_log->Fill(trigger_type);*/
      delta_time += timing ;
      if (ev->GetTriggerCount(4) > 0){
        pulse_inj_time = 0;
        pulse_inj_time = delta_time ;
        pulse_inj_event = true ;
        inj_count++;
        //cout<<" For INJ event id "<<ev->GetEventID()<<endl;
        //cout<<" This is pulse injection event !"<<trigger_type<<endl;
      }

      // For vacuum data we want NHit trigger

      if (!ev->GetPassAllCuts()) continue;
      /*if (ev->GetTriggerCount(4) > 0){
        cout<<" After the cut "<<endl;
        cout<<" Trigger count for NHIT: "<<ev->GetTriggerCount(1)<<endl;
        cout<<" Trigger count for PINJ: "<<trigger_type<<endl;
        cout<<" Trigger count for PERI: "<<ev->GetTriggerCount(7)<<endl;
        cout<<" Trigger count for generic: "<<ev->GetTriggerCount()<<endl;
        cout<<" GetTriggerOverlapType "<<ev->GetTriggerOverlapType()<<endl;
      }*/
      if (trigger_type != T_type) continue;
      //Fill up 2-D histogram for basic information of the event
      fp_q->Fill(fprompt,charge);
      fp_r->Fill(fprompt,w);
      q_r->Fill(charge,w);
      cos_phi->Fill(phi,costheta);
      //
      //defined the condition for further analysis
      condition = 100;
      ESR = false ;
      ESR_41 = false ;
      max_q = 5 ;
      max_pmt = 100 ;
      max_ptime = -1100.0 ;
      max_q_41 = 5 ;
      max_ptime_41 = -1100.0;

      if ((fprompt>0.9) && (fprompt<0.98)) {
        condition =0; //Chorenkov light events
        cos_phi_chorenkov->Fill(phi,costheta);
      }

      if ((fprompt>0.37) && (fprompt<0.42) && (charge >100)&& (w<0.98) && (w>0.6)) {
        condition = 1 ; //alpha events
        cos_phi_alpha->Fill(phi,costheta);
      }
      if ((fprompt>0) && (fprompt<0.4) && (charge < 5)){
        cout<<" Events  with ev id "<<ev->GetEventID()<<endl;
        cout<<" With Fprompt "<<fprompt<<" Charge "<<"Total Charge"<<charge<<endl;
        cout<<" reconstruct centorid radius "<<w<<endl;
      }
      //calculate the diferential time between two consecutive events
      if ((first_hit) &&(second_hit)){
        double time_diff = flag_time2 - flag_time;
        time_h->Fill(time_diff*1e-6);
        //cout<<" Time Difference between two PMT 41 events is "<<time_diff<<endl;
        first_hit = false ;
        second_hit = true ;
        flag_time = flag_time2;
        //counter++;
      }
      if (third_hit){
        double time_diff = flag_time2 - flag_time ;
        //cout<<"second message: Time Difference between two PMT 41 events is "<<time_diff<<endl;
        time_h->Fill(time_diff*1e-6);
        flag_time = flag_time2;
        third_hit = false ;
      }
      //
      for (int ipmt = 0 ; ipmt<ev->GetPMTCount();ipmt++){
        PMT* pmt = ev->GetPMT(ipmt);
        double pmt_q = pmt->GetQPE();
        int chID = dc->MapPMTIDtoChannelID(pmt->GetID());
        double sampletime = dc->GetChannelSamplingTime(chID);
        if (special_event_analysis){
          if ((pmt_q > 0.8*charge) && (ipmt!=41)) {

            ESR = true ;
            pmt_count->Fill((float) ipmt);

          }
          if ((pmt_q > 0.8*charge) && (ipmt==41)) {

            ESR_41 = true ;
            pmt_count->Fill((float) ipmt);
          }
          switch (condition){
            case 0:
              // collect Chorenkov Events
              read_waveform(pmt,trig,sampletime,hwavesum[0]);
              break;

            //ened of case 1
            case 1:
              read_waveform(pmt,trig,sampletime,hwavesum[1]);
              break;
            default :
              //cout<< " No events pass the selection "<<endl;
              continue;
          }//switch loop
        }//Special event selector
        else{
          if (ipmt==41){
            pulse_info(pmt,pulseTime_q_41,-1000,16000);
          }
          else {
            pulse_info(pmt,pulseTime_q,-1000,16000);
          }
          read_waveform(pmt,trig,sampletime,waveform);

        }//else stament



      }//pmt loop
      // ESR Scintillation events
      if (special_event_analysis){
        if (ESR){
          //
          event_time = delta_time ;
          if (pulse_inj_event){
            time_diff_inj = event_time - pulse_inj_time ;
            event_inj_diff->Fill(time_diff_inj*1e-6);
            //cout<<" Time after last Pulse inj events "<<event_time-pulse_inj_time<<" For event id "<<ev->GetEventID()<<endl;
          }
          if (first_hit){
                flag_time2 = delta_time;
                second_hit = true ;
          }

          if (!second_hit){
            first_hit = true;
            flag_time = delta_time;
          }
          if ((!first_hit) && (second_hit)){
            flag_time2 = delta_time ;
            third_hit = true ;
          }
          //
          fp_q_equal->Fill(fprompt,charge);
          fp_r_equal->Fill(fprompt,w);
          q_r_equal->Fill(charge,w);
          cos_phi_equal->Fill(phi,costheta);
          for (int ipmt = 0 ; ipmt<ev->GetPMTCount();ipmt++){
            PMT* pmt = ev->GetPMT(ipmt);
            double pmt_q = pmt->GetQPE();
            int chID = dc->MapPMTIDtoChannelID(pmt->GetID());
            double sampletime = dc->GetChannelSamplingTime(chID);
            read_waveform(pmt,trig,sampletime,hwavesum[2]);

            for (unsigned ipulse = 0; ipulse<pmt->GetPulseCount(); ipulse++){
              Pulse* pulse = pmt->GetPulse(ipulse);
              double ptime = pulse->GetTime();
              double p_charge = pulse->GetCharge();
              if (p_charge > max_q ){
                max_q = p_charge ;
                max_pmt = ipmt ;
                max_ptime = ptime ;
              }
            }//pulse loop

          }//second pmt loop

          qmax_ptime->Fill(max_ptime,max_q);
          qmax_ipmt->Fill(max_pmt,max_q);
          qmax_charge->Fill(charge,max_q);
        }// if ESR statment
        if (ESR_41){
          event_time = delta_time ;
          if (pulse_inj_event){
            time_diff_inj = event_time - pulse_inj_time ;
            event_inj_diff->Fill(time_diff_inj*1e-6);
            //cout<<" Time after last Pulse inj events "<<event_time-pulse_inj_time<<" For event id "<<ev->GetEventID()<<endl;
          }
          fp_q_equal_41->Fill(fprompt,charge);
          fp_r_equal_41->Fill(fprompt,w);
          q_r_equal_41->Fill(charge,w);
          cos_phi_equal->Fill(phi,costheta);
          for (int ipmt = 0 ; ipmt<ev->GetPMTCount();ipmt++){
            PMT* pmt = ev->GetPMT(ipmt);
            double pmt_q = pmt->GetQPE();
            int chID = dc->MapPMTIDtoChannelID(pmt->GetID());
            double sampletime = dc->GetChannelSamplingTime(chID);
            read_waveform(pmt,trig,sampletime,hwavesum[3]);

            for (unsigned ipulse = 0; ipulse<pmt->GetPulseCount(); ipulse++){
              Pulse* pulse = pmt->GetPulse(ipulse);
              double ptime = pulse->GetTime();
              double p_charge = pulse->GetCharge();
              if (p_charge > max_q_41 ){
                max_q_41 = p_charge ;
                //max_pmt_41 = ipmt ;
                max_ptime_41 = ptime ;
              }
            }//pulse loop


          }//second pmt loop
          qmax_ptime_41->Fill(max_ptime_41,max_q_41);
          qmax_charge_41->Fill(charge,max_q_41);
        }// if ESR_41 statment

        if ((!ESR) && (!ESR_41)){
          fp_q_normal->Fill(fprompt,charge);
          fp_r_normal->Fill(fprompt,w);
          q_r_normal->Fill(charge,w);
          cos_phi_normal->Fill(phi,costheta);
          for (int ipmt = 0 ; ipmt<ev->GetPMTCount();ipmt++){
            PMT* pmt = ev->GetPMT(ipmt);
            double pmt_q = pmt->GetQPE();
            int chID = dc->MapPMTIDtoChannelID(pmt->GetID());

            double sampletime = dc->GetChannelSamplingTime(chID);
            read_waveform(pmt,trig,sampletime,hwavesum[4]);

          }//second pmt loop
        }// if !ESR and !ESR_41 statment
      }// if special_event selector




    }// event loop

  }// subrun loop

  for (int ibin=1;ibin<=pmt_count->GetNbinsX();++ibin){
    pmt_count->SetBinContent(ibin,pmt_count->GetBinContent(ibin)/float(pmt_count->GetEntries()));
  }
  projection_x = alpha_cut_results(fp_q);
  TCanvas *c = alpha_cut(fp_q);


  TString outfilename = dirname + rootfilename;
  TFile *f = new TFile(outfilename,"RECREATE");



  if (special_event_analysis){
    for (int j=0;j<hwavesum.size();j++){
      NormalizedSum(hwavesum[j]);
    }

    fp_q->Write();
    fp_r->Write();
    q_r->Write();
    for (int g =0;g<hwavesum.size();g++){
      hwavesum[g]->Write();
    }
    fp_q_equal->Write();
    fp_r_equal->Write();
    q_r_equal->Write();

    fp_q_equal_41->Write();
    fp_r_equal_41->Write();
    q_r_equal_41->Write();

    fp_q_normal->Write();
    fp_r_normal->Write();
    q_r_normal->Write();

    cos_phi->Write();
    cos_phi_chorenkov->Write();
    cos_phi_alpha->Write();
    cos_phi_equal->Write();
    cos_phi_normal->Write();

    pmt_count->Write();
    time_h->Write();

    //trigger_log->Write();
    c->Write();
    projection_x->Write();
    event_inj_diff->Write();
    //TriggerPattern->Write();

    qmax_ptime->Write();
    qmax_ipmt->Write();

    qmax_ptime_41->Write();

    qmax_charge->Write();
    qmax_charge_41->Write();
  }
  else{
    NormalizedSum(waveform);
    waveform->Write();
    fp_q->Write();
    fp_r->Write();
    q_r->Write();
    cos_phi->Write();
    cos_phi_chorenkov->Write();
    cos_phi_alpha->Write();
    pulseTime_q->Write();
    pulseTime_q_41->Write();
    //cos_phi_equal->Write();
    //cos_phi_normal->Write();
  }

  //



  f->Close();

  return 0;
}
