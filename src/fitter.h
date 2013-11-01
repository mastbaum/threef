#ifndef __AURORE_FITTER__
#define __AURORE_FITTER__

#include <vector>
#include <string>

namespace ROOT {
  namespace Minuit2 {
    class FCNBase;
  }
}

namespace aurore {

class Sampler;

class LikelihoodSpace {};

/** Generic container for data. */
class Dataset {
  public:
    Dataset() {}
    virtual ~Dataset() {}
};


/** Abstract interface to fitters. */
class Fitter {
  public:
    struct BestFit;

    Fitter(std::vector<std::string> _param_names)
        : param_names(_param_names) {}

    virtual Dataset* generate_data(const std::vector<double>& params) const = 0;
    virtual double nll(const std::vector<double>& params, Dataset* data) const = 0;

    ROOT::Minuit2::FCNBase* get_minuit2_fcn(Dataset* data);

    Fitter::BestFit* migrad(const std::vector<double>& initial_params,
                           Dataset* data);

    Fitter::BestFit* markov(const std::vector<double>& initial_params,
                           Dataset* data,
                           const Sampler& sampler,
                           const size_t steps, const float burnin_fraction,
                           LikelihoodSpace*& likelihood_space);

  protected:
    std::vector<std::string> param_names;
};


/** Results from a fit. */
struct Fitter::BestFit {
  std::vector<double> parameters;
  double value;
};


/** Simple interface to fitting. */
class FitSimple : public Fitter {
  public:
    typedef double (*NLLFunction)(const std::vector<double>&, Dataset*);
    typedef Dataset* (*MCFunction)(const std::vector<double>&);

    FitSimple(std::vector<std::string> _param_names, NLLFunction _nll_function, MCFunction _mc_function)
        : Fitter(_param_names), nll_function(_nll_function), mc_function(_mc_function) {}

      Dataset* generate_data(const std::vector<double>& params) const {
        return mc_function(params);
      }

      double nll(const std::vector<double>& params, Dataset* data) const {
        return nll_function(params, data);
      }

    protected:
      NLLFunction nll_function;
      MCFunction mc_function;
};

}  // namespace aurore

#endif  // __AURORE_FITTER

