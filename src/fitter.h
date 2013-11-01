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
class LikelihoodSpace;

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

    /**
     * Constructor.
     *
     * \param[in] _param_names Names of the fit parameters
     */
    Fitter(std::vector<std::string> _param_names)
        : param_names(_param_names) {}

    /**
     * Generate a data set.
     *
     * \param[in] params Parameter values to use for generation
     * \returns A Dataset
     */
    virtual Dataset* generate_data(const std::vector<double>& params) const = 0;

    /**
     * Evaluate the negative log likelihood (NLL) function.
     *
     * \param[in] params Parameters at which to evaluate
     * \param[in] data Data set with which to evaluate
     * \returns The NLL value
     */
    virtual double nll(const std::vector<double>& params, Dataset* data) const = 0;

    /**
     * Get a Minuit2 FCNBase object given the NLL function and data.
     *
     * Use this to take an aurore::Fitter and use it in Minuit2.
     *
     * \param[in] data The dataset to fit
     * \returns A Minuit2 FCNBase to plug into a Minuit2 fit
     */
    ROOT::Minuit2::FCNBase* get_minuit2_fcn(Dataset* data);

    /**
     * Fit using ROOT (Minuit)'s Migrad fitter.
     *
     * \param[in] initial_params Initial guess for parameters
     * \param[in] data The data set to fit
     * \returns The best fit parameters
     */
    Fitter::BestFit* migrad(const std::vector<double>& initial_params,
                           Dataset* data);

    /**
     * Fit by mapping out the likelihood space with a Markov Chain MC.
     *
     * \param[in] initial_params Initial guess for parameters
     * \param[in] data The data set to fit
     * \param[in] sampler The sampler defining how to explore the space
     * \param[in] steps Number of steps in random walk
     * \param[in] burnin_fraction Fraction of initial steps to throw away
     * \param[out] likelihood_space The set of samples from the NLL function
     * \returns The best fit parameters
     */
    Fitter::BestFit* markov(const std::vector<double>& initial_params,
                           Dataset* data,
                           const Sampler& sampler,
                           const size_t steps, const float burnin_fraction,
                           LikelihoodSpace*& likelihood_space);

  protected:
    std::vector<std::string> param_names;  //!< Names of the fit parameters
};


/** Results from a fit. */
struct Fitter::BestFit {
  std::vector<double> parameters;  //!< Values of parameters at best-fit point
  double value;  //!< Value of function at best-fit point
};


/** Simple interface to fitting. */
class FitSimple : public Fitter {
  public:
    /** A negative log likelihood function. */
    typedef double (*NLLFunction)(const std::vector<double>&, Dataset*);

    /** A function that generates data for given model parameters. */
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
      NLLFunction nll_function;  //!< The NLL function
      MCFunction mc_function;  //!< The data generation function
};

}  // namespace aurore

#endif  // __AURORE_FITTER

