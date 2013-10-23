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
#include <TH1F.h>
#include <TCanvas.h>
#include <TNtuple.h>
#include <TError.h>
#include <TDirectory.h>
#include <TH2F.h>
#include "pdfs.h"
#include "mcmc.h"
#include "likelihood.h"

int main(int argc, char* argv[]) {
  gErrorIgnoreLevel = kWarning;

  // 0. Define PDFs
  std::vector<TH1F*> pdfs;
  pdfs.push_back(pdfs::make_gaussian(1, 3, 2, 0.5));
  pdfs.push_back(pdfs::make_flat(1, 3));

  std::vector<std::string> names;
  names.push_back("a");
  names.push_back("b");
  assert(names.size() == pdfs.size());

  // 1. Data: take it or fake it
  std::vector<float> rates;
  rates.push_back(0);
  rates.push_back(50);
  assert(rates.size() == pdfs.size());

  std::vector<float> constraints;
  constraints.push_back(0);
  constraints.push_back(0);
  assert(constraints.size() == pdfs.size());

  std::vector<float> data;
  for (size_t i=0; i<rates.size(); i++) {
    float rate;
    if (constraints[i] > 0) {
      rate = gRandom->Gaus(rates[i], constraints[i]);
    }
    else {
      rate = rates[i];
    }
    for (int j=0; j<gRandom->Poisson(rate); j++) {
      data.push_back(pdfs[i]->GetRandom());
    }
  }

  // 2. Fit data, floating everything
  TNtuple* lspace = mcmc(pdfs, names, rates, constraints, data, 100000);
  float ml;
  std::vector<float> best_fit = get_best_fit(lspace, ml);

  std::cout << "--- Fit results ---" << std::endl;
  std::cout << "ML = " << ml << std::endl;
  for (size_t i=0; i<names.size(); i++) {
    std::cout << names[i] << ": " << best_fit[i] << " "
              << "(expectation " << rates[i] << ")"
              << std::endl;
  }

  TCanvas c;
  lspace->Draw("a:b>>hlspace", "", "col z");
  c.SaveAs("lspace.pdf");

  // 3. Grid scan through parameter values, finding Rc for each
  std::vector<std::pair<float, float> > param_limits = get_parameter_limits(lspace);

  return 0;
}

