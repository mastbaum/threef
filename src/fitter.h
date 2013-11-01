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

/** Generic container for data. */
class Dataset {
  public:
    Dataset() {}
    virtual ~Dataset() {}
};


/** Abstract interface to fitters. */
class Fitter {
  public:
    class Result;

    Fitter(std::vector<std::string> _param_names)
        : param_names(_param_names) {}

    virtual Dataset* generate_data(const std::vector<double>& params) const = 0;
    virtual double nll(const std::vector<double>& params, Dataset* data) const = 0;

    ROOT::Minuit2::FCNBase* get_minuit2_fcn(Dataset* data);

    Fitter::Result* migrad(const std::vector<double>& initial_params,
                           Dataset* data);

    Fitter::Result* markov(const std::vector<double>& initial_params,
                           Dataset* data,
                           const size_t steps, const float burnin_fraction);

  protected:
    std::vector<std::string> param_names;
};

class LikelihoodSpace {}; 

/** Results from a fit. */
class Fitter::Result {
  public:
    /** A fitted parameter. */
    class Parameter {
      public:
        Parameter() : best_fit(0), interval(NULL) {}
        virtual ~Parameter() { delete this->interval; }

        /** Best-fit value of parameter. */
        void set_best_fit(const double _best_fit) { this->best_fit = _best_fit; }
        double get_best_fit() const { return this->best_fit; }

        /** A confidence interval, if defined. */
        void set_interval(const double a, const double b) {
          delete this->interval;
          this->interval = new std::pair<double, double>(a, b);
        }
        std::pair<double, double>* get_interval() const { return this->interval; }

      protected:
        double best_fit;  //!< Best-fit value
        std::pair<double, double>* interval;  //!< Confidence interval
    };

    Result() : ml(0), likelihood_space(NULL) {}
    virtual ~Result() { delete likelihood_space; }

    std::vector<Parameter> parameters;  //!< Parameters and uncertainties
    LikelihoodSpace* likelihood_space;  //!< Full likelihood space
    double ml;  //!< Value of the NLL at the best-fit point
};


typedef double (*NLLFunction)(const std::vector<double>&, Dataset*);
typedef Dataset* (*MCFunction)(const std::vector<double>&);

/** Simple interface to fitting. */
class FitSimple : public Fitter {
  public:
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

