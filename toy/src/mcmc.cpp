#include <iostream>
#include <vector>
#include <string>
#include <TH1F.h>
#include <TMath.h>
#include <TRandom.h>
#include <TNtuple.h>
#include "nll.h"
#include "mcmc.h"

TNtuple* mcmc(std::vector<TH1F*> pdfs, std::vector<std::string> names,
              std::vector<float> expectations, std::vector<float> constraints,
              std::vector<float> data, size_t steps) {
  unsigned long accepted = 0;
  unsigned long total = 0;
  float jump = 5;

  // Initial nll
  std::vector<float> norms = expectations;
  double lcurrent = nll(pdfs, norms, expectations, constraints, data);

  // The likelihood space
  std::string varlist;
  for (size_t i=0; i<names.size(); i++) {
    varlist += names[i] + ":";
  }
  varlist += "likelihood";
  TNtuple* lspace = new TNtuple("lspace", "Likelihood space", varlist.c_str());

  for (size_t istep=0; istep<steps; istep++) {
      // Jump
      std::vector<float> new_norms;
      for (size_t i=0; i<norms.size(); i++) {
        new_norms.push_back(gRandom->Gaus(norms[i], jump));
      }
 
      double lnew = nll(pdfs, new_norms, expectations, constraints, data);
 
      bool accept = false;
      double alpha = 0;
      if (lnew < lcurrent) {
          accept = true;
      }
      else if (lnew - lcurrent > 15) {
          accept = false;
      }
      else {
          alpha = TMath::Exp(lcurrent - lnew);
          if (gRandom->Uniform() <= alpha) {
              accept = true;
          }
      }
 
      //std::cout << "+  (" << coords[0]    << ", " << coords[1]    << "; " << lcurrent << ") "
      //          << "-> (" << newcoords[0] << ", " << newcoords[1] << "; " << lnew     << "), "
      //          << "a = " << alpha << " // " << (accept ? "ACCEPT" : "REJECT") << std::endl;

      if (accept) {
          accepted++;
          norms = new_norms;
          lcurrent = lnew;
      }

      // Use first 10% of steps for burn-in phase
      if (istep >= 1.0 * steps / 10) {
          float* v = new float[norms.size() + 1];
          for (size_t i=0; i<norms.size(); i++) {
            v[i] = norms[i];
          }
          v[norms.size()] = lcurrent;
          lspace->Fill(v);
      }

      total++;
  }
  std::cout << "mcmc: Accept ratio: " << 1.0 * accepted / total << std::endl;

  return lspace;
}

