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
#include "functions.hh"
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
  char rootfilename[20];
  sprintf(rootfilename,"waveform_test_%d.root",run);
  TString dirname = TString("/data2/cryopit_data/");

  RAT::DetectorConfig* dc = RAT::DetectorConfig::GetDetectorConfig("MiniCLEAN", "MiniCLEAN/MiniCLEAN.geo",502);
  double delta_time =0;
  double flag_time =0 ;
  double flag_time2 =0 ;
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
  TH1D* time_h = Create1DHist("Time difference between two consecutive events"," dt (10 ms) "," counts ",2000,-10,1990,1);

  projection_x = Create1DHist("Projection from 2D Hist","Fprompt","counts",100,0,1,1);
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
    sprintf(sub,"Run_%06u/cal/MCL_%06u_%06u.root",run,run,subrun);
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

      // For vacuum data we want NHit trigger
      if (trigger_type != T_type) continue;
      if (!ev->GetPassAllCuts()) continue;
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
      delta_time += timing ;
      if ((fprompt>0.9) && (fprompt<0.98)) {
        condition =0; //Chorenkov light events
        cos_phi_chorenkov->Fill(phi,costheta);
      }

      if ((fprompt>0.37) && (fprompt<0.42) && (charge >100)&& (w<0.98) && (w>0.6)) {
        condition = 1 ; //alpha events
        cos_phi_alpha->Fill(phi,costheta);
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
            pulse_info(pmt,pulseTime_q_41);
          }
          else {
            pulse_info(pmt,pulseTime_q);
          }
          read_waveform(pmt,trig,sampletime,waveform);

        }//else stament



      }//pmt loop
      // ESR Scintillation events
      if (special_event_analysis){
        if (ESR){
          //

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

          }//second pmt loop
        }// if ESR statment
        if (ESR_41){
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

          }//second pmt loop
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

    c->Write();
    projection_x->Write();
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
