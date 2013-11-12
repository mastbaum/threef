#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include <TH1F.h>
#include <aurore/likelihood.h>
#include <aurore/interval.h>
#include <aurore/intervals/bayesian/central.h>

namespace aurore {
  namespace intervals {
    namespace bayesian {

std::pair<double, double> Central::operator()(std::string param) {
  std::vector<std::pair<std::vector<double>, double> > samples = \
    this->likelihood_space->get_samples();

  std::vector<std::string> param_names = \
    this->likelihood_space->get_parameter_names();

  size_t param_index = (std::find(param_names.begin(),
                                  param_names.end(),
                                  param) -
                        param_names.begin());

  const std::vector<std::string>& names = \
    this->likelihood_space->get_parameter_names();

  std::sort(samples.begin(), samples.end(),
            LikelihoodSpace::ParamSorter(param, names));

  float tail = (1.0 - this->cl) / 2;

  double lower = samples[std::floor(tail * samples.size())].first[param_index];

  double upper = samples[std::ceil((1.0 - tail) *
                         samples.size())].first[param_index];

  return std::make_pair<double, double>(lower, upper);
}

    }  // namespace bayesian
  }  // namespace intervals
}  // namespace aurore

