#ifndef __MCMC_H__
#define __MCMC_H__

#include <vector>
#include <string>

class TH1F;
class TNtuple;

TNtuple* mcmc(std::vector<TH1F*> pdfs, std::vector<std::string> names,
              std::vector<float> expectations, std::vector<float> constraints,
              std::vector<float> data, size_t steps=10000, float jump=5);

#endif  // __MCMC_H__

