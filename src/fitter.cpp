#include <iostream>
#include <vector>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include <aurore/fitter.h>
#include <aurore/likelihood.h>
#include <aurore/migrad.h>
#include <aurore/sampler.h>

namespace aurore {

ROOT::Minuit2::FCNBase* Fitter::get_minuit2_fcn(Dataset* data) {
  return new MigradFCN(this, data);
}

Fitter::BestFit*
Fitter::migrad(const std::vector<double>& initial_params, Dataset* data) {
  ROOT::Minuit2::FCNBase* fcn = new aurore::MigradFCN(this, data);
  std::vector<double> errors(initial_params.size(), 0);  // FIXME

  ROOT::Minuit2::MnUserParameters mn_params_initial(initial_params, errors);
  ROOT::Minuit2::MnMigrad mn_migrad(*fcn, mn_params_initial);
  ROOT::Minuit2::FunctionMinimum min = mn_migrad();  // FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters mn_params = min.UserParameters();

  aurore::Fitter::BestFit* best_fit = new aurore::Fitter::BestFit;
  best_fit->value = min.Fval();
  for (size_t i=0; i<mn_params.Params().size(); i++) {
    best_fit->parameters.push_back(mn_params.Params()[i]);
  }

  delete fcn;

  return best_fit;
}

Fitter::BestFit*
Fitter::markov(const std::vector<double>& initial_params,
               Dataset* data,
               const Sampler& sampler,
               const size_t steps, const float burnin_fraction,
               LikelihoodSpace*& likelihood_space) {
  assert(initial_params.size() == this->param_names.size());

  // Set up the likelihood space sample buffer
  // A vector of (parameter vector, function value) pairs
  std::vector<std::pair<std::vector<double>, double> > samples;

  size_t accepted = 0;

  double global_nll_value = 1e9;
  std::vector<double> global_nll_params;

  std::vector<double> current_params = initial_params;
  double current_nll = this->nll(current_params, data);

  for (size_t istep=0; istep<steps; istep++) {
    std::vector<double> new_params = sampler.proposal_jump(current_params);
    double new_nll = this->nll(new_params, data);

    if (sampler.accept(current_nll, new_nll)) {
      accepted++;
      current_params = new_params;
      current_nll = new_nll;
    }

    std::pair<std::vector<double>, double> step = \
      std::make_pair<std::vector<double>, double>(current_params, current_nll);

    if (current_nll < global_nll_value) {
      global_nll_value = current_nll;
      global_nll_params = current_params;
    }

    samples.push_back(step);
  }

  float accept_fraction = 1.0 * accepted / steps;
  if (accept_fraction < 0.1 || accept_fraction > 0.5) {
    std::cerr << "Fitter::markov: Warning: Questionable MCMC accept ratio "
              << accept_fraction << std::endl;
  }

  Fitter::BestFit* best_fit = new Fitter::BestFit;
  best_fit->value = global_nll_value;
  best_fit->parameters = global_nll_params;

  likelihood_space = new LikelihoodSpace(samples, this->param_names);

  return best_fit;
}

}  // namespace aurore

