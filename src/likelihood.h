#ifndef __AURORE_LIKELIHOOD__
#define __AURORE_LIKELIHOOD__

#include <vector>
#include <utility>
#include <string>

class TNtuple;
class TH1F;

namespace aurore {

/**
 * Likelihood space.
 *
 * Wraps a set of samples from the likelihood function, and provides
 * statistics functions.
 */
class LikelihoodSpace {
  public:
    /** Functor to sort samples by a parameter value. */
    struct ParamSorter {
      ParamSorter(size_t index) : param_index(index) {}
      ParamSorter(std::string _param, std::vector<std::string> param_names);
      bool operator()(const std::pair<std::vector<double>, double>& left,
                      const std::pair<std::vector<double>, double>& right);
      size_t param_index;
    };

    /** Functor to sort samples by likelihood value. */
    struct LikelihoodSorter {
      bool operator()(const std::pair<std::vector<double>, double>& left,
                      const std::pair<std::vector<double>, double>& right);
    };

    /**
     * Constructor.
     *
     * \param samples A set of samples of the likelihood space
     */
    LikelihoodSpace(std::vector<std::pair<std::vector<double>, double> >& samples,
                    std::vector<std::string> param_names);

    /** Destructor. */
    virtual ~LikelihoodSpace();

    /** Get the samples. */
    std::vector<std::pair<std::vector<double>, double> > get_samples() {
      return this->samples;
    }

    /** Get the parameter names. */
    const std::vector<std::string>& get_parameter_names() {
      return this->param_names;
    }

    /** Get the parameters for the maximum L point. */
    std::vector<double> get_best_fit(double& nll);

    /**
     * Get a projection.
     *
     * \param name Name of dimension to project out
     * \param cut ROOT drawing cut
     * \returns ROOT TH1F with projected histogram
     */
    TH1F* get_projection(std::string name, std::string cut="");

    /**
     * Get points within a given distance of the maximum.
     *
     * \param delta Number of likelihood units from max to include
     * \returns TNtuple with requested samples
     */
    //TNtuple* get_contour(float delta);

  private:
    std::vector<std::pair<std::vector<double>, double> > samples;
    std::vector<std::string> param_names;
    std::pair<std::vector<double>, double> best_fit;
    TNtuple* samples_ntuple;  //!< Samples of the likelihood function
};

}  // namespace aurore

#endif  // __AURORE_LIKELIHOOD__

