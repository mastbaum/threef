#ifndef __LIKELIHOOD_H__
#define __LIKELIHOOD_H__

#include <vector>
#include <string>

class TNtuple;

/** Get the variable names from a TNtuple. */
std::vector<std::string> get_field_names(TNtuple* ntuple);

/** Get the maximum likelihood parameters and value. */
std::vector<float> get_best_fit(TNtuple* lspace, float& ml);

/** Get the approximate ranges of the parameters in a TNtuple. */
std::vector<std::pair<float, float> > get_parameter_limits(TNtuple* ntuple);

#endif  // __LIKELIHOOD_H__

