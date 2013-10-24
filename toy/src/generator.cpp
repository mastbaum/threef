#include <iostream>
#include <vector>
#include <TRandom.h>
#include <TH1F.h>
#include <generator.h>

std::vector<float> make_fake_data(std::vector<TH1F*> pdfs,
                                  std::vector<float> rates,
                                  std::vector<float> constraints) {
  std::vector<float> data;
  for (size_t i=0; i<rates.size(); i++) {
    float rate;
    if (constraints[i] > 0) {
      rate = gRandom->Gaus(rates[i], constraints[i]);
    }
    else {
      rate = rates[i];
    }
    int n = gRandom->Poisson(rate);
    for (int j=0; j<n; j++) {
      data.push_back(pdfs[i]->GetRandom());
    }
  }

  return data;
}

