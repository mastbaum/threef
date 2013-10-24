#ifndef __ANNEAL_H__
#define __ANNEAL_H__

#include <vector>
#include <string>

class TH1F;
class TNtuple;

TNtuple* anneal(std::vector<TH1F*> pdfs, std::vector<std::string> names,
                std::vector<float> expectations, std::vector<float> constraints,
                std::vector<float> data, size_t steps=1000, float jump=5);

#endif  // __ANNEAL_H__

