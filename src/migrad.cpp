#include <vector>
#include <aurore/fitter.h>
#include <aurore/migrad.h>

namespace aurore {

double MigradFCN::operator()(const std::vector<double>& params) const {
  return this->fitter->nll(params, this->data);
};

}  // namespace aurore

