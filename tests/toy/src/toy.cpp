/**
 * \file toy.cpp
 * 
 * Toy implementation of 3f fit.
 */

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <assert.h>
#include <TRandom.h>
#include <TRandom2.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TNtuple.h>
#include <TError.h>
#include <TDirectory.h>
#include <TH2D.h>
#include <nll.h>
#include <pdfs.h>

#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnMinos.h>



int main(int argc, char* argv[]) {
  gErrorIgnoreLevel = kWarning;
  delete gRandom;
  gRandom = new TRandom2();
  gRandom->SetSeed(0);

  const double cl = 0.9;

  NLL nll;

  // 0. Define PDFs
  std::cout << "Generating PDFs..." << std::endl;
  std::vector<TH1D*> pdfs;
  pdfs.push_back(pdfs::make_gaussian(0, 10, 2.5, 0.25, 50));
  pdfs.push_back(pdfs::make_gaussian(0, 10, 7.5, 0.25, 50));

  nll.SetPDFs(pdfs);

  std::vector<std::string> names;
  names.push_back("a");
  names.push_back("b");
  assert(names.size() == pdfs.size());

  nll.SetParamNames(names);

  // 1. Data: take it or fake it
  std::vector<double> rates;
  rates.push_back(50);
  rates.push_back(75);
  assert(rates.size() == pdfs.size());

  nll.SetExpectations(rates);

  std::vector<double> constraints;
  constraints.push_back(0);
  constraints.push_back(0);
  assert(constraints.size() == pdfs.size());

  nll.SetConstraints(constraints);

  std::cout << "Making fake data..." << std::endl;
  FFF::DataWrapper* data = nll.GenerateData(rates);


  std::vector<std::pair<double, double> > param_limits;
  param_limits.push_back(std::make_pair<double, double>(30,70));
  param_limits.push_back(std::make_pair<double, double>(55,95));

  std::vector<size_t> grid_steps;
  grid_steps.push_back(20);
  grid_steps.push_back(20);

  // Do minuit2 migrad fit and calculate minos errors
  std::cout << "Calculating migrad fit and Minos errors..." << std::endl;
  ROOT::Minuit2::FCNBase* migradfunc = nll.GetMinuit2FCNBase(data);
  std::vector<double> verrors;
  for (size_t i=0;i<rates.size();i++)
    verrors.push_back(0.0); //FIXME
  ROOT::Minuit2::MnUserParameters mnParams(rates,verrors);
  ROOT::Minuit2::MnMigrad migrad(*migradfunc,mnParams);  
  ROOT::Minuit2::FunctionMinimum theMin = migrad(); //FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters migradresult = theMin.UserParameters();
  ROOT::Minuit2::MnMinos minos(*migradfunc,theMin);
  std::vector<std::pair<double, double> > minoserrors;
  minoserrors.push_back(minos(0));
  minoserrors.push_back(minos(1));


  std::cout << "Calculating Simulated Annealing fit..." << std::endl;
  std::vector<double> saresult = nll.SAnnealFit(rates,data,50000,0.1,5);

  std::cout << "Calculating MCMC fit and errors..." << std::endl;
  std::vector<double> mcmcresult = nll.MCMCFit(rates,data,50000,0.1,5);

  std::cout << "Truth: \t\t" << rates[0] << "\t\t\t\t " << rates[1] << std::endl;
  std::cout << "Migrad/minos: \t" << migradresult.Params()[0] << " +" << minoserrors[0].second << " -" << -1*minoserrors[0].first;
  std::cout << "\t " << migradresult.Params()[1] << " +" << minoserrors[1].second << " -" << -1*minoserrors[1].first;
  std::cout << " L=" << theMin.Fval() << std::endl;
  std::cout << "SimAnnealing: \t" << saresult[0] << "\t\t\t\t " << saresult[1] << " L=" << saresult[2] << std::endl;
  std::cout << "MCMC: \t\t" << mcmcresult[0] << "\t\t\t\t " << mcmcresult[1] << " L=" << mcmcresult[2] << std::endl;

  // Make a plot of the data and the best fit
  MyData *mydata = dynamic_cast<MyData*> (data);
  TCanvas ca;
  TH1D hdata("hdata",";Energy;Counts",50,0,10);
  hdata.Sumw2();
  for (size_t i=0;i<mydata->events.size();i++)
    hdata.Fill(mydata->events[i]);
  hdata.Draw();
  TH1D hfit("hfit","",50,0,10);
  for (size_t i=0;i<pdfs.size();i++){
    pdfs[i]->SetLineColor(i+2);
    pdfs[i]->DrawNormalized("same",saresult[i]);
    hfit.Add(pdfs[i],saresult[i]);
  }
  hfit.SetLineColor(kBlack);
  hfit.SetLineStyle(2);
  hfit.Draw("same");
  ca.Update();
  ca.SaveAs("spectra.pdf");

  std::cout << "Mapping out likelihood space..." << std::endl;
  // Map out the likelihood space with an mcmc
  TNtuple *lspace = nll.MCMCMapLikelihood(rates,data,50000,0.1,5);
  TCanvas c;
  lspace->Draw("b:a>>hlspace(40, 25, 75, 40, 50, 125)", "", "col z");
  TH2D* hlspace = dynamic_cast<TH2D*>(gDirectory->FindObject("hlspace"));
  c.SaveAs("lspace.pdf");
  delete lspace;
  std::cout << "Done" << std::endl;


//  nll.GetFrequentistIntervals(param_limits,grid_steps,rates, data);
  
  return 0;
}

