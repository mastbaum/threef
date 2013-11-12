#include <iostream>
#include <vector>
#include <string>
#include <TMath.h>
#include <TRandom.h>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnMinos.h>
#include <aurore/fitter.h>
#include <aurore/likelihood.h>
#include <aurore/interval.h>
#include <aurore/sampler.h>
#include <aurore/samplers/metropolis.h>
#include <aurore/intervals/bayesian/central.h>
#include <aurore/intervals/frequentist/likelihood_ratio.h>

class MyData : public aurore::Dataset {
  public:
    MyData(double _value) : value(_value) {}
    virtual ~MyData() {}
    double value;
};


double myNLL(const std::vector<double>& params, aurore::Dataset* data) {
  MyData* mydata = dynamic_cast<MyData*>(data);
  return 0.5 * TMath::Power(params[0] - mydata->value, 2);
}

aurore::Dataset* myMC(const std::vector<double>& params) {
  return new MyData(gRandom->Gaus(params[0], 1));
}

int main(int argc, char* argv[]) {
  gRandom->SetSeed(0);

  std::vector<double> initial_params(1, 10.0);
  std::vector<std::string> param_names(1, "a");

  aurore::FitSimple fit(param_names, &myNLL, &myMC);
  aurore::Dataset* data = fit.generate_data(initial_params);
  std::cout << "The correct answer is "
            << dynamic_cast<MyData*>(data)->value
            << " with a likelihood of 0.0"
            << std::endl << std::endl;

  // Fit with MIGRAD
  {
    aurore::Fitter::BestFit* results = fit.migrad(initial_params, data);
    std::cout << "migrad: " << results->parameters[0]
              << ", nll = " << results->value << std::endl;
    delete results;
  }

  // Use the Minuit2 wrapper to get MINOS errors
  ROOT::Minuit2::FCNBase* migradfunc = fit.get_minuit2_fcn(data);
  std::vector<double> errors(initial_params.size(), 1.0);
  ROOT::Minuit2::MnUserParameters mn_params(initial_params, errors);
  ROOT::Minuit2::MnMigrad migrad(*migradfunc, mn_params);  
  ROOT::Minuit2::FunctionMinimum minimum = migrad();  //FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters migradresult = minimum.UserParameters();
  ROOT::Minuit2::MnMinos minos(*migradfunc, minimum);
  migradfunc->SetErrorDef(0.5 * 2.71);  // 90% CL for NLL with 1 d.o.f.
  std::pair<double, double> minos_errors = minos(0);

  std::cout << "Minos: " << migradresult.Params()[0]
            << " -" << -1 * minos_errors.first
            << " +" << minos_errors.second
            << std::endl << std::endl;

  // Fit with a Markov Chain Monte Carlo, using the Metropolis algorithm
  {
    std::vector<double> jump_sigma(1, 5);
    aurore::samplers::Metropolis metropolis(jump_sigma);
    aurore::LikelihoodSpace* lspace;
    aurore::Fitter::BestFit* results = \
      fit.markov(initial_params, data, metropolis, lspace);
    std::cout << "markov: " << results->parameters[0]
              << ", nll = " << results->value << std::endl;

    // Calculate some intervals
    aurore::intervals::bayesian::Central central(0.9, lspace);
    std::pair<double, double> interval = central("a");
    std::cout << "Bayesian central: " << results->parameters[0]
              << " -" << results->parameters[0] - interval.first
              << " +" << interval.second - results->parameters[0]
              << std::endl;

    aurore::intervals::frequentist::LikelihoodRatio fc(0.9, 10000, data,
                                                       lspace, &fit);
    interval = fc("a");
    std::cout << "Frequentist FC: " << results->parameters[0]
              << " -" << results->parameters[0] - interval.first
              << " +" << interval.second - results->parameters[0]
              << std::endl;
  }

  return 0;
}

