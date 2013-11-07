#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <assert.h>
#include <TH1F.h>
#include <aurore/fitter.h>
#include <aurore/likelihood.h>
#include <aurore/interval.h>
#include <aurore/intervals/frequentist/likelihood_ratio.h>

namespace aurore {
  namespace intervals {
    namespace frequentist {

std::pair<double, double> LikelihoodRatio::operator()(float cl, std::string param) {
  std::vector<std::pair<std::vector<double>, double> > samples = \
    this->likelihood_space->get_samples();

  const std::vector<std::string>& names = \
    this->likelihood_space->get_parameter_names();

  size_t param_index = (std::find(names.begin(), names.end(), param) -
                        names.begin());

  std::sort(samples.begin(), samples.end(),
            LikelihoodSpace::ParamSorter(param, names));

  // Find min and max values of parameter to guide bisection
  std::pair<double, double> parameter_extrema = \
    std::make_pair<double, double>(samples.front().first[param_index],
                                   samples.back().first[param_index]);

  double ml;
  std::vector<double> best_fit = this->likelihood_space->get_best_fit();

  std::pair<double, double> lower_half = \
    std::make_pair<double, double>(parameter_extrema.first,
                                   best_fit[param_index]);

  double lower = find_boundary(param_index, lower_half, 1e-4, cl);

  std::pair<double, double> upper_half = \
    std::make_pair<double, double>(best_fit[param_index],
                                   parameter_extrema.second);

  double upper = find_boundary(param_index, upper_half, 1e-4, cl);

  return std::make_pair<double, double>(lower, upper);
}


double LikelihoodRatio::find_boundary(size_t param_index,
                                      std::pair<double, double> range,
                                      double tolerance, float cl) {
  double ml;
  std::vector<double> params = this->likelihood_space->get_best_fit(ml);

  double x = range.first;
  double delta = (range.second - range.first) / 2;
  params[param_index] = x;

  double dr = delta_r(cl, params);
  double drc = dr + 1;
  while (delta > tolerance || std::abs(dr - drc) > tolerance) {
    params[param_index] = x;
    drc = delta_r(cl, params);
    std::cout << "--> " << params[0] << ": " << dr << " " << drc << std::endl;

    // Check if the value has changed sign
    if (std::abs(dr) + std::abs(drc) > std::abs(dr + drc)) {
      x -= delta;
      delta /= 2;
    }
    else {
      x += delta;
    }
    dr = drc;
  }

  return x;
}


double LikelihoodRatio::delta_r(float cl, std::vector<double> params) {
  double ld = this->fitter->nll(params, this->data);

  double lm;
  this->likelihood_space->get_best_fit(lm);

  double rd = 2.0 * (ld - lm);
  double rc = get_critical_ratio(cl, params);
  std::cout << "    rd=" << rd << " rc=" << rc << std::endl;

  return rc - rd;
}

double LikelihoodRatio::get_critical_ratio(float cl, std::vector<double> params) {
  double tolerance = 1e-5;
  
  std::vector<double> rs;
  std::vector<double> rcs(5000);
  double s = 1;
  size_t iexp = 0;
  bool converged = false;
  double rc_average = 0;

  // Run fake experiments until Rc converges.
  do {
    iexp++;
    Dataset* fake_data = this->fitter->generate_data(params);
    double lx = this->fitter->nll(params, fake_data);
    Fitter::BestFit* best_fit = \
      this->fitter->migrad(params, fake_data);

    delete fake_data;

    double lf = best_fit->value;
    double rf = 2.0 * (lx - lf);
    rs.push_back(rf);

    std::sort(rs.begin(), rs.end());
    size_t idx = std::min((size_t)std::ceil(1.0 * rs.size() * cl), rs.size() - 1);
    
    double rc = rs.at(idx);

    // Using a fixed-length queue instead of keeping all the samples seems
    // to help.
    rcs[iexp % rcs.size()] = rc;

    // Should this be the total cumulative average?
    rc_average = std::accumulate(rcs.begin(), rcs.end(), 0.0) / rcs.size();

    // Convergence check
    if (rcs.size() > 1) {
      // This is sqrt( 1/n * sum( (x_i - x_ave)^2 ) )
      double s2 = 0;
      for (size_t i=0; i<rcs.size(); i++) {
        s2 += (rcs[i] - rc_average) * (rcs[i] - rc_average);
      }
      s2 *= (1.0 / (rcs.size() - 1));
      double s_old = s;
      s = std::sqrt(s2);
      std::cout << "// " << iexp << " " << rc << " " << rc_average << " " \
                << std::abs((s_old-s)/s) << std::endl;

      // Convergence: Variance is small
      if (iexp > 1000 && s < tolerance) {
        converged = true;
      }
    }
  } while (!converged);

  std::cout << "converged after " << iexp << " experiments" << std::endl;
  std::sort(rs.begin(), rs.end());
  size_t idx = std::min((size_t)std::ceil(1.0 * rs.size() * cl), rs.size() - 1);
  return rs[idx];
}

    }  // namespace frequentist
  }  // namespace intervals
}  // namespace aurore

