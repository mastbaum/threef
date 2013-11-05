#ifndef __AURORE_SAMPLER__
#define __AURORE_SAMPLER__

#include <vector>

namespace aurore {

/** A PDF sampler. */
class Sampler {
  public:
    Sampler() {}
    virtual ~Sampler() {}

    virtual std::vector<double> propose_jump(const std::vector<double>& coordinates) const = 0;
    virtual bool accept(const double current, const double proposed) const = 0;
};

}  // namespace aurore

#endif  // __AURORE_SAMPLER__

