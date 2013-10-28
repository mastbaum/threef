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
#include <TH1F.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TNtuple.h>
#include <TError.h>
#include <TDirectory.h>
#include <TH2F.h>
#include <nll.h>
#include <pdfs.h>
#include <mcmc.h>
#include <anneal.h>
#include <likelihood.h>
#include <generator.h>

int main(int argc, char* argv[]) {
  gErrorIgnoreLevel = kWarning;
  delete gRandom;
  gRandom = new TRandom2();
  gRandom->SetSeed(0);

  const float cl = 0.9;

  // 0. Define PDFs
  std::cout << "Generating PDFs..." << std::endl;
  std::vector<TH1F*> pdfs;
  pdfs.push_back(pdfs::make_gaussian(0, 10, 2.5, 0.25, 50));
  pdfs.push_back(pdfs::make_gaussian(0, 10, 7.5, 0.25, 50));

  std::vector<std::string> names;
  names.push_back("a");
  names.push_back("b");
  assert(names.size() == pdfs.size());

  // 1. Data: take it or fake it
  std::vector<float> rates;
  rates.push_back(50);
  rates.push_back(75);
  assert(rates.size() == pdfs.size());

  std::vector<float> constraints;
  constraints.push_back(0);
  constraints.push_back(0);
  assert(constraints.size() == pdfs.size());

  std::cout << "Making fake data..." << std::endl;
  std::vector<float> data = make_fake_data(pdfs, rates, constraints);

  // 2. Fit data, floating everything
  std::cout << "Initial MCMC..." << std::endl;
  TNtuple* lspace_mc = mcmc(pdfs, names, rates, constraints, data, 500000, 5);
  float ml_mc;
  std::vector<float> best_fit_mc = get_best_fit(lspace_mc, ml_mc);

  std::cout << "--- MCMC Fit results ---" << std::endl;
  std::cout << "ML = " << ml_mc << std::endl;
  std::cout << "N = " << data.size() << std::endl;
  for (size_t i=0; i<names.size(); i++) {
    std::cout << names[i] << ": " << best_fit_mc[i] << " "
              << "(expectation " << rates[i] << ")"
              << std::endl;
  }

  std::cout << "Initial SA..." << std::endl;
  TNtuple* lspace = anneal(pdfs, names, rates, constraints, data, 5000, 5);
  float ml;
  std::vector<float> best_fit = get_best_fit(lspace, ml);

  std::cout << "--- SA Fit results ---" << std::endl;
  std::cout << "ML = " << ml << std::endl;
  std::cout << "N = " << data.size() << std::endl;
  for (size_t i=0; i<names.size(); i++) {
    std::cout << names[i] << ": " << best_fit[i] << " "
              << "(expectation " << rates[i] << ")"
              << std::endl;
  }

  // Make a plot of the best-fit energy spectrum and likelihood space
  TCanvas ca;
  TH1F hdata("hdata", ";Energy;Counts", 50, 0, 10);
  hdata.Sumw2();
  for (size_t i=0; i<data.size(); i++) {
    hdata.Fill(data[i]);
  }
  hdata.Draw();
  TH1F hfit("hfit", "", 50, 0, 10);
  for (size_t i=0; i<pdfs.size(); i++) {
    pdfs[i]->SetLineColor(i + 2);
    pdfs[i]->DrawNormalized("same", best_fit[i]);
    hfit.Add(pdfs[i], best_fit[i]);
  }
  hfit.SetLineColor(kBlack);
  hfit.SetLineStyle(2);
  hfit.Draw("same");
  ca.Update();
  ca.SaveAs("spectra.pdf");

  TCanvas c;
  lspace_mc->Draw("b:a>>hlspace(40, 25, 75, 40, 50, 125)", "", "col z");
  TH2F* hlspace = dynamic_cast<TH2F*>(gDirectory->FindObject("hlspace"));
  c.SaveAs("lspace.pdf");
  delete lspace;

  // 3. Grid scan through parameter values, finding Rc for each
  std::cout << "--- Grid scan ---" << std::endl;

  std::vector<std::pair<float, float> > param_limits;
  param_limits.push_back(std::make_pair<float, float>(hlspace->GetXaxis()->GetXmin(),
                                                      hlspace->GetXaxis()->GetXmax()));
  param_limits.push_back(std::make_pair<float, float>(hlspace->GetYaxis()->GetXmin(),
                                                      hlspace->GetYaxis()->GetXmax()));

  std::vector<size_t> grid_steps;
  grid_steps.push_back(20);
  grid_steps.push_back(20);

  TH2F* hcont = grid_scan_contour(param_limits, grid_steps, pdfs, names, rates,
                                  constraints, data, ml, cl, 250);

  TCanvas c1;
  hlspace->Draw("col z");
  hcont->Draw("box same");
  c1.SaveAs("bettercont.pdf");
  delete hlspace;

  // 4. Extrema in each dimension provide an interval
  std::cout << "--- Frequentist Intervals ---" << std::endl;
  for (int i=0; i<2; i++) {
    int lower_bin = hcont->FindFirstBinAbove(0, i + 1);
    TAxis* ax = (i == 0 ? hcont->GetXaxis() : hcont->GetYaxis());
    float lower_bound = ax->GetBinLowEdge(lower_bin);

    int upper_bin = hcont->FindLastBinAbove(0, i + 1);
    float upper_bound = ax->GetBinLowEdge(upper_bin) +
                        ax->GetBinWidth(upper_bin);

    std::cout << names[i] << ": [" << lower_bound << ", " << upper_bound << "] "
              << "at " << 100 * cl << "\% CL" << std::endl;
  }

  delete hcont;

  return 0;
}

