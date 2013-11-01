#include <vector>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include <aurore/fitter.h>
#include <aurore/migrad.h>

namespace aurore {

ROOT::Minuit2::FCNBase* Fitter::get_minuit2_fcn(Dataset* data) {
  return new MigradFCN(this, data);
}

Fitter::Result*
Fitter::migrad(const std::vector<double>& initial_params, Dataset* data) {
  ROOT::Minuit2::FCNBase* fcn = new aurore::MigradFCN(this, data);
  std::vector<double> errors(initial_params.size(), 0);  // FIXME

  ROOT::Minuit2::MnUserParameters mn_params_initial(initial_params, errors);
  ROOT::Minuit2::MnMigrad mn_migrad(*fcn, mn_params_initial);
  ROOT::Minuit2::FunctionMinimum min = mn_migrad();  // FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters mn_params = min.UserParameters();

  aurore::Fitter::Result* result = new aurore::Fitter::Result;
  result->ml = min.Fval();
  for (size_t i=0; i<mn_params.Params().size(); i++) {
    double best_fit = mn_params.Params()[i];
    double error = mn_params.Errors()[i];
    aurore::Fitter::Result::Parameter p;
    p.set_best_fit(best_fit);
    p.set_interval(best_fit - error, best_fit + error);
    result->parameters.push_back(p);
  }

  delete fcn;

  return result;
}

}  // namespace aurore

