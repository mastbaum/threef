#ifndef __AURORE_SAMPLERS_METROPOLIS__
#define __AURORE_SAMPLERS_METROPOLIS__

#include <vector>

namespace aurore {
  namespace samplers {

class Metropolis : public Sampler {
  public:
    Metropolis(std::vector<double> _jump_sigma) : jump_sigma(_jump_sigma) {}
    virtual ~Metropolis() {}

    virtual std::vector<double> propose_jump(const std::vector<double>& coordinates) const;
    virtual bool accept(const double current, const double proposed) const;

  protected:
    std::vector<double> jump_sigma;
};

  }  // namespace samplers
}  // namespace aurore

#endif  // __AURORE_SAMPLERS_METROPOLIS__

