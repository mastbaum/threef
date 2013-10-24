#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <vector>

class TH1F;

std::vector<float> make_fake_data(std::vector<TH1F*> pdfs,
                                  std::vector<float> rates,
                                  std::vector<float> constraints);

#endif  // __GENERATOR_H__

