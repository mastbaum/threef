#include <cmath>
#include <vector>
#include <assert.h>
#include <TMath.h>
#include <TRandom.h>
#include <aurore/sampler.h>
#include <aurore/samplers/metropolis.h>

namespace aurore {
  namespace samplers {

std::vector<double> Metropolis::propose_jump(const std::vector<double>& coordinates) const {
  assert(coordinates.size() == this->jump_sigma.size());
  std::vector<double> proposal(coordinates.size());
  for (size_t i=0; i<coordinates.size(); i++) {
    proposal[i] = gRandom->Gaus(coordinates[i], this->jump_sigma[i]);
  }
  return proposal;
}

bool Metropolis::accept(const double current, const double proposed) const {
  if (proposed > current) {
    return true;
  }

  if (gRandom->Uniform() <= std::exp(proposed - current)) {
    return true;
  }

  return false;
}

  }  // namespace samplers
}  // namespace aurore

