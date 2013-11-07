#ifndef __AURORE_INTERVALS_FREQUENTIST_LIKELIHOOD_RATIO__
#define __AURORE_INTERVALS_FREQUENTIST_LIKELIHOOD_RATIO__

#include <utility>
#include <string>
#include <aurore/interval.h>

namespace aurore {
  class Dataset;
  class Fitter;
  class LikelihoodSpace;
  namespace intervals {
    namespace frequentist {

/**
  * Interval constructed according to the likelihood-ratio ordering principle.
  */
class LikelihoodRatio : public Interval {
  public:
    LikelihoodRatio(size_t _n_fake, Dataset* _data,
                    LikelihoodSpace* _likelihood_space, Fitter* _fitter)
        : Interval(_likelihood_space),
          fitter(_fitter), data(_data), n_fake(_n_fake) {}

    virtual ~LikelihoodRatio() {}

    virtual std::pair<double, double> operator()(float cl, std::string param);

  protected:
    virtual double find_boundary(size_t param_index,                       
                                 std::pair<double, double> range,          
                                 double tolerance, float cl);

    virtual double delta_r(float cl, std::vector<double> params); 

    virtual double get_critical_ratio(float cl, std::vector<double> params);

    Dataset* data;
    Fitter* fitter;
    size_t n_fake;  //!< Number of fake experiments
};

    }  // namespace frequentist
  }  // namespace intervals
}  // namespace aurore

#endif  // __AURORE_INTERVALS_FREQUENTIST_LIKELIHOOD_RATIO__

