#include <RAT/DB.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Pulse.hh>
#include <RAT/DetectorConfig.hh>
#include <RAT/DS/Root.hh>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TCut.h>
#include <TCutG.h>
#include "alpha_cut_results.hh"
using namespace RAT::DS;
using namespace std;

TH1D* alpha_cut_results(TH2D* fpq){

    TCutG *cut = new TCutG("cut",6);
    //defined the area of the Cut
    cut->SetVarX("");
    cut->SetVarY("");
    cut->SetTitle("Cut for Alpha events");
    cut->SetFillColor(2);
    cut->SetPoint(0,0.245094,89.3518);
    cut->SetPoint(1,0.525597,112.5);
    cut->SetPoint(2,0.614121,267.13);
    cut->SetPoint(3,0.463737,477.315);
    cut->SetPoint(4,0.142705,93.9815);
    cut->SetPoint(5,0.245094,89.3518);
    //

    TH1D* px = fpq->ProjectionX("px",1,2000,"[cut]");



    return px;

}
