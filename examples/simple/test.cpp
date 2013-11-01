#include <iostream>
#include <vector>
#include <string>
#include <TMath.h>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnMinos.h>
#include <aurore/fitter.h>

class MyData : public aurore::Dataset {
  public:
    MyData(double _truth) : truth(_truth) {}
    virtual ~MyData() {}

    double truth;
};


double myNLL(const std::vector<double>& params, aurore::Dataset* data) {
  MyData* mydata = dynamic_cast<MyData*>(data);
  
  if (params[0] < 0 || params[0] > TMath::Power(mydata->truth, 2)) {
    return 1e9;
  }

  return -1.0 * TMath::Log(1 - TMath::Power((params[0] - mydata->truth) / mydata->truth, 2));
}

aurore::Dataset* myMC(const std::vector<double>& params) {
  return new MyData(params[0]);
}

int main(int argc, char* argv[]) {
  std::vector<double> initial_params(1, 5.0);
  std::vector<std::string> param_names(1, "a");

  aurore::FitSimple fit(param_names, &myNLL, &myMC);
  aurore::Dataset* data = fit.generate_data(initial_params);
  std::cout << "The correct answer is 5.0, with a likelihood of 0.0" << std::endl;

  aurore::Fitter::Result* results = fit.migrad(initial_params, data);
  std::cout << "Migrad: L = " << results->ml << std::endl;
  for (size_t i=0; i<results->parameters.size(); i++) {
    std::cout << results->parameters[i].get_best_fit() << ", on ["
              << results->parameters[i].get_interval()->first << ", "
              << results->parameters[i].get_interval()->second << "]"
              << std::endl;
  }

  // You can also do normal minuit2 stuff
  ROOT::Minuit2::FCNBase* migradfunc = fit.get_minuit2_fcn(data);
  std::vector<double> errors(initial_params.size(), 0);
  ROOT::Minuit2::MnUserParameters mn_params(initial_params, errors);
  ROOT::Minuit2::MnMigrad migrad(*migradfunc, mn_params);  
  ROOT::Minuit2::FunctionMinimum minimum = migrad(); //FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters migradresult = minimum.UserParameters();
  ROOT::Minuit2::MnMinos minos(*migradfunc, minimum);
  std::pair<double, double> minos_errors = minos(0);

  std::cout << "Minos: " << migradresult.Params()[0]
            << " -" << -1 * minos_errors.first
            << " +" << minos_errors.second
            << std::endl;

  return 0;
}

