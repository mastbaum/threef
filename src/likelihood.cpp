#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <assert.h>
#include <TDirectory.h>
#include <TEnv.h>
#include <TH1F.h>
#include <TNtuple.h>
#include <aurore/likelihood.h>

namespace aurore {

LikelihoodSpace::ParamSorter::ParamSorter(
      std::string param, std::vector<std::string> param_names) {
  param_index = (std::find(param_names.begin(), param_names.end(), param) -
                 param_names.begin());
}


bool LikelihoodSpace::ParamSorter::operator()(
    const std::pair<std::vector<double>, double>& left,
    const std::pair<std::vector<double>, double>& right) {
  return left.first[param_index] < right.first[param_index];
}


bool LikelihoodSpace::LikelihoodSorter::operator()(
    const std::pair<std::vector<double>, double>& left,
    const std::pair<std::vector<double>, double>& right) {
  return left.second < right.second;
}


LikelihoodSpace::LikelihoodSpace(
      std::vector<std::pair<std::vector<double>, double> >& _samples,
      std::vector<std::string> _param_names)
      : samples(_samples), param_names(_param_names), samples_ntuple(NULL) {
  // Create an Ntuple with the samples; will use some ROOT functions for
  // projections, etc., for now.
  std::string varlist;
  for (size_t i=0; i<this->param_names.size(); i++) {
    varlist += this->param_names[i] + ":";
  }
  varlist += "likelihood";
  samples_ntuple = new TNtuple("lspace", "Likelihood space", varlist.c_str());

  this->best_fit.second = 1e9;

  for (size_t i=0; i<samples.size(); i++) {
    std::vector<double> params = samples[i].first;
    double l = samples[i].second;
    float v[params.size() + 1];
    for (size_t j=0; j<params.size(); j++) {
      v[j] = params[j];
    }
    v[params.size()] = l;
    samples_ntuple->Fill(v);    

    // Extract the best fit while we're at it
    if (l < this->best_fit.second) {
      this->best_fit = samples[i];
    }
  }
}


LikelihoodSpace::~LikelihoodSpace() {
  delete samples_ntuple;
}


std::vector<double> LikelihoodSpace::get_best_fit(double& nll) {
  nll = this->best_fit.second;
  return this->best_fit.first;
}


std::vector<double> LikelihoodSpace::get_best_fit() {
  return this->best_fit.first;
}


TH1F* LikelihoodSpace::get_projection(std::string name, std::string cut) {
  int default_nbins = 100;
  gEnv->GetValue("Hist.Binning.1D.x", default_nbins);
  gEnv->SetValue("Hist.Binning.1D.x", 10000);
  this->samples_ntuple->Draw((name + ">>_hp").c_str(), cut.c_str(), "goff");
  gEnv->SetValue("Hist.Binning.1D.x", default_nbins);
  TH1F* hp = dynamic_cast<TH1F*>(gDirectory->FindObject("_hp"));
  assert(hp);
  TH1F* h = dynamic_cast<TH1F*>(hp->Clone("h"));
  h->SetDirectory(NULL);

  return h;
}

}  // namespace aurore

