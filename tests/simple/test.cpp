#include <vector>
#include <algorithm>

#include <iostream>
#include <TRandom.h>
#include <TFile.h>
#include <TMath.h>

#include <fitsimple.h>

std::vector<double> truth(1);

double nll(const std::vector<double>& params)
{
  if (params[0] < 0 || params[0] > truth[0]*truth[0])
    return 1e9;
  return -1.0*TMath::Log(1-(params[0]-truth[0])*(params[0]-truth[0])/(truth[0]*truth[0]));
}

void mc(const std::vector<double>& params)
{
  truth[0] = params[0];
}

int main(){

  truth[0] = 3.0;

  std::vector<double> initparams;
  initparams.push_back(5.0);

  FitSimple fit(&nll,&mc);
  std::vector<double> results = fit.MigradFit(initparams);
  std::cout << results[0] << " " << results[1] << std::endl;

  TNtuple *lspace = fit.MCMCMapLikelihood(initparams);
  TFile f("lspace.root","RECREATE");
  lspace->Write();
  f.Close();
  results = fit.MCMCFit(initparams);
  std::cout << results[0] << " " << results[1] << std::endl;

  results = fit.SAnnealFit(initparams);
  std::cout << results[0] << " " << results[1] << std::endl;

  return 0;

}
