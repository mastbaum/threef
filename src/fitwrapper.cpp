#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
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


std::vector<std::pair<double, double> > FitWrapper::GetFrequentistIntervals(std::vector<std::pair<double, double> > param_limits,
                        std::vector<size_t> grid_steps,
                        std::vector<double> initparams,
                        double cl,
                        size_t nfake) {
  assert(param_limits.size() == grid_steps.size());
  assert(param_limits.size() == 2);  // :(

  std::vector<std::pair<double, double> > intervals;

  std::vector<double> delta;
  for (size_t i=0; i<param_limits.size(); i++) {
    delta.push_back((param_limits[i].second - param_limits[i].first) / grid_steps[i]);
    intervals.push_back(std::make_pair(param_limits[i].second,param_limits[i].first)); 
  }

  TNtuple* lspace_mc = MCMCMapLikelihood(initparams);
  std::vector<double> bestfit = SAnnealFit(initparams);
  double ml = bestfit[initparams.size()];
  std::cout << "--- SA Fit results ---" << std::endl;
  std::cout << "ML = " << ml << std::endl;
  for (size_t i=0; i<fParamNames.size(); i++) {
    std::cout << fParamNames[i] << ": " << bestfit[i] << " "
      << "(expectation " << initparams[i] << ")"
      << std::endl;
  }

  TCanvas c;
  lspace_mc->Draw("b:a>>hlspace(40, 25, 75, 40, 50, 125)", "", "col z");
  TH2D* hlspace = dynamic_cast<TH2D*>(gDirectory->FindObject("hlspace"));
  c.SaveAs("lspace.pdf");
  delete lspace_mc;

  // Grid scan through parameter values, finding Rc for each
  std::cout << "--- Grid scan ---" << std::endl;

  TH2F* hcont = \
    new TH2F("hcont", "Contour", grid_steps[0], param_limits[0].first, param_limits[0].second,
                                 grid_steps[1], param_limits[1].first, param_limits[1].second);

  for (size_t i=0; i<grid_steps[0]; i++) {
    double a = param_limits[0].first + i * delta[0];
    for (size_t j=0; j<grid_steps[1]; j++) {
      double b = param_limits[1].first + j * delta[1];
      std::vector<double> norms_here;
      norms_here.push_back(a);
      norms_here.push_back(b);

      // Ld = L(Ns, Ti | data)
      double ld = GetNLL(norms_here);
      double rd = ld - ml;

      std::vector<double> rs;
      for (size_t k=0; k<nfake; k++) {
        GenerateData(norms_here);

        // Lx = L(Ns, Ti | fake data)
        double lx = GetNLL(norms_here);

        // Lf = L(hat(Ns), hat(Ti) | fake data)
        std::vector<double> bff = SAnnealFit(norms_here);
        double lf = bff[norms_here.size()];


        double rf = lf - lx;
        rs.push_back(rf);
      }

      // Find Rc s.t. P(R < Rc) = 0.9
      std::sort(rs.begin(), rs.end());
      double rc = rs[(int)(rs.size() * cl)];

      std::cout << (rd < rc ? "A" : "R") << " "
                << "(" << a << ", " << b << "), "
                << "Rc = " << rc << ", "
                << "Rd = " << rd << std::endl;

      // If R(Ns, Ti | data) <= Rc, this point is in the contour
      if (rd <= rc) {
        hcont->Fill(a, b, 10000);
        if (intervals[0].first > a)
          intervals[0].first = a;
        if (intervals[0].second < a)
          intervals[0].second = a;

        if (intervals[1].first > a)
          intervals[1].first = a;
        if (intervals[1].second < b)
          intervals[1].second = b;
      }
    }

    TCanvas c1;
    hcont->Draw("box");
    c1.SaveAs("cc.pdf");
  }

  // 4. Extrema in each dimension provide an interval
  std::cout << "--- Frequentist Intervals ---" << std::endl;
  for (size_t i=0;i<intervals.size();i++){
    std::cout << fParamNames[i] << ": [" << intervals[i].first << ", " << intervals[i].second << "] "
      << "at " << 100 * cl << "\% CL" << std::endl;
  }
  delete hcont;
  return intervals;
}

