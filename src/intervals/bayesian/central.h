#ifndef __AURORE_INTERVALS_BAYESIAN_CENTRAL__
#define __AURORE_INTERVALS_BAYESIAN_CENTRAL__

#include <utility>
#include <string>
#include <aurore/neyman.h>

namespace aurore {
  class Fitter;
  class LikelihoodSpace;
  namespace intervals {
    namespace bayesian {

/** Project the space onto the dimension of interest and integrate. */
class Central : public NeymanInterval {
  public:
    Central(float _cl, Fitter* _fitter, LikelihoodSpace* _likelihood_space)
        : NeymanInterval(_cl, _fitter, _likelihood_space) {}
    virtual ~Central() {}

    virtual std::pair<double, double> operator()(std::string param);
};

    }  // namespace bayesian
  }  // namespace intervals
}  // namespace aurore

#endif  // __AURORE_INTERVALS_BAYESIAN_CENTRAL__

