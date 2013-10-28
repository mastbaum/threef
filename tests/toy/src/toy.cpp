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
  nll.GenerateData(rates);


  std::vector<std::pair<double, double> > param_limits;
  param_limits.push_back(std::make_pair<double, double>(30,70));
  param_limits.push_back(std::make_pair<double, double>(55,95));

  std::vector<size_t> grid_steps;
  grid_steps.push_back(20);
  grid_steps.push_back(20);

  nll.GetFrequentistIntervals(param_limits,grid_steps,rates);
  
  return 0;
}

