#ifndef __LIKELIHOOD_H__
#define __LIKELIHOOD_H__

#include <vector>
#include <string>

class TH2F;
class TNtuple;

/** Get the variable names from a TNtuple. */
std::vector<std::string> get_field_names(TNtuple* ntuple);

/** Get the maximum likelihood parameters and value. */
std::vector<float> get_best_fit(TNtuple* lspace, float& ml);

/** Get the approximate ranges of the parameters in a TNtuple. */
std::vector<std::pair<float, float> > get_parameter_limits(TNtuple* ntuple);

/**
 * Perform a grid scan to find the acceptance region.
 *
 * Note: only works for the 2D case.
 *
 * \todo Extend to N dimensions (recursive).
 * \todo Improve the output; should be a continuous function.
 *
 * \param param_limits Extents for scan in each dimension
 * \param grid_step Number of scanning steps in each dimension
 * \param pdfs PDF histograms
 * \param names Names of signals
 * \param rates Expected rates for signals, used for constraints
 * \param constraints Gaussian constraints on signal rates
 * \param data The "true" data set
 * \param ml Global best-fit maximum likelihood
 * \param cl Frequentist confidence level
 * \param nfake Number of experiments to simulate at each grid point
 * \returns A 2D histogram where nonzero bins are in the contour.
 */
TH2F* grid_scan_contour(std::vector<std::pair<float, float> > param_limits,
                        std::vector<size_t> grid_steps,
                        std::vector<TH1F*> pdfs,
                        std::vector<std::string> names,
                        std::vector<float> rates,
                        std::vector<float> constraints,
                        std::vector<float> data,
                        float ml, float cl,
                        size_t nfake=100);

#endif  // __LIKELIHOOD_H__

