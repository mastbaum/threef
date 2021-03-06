#ifndef __AURORE_INTERVALS_BAYESIAN_CENTRAL__
#define __AURORE_INTERVALS_BAYESIAN_CENTRAL__

#include <utility>
#include <string>
#include <aurore/interval.h>

namespace aurore {
  class LikelihoodSpace;
  namespace intervals {
    namespace bayesian {

/** Project the space onto the dimension of interest and integrate. */
class Central : public Interval {
  public:
    Central(LikelihoodSpace* _likelihood_space)
        : Interval(_likelihood_space) {}
    virtual ~Central() {}

    virtual std::pair<double, double> operator()(float cl, std::string param);
};

    }  // namespace bayesian
  }  // namespace intervals
}  // namespace aurore

#endif  // __AURORE_INTERVALS_BAYESIAN_CENTRAL__

