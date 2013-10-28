#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <TFile.h>
#include <tmath.h>
#include <TRandom.h>
#include <TNtuple.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>


#include <fitwrapper.h>

std::vector<double> FitWrapper::MigradFit(const std::vector<double>& initparams)
{
  std::vector<double> finalParams;
  std::vector<double> verrors;
  for (size_t i=0;i<initparams.size();i++)
    verrors.push_back(0.0); //FIXME
  ROOT::Minuit2::MnUserParameters mnParams(initparams,verrors);
  ROOT::Minuit2::MnMigrad migrad(*this,mnParams);  
  ROOT::Minuit2::FunctionMinimum theMin = migrad(); //FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters results = theMin.UserParameters();
  finalParams = results.Params();
  finalParams.push_back(theMin.Fval());

  return finalParams;
}

std::vector<double> FitWrapper::MCMCFit(const std::vector<double>& initparams,
    size_t steps, float burnin, float jump)
{
  TNtuple* lspace =  MCMCMapLikelihood(initparams, steps, burnin, jump);

  float* args = lspace->GetArgs();
  float ml = 1e9;
  std::vector<double> best_fit(initparams.size());

  for (int i=0; i<lspace->GetEntries(); i++) {
    lspace->GetEntry(i);
    if (args[initparams.size()] < ml) {
      ml = args[initparams.size()];
      for (size_t j=0; j<initparams.size(); j++) {
        best_fit[j] = args[j];
      }
    }
  }
  best_fit.push_back(ml);
  return best_fit;
}

TNtuple* FitWrapper::MCMCMapLikelihood(const std::vector<double>& initparams,
    size_t steps, float burnin, float jump)
{
  unsigned long accepted = 0;
  unsigned long total = 0;

  std::vector<double> params = initparams;
  std::vector<double> newParams(initparams.size());
  std::vector<double> verrors;
  for (size_t i=0;i<initparams.size();i++)
    verrors.push_back(0.0); //FIXME

  double lcurrent = GetNLL(params);

  std::string varlist;
  if (fParamNames.size() > 0){
    for (size_t i=0;i<initparams.size();i++)
      varlist += fParamNames[i] + ":";
  }else{
    for (int i=0;i<(int)initparams.size();i++){
      std::stringstream ss;
      ss << "param" << i << ":";
      varlist += ss.str();
    }
  }
  varlist += "likelihood";
  TNtuple* lspace = new TNtuple("lspace","Likelihood space", varlist.c_str());

  for (size_t istep=0;istep<steps;istep++){
    // Jump
    for (size_t i=0;i<initparams.size();i++){
      if (verrors[i] >= 0)
        newParams[i] = gRandom->Gaus(params[i],jump);
      else
        newParams[i] = params[i];
    }

    double lnew = GetNLL(newParams);

    bool accept = false;
    double alpha = 0;
    if (lcurrent > lnew){
      accept = true;
    }else{
      alpha = tmath::Exp(lcurrent-lnew);
      if (gRandom->Uniform() <= alpha)
        accept = true;
    }

    if (accept){
      accepted++;
      params = newParams;
      lcurrent = lnew;
    }

    // burn in
    if (istep >= 1.0*steps*burnin){
      float* v = new float[initparams.size() + 1];
      for (size_t i=0; i<initparams.size(); i++) {
        v[i] = params[i];
      }
      v[initparams.size()] = lcurrent;
      lspace->Fill(v);
      delete v;
    }
    total++;
  }

  lspace->SetDirectory(0);

  // std::cout << "MCMCFit: Accept ratio: " << 1.0*accepted/total << std::endl;
  return lspace;
}

std::vector<double> FitWrapper::SAnnealFit(const std::vector<double>& initparams,
    size_t steps, float burnin, float jump)
{
  unsigned long accepted = 0;
  unsigned long total = 0;

  std::vector<double> params = initparams;
  std::vector<double> newParams(initparams.size());
  std::vector<double> verrors;
  for (size_t i=0;i<initparams.size();i++)
    verrors.push_back(0.0); //FIXME

  double lcurrent = GetNLL(params);

  std::vector<double> bfparams = initparams;
  double ml = lcurrent;

  for (size_t istep=0;istep<steps;istep++){
    // Temperature
    float t = tmath::Power(0.999,1.0*istep);

    // Jump
    for (size_t i=0;i<initparams.size();i++){
      if (verrors[i] >= 0)
        newParams[i] = gRandom->Gaus(params[i],jump);
      else
        newParams[i] = params[i];
    }

    double lnew = GetNLL(newParams);

    bool accept = false;
    double alpha = 0;
    if (lcurrent > lnew){
      accept = true;
    }else{
      alpha = tmath::Exp((lcurrent-lnew)/t);
      if (gRandom->Uniform() <= alpha)
        accept = true;
    }

    if (accept){
      accepted++;
      params = newParams;
      lcurrent = lnew;
    }

    // burn in
    if (istep >= 1.0*steps*burnin){
      float* v = new float[initparams.size() + 1];
      for (size_t i=0; i<initparams.size(); i++) {
        v[i] = params[i];
      }
      v[initparams.size()] = lcurrent;
      delete v;

      if (lcurrent < ml){
        bfparams = params;
        ml = lcurrent;
      }
    }
    total++;
  }
//  std::cout << "SAnnealFit: Accept ratio: " << 1.0*accepted/total << std::endl;
  bfparams.push_back(ml);
  return bfparams;
}



