#include <vector>
#include <algorithm>

#include <iostream>
#include <TRandom.h>
#include <TFile.h>
#include <TMath.h>

#include <Minuit2/FCNBase.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnMinos.h>

#include <fitsimple.h>

std::vector<double> truth(1);

class MyData : public FFF::DataWrapper{
  public:
    MyData(double _truth){truth = _truth;};
    ~MyData(){};

    double truth;
};

double myNLL(const std::vector<double>& params, const FFF::DataWrapper* data)
{
  const MyData* mydata = dynamic_cast<const MyData*> (data);
  
  if (params[0] < 0 || params[0] > mydata->truth*mydata->truth)
    return 1e9;
  return -1.0*TMath::Log(1-(params[0]-mydata->truth)*(params[0]-mydata->truth)/(mydata->truth*mydata->truth));
}

FFF::DataWrapper* myMC(const std::vector<double>& params)
{
  MyData* data = new MyData(params[0]);
  return data;
}

int main(){

  std::vector<double> initparams;
  initparams.push_back(5.0);

  FFF::FitSimple fit(&myNLL,&myMC);
  FFF::DataWrapper* data = fit.GenerateData(initparams);
  std::cout << "The correct answer is 5.0, with a likelihood of 0.0" << std::endl;


  std::vector<double> results = fit.MigradFit(initparams,data);
  std::cout << "Migrad: " << results[0] << " L=" << results[1] << std::endl;

  TNtuple *lspace = fit.MCMCMapLikelihood(initparams,data);
  TFile f("lspace.root","RECREATE");
  lspace->Write();
  f.Close();
  results = fit.MCMCFit(initparams,data);
  std::cout << "MCMC: " << results[0] << " L=" << results[1] << std::endl;

  results = fit.SAnnealFit(initparams,data);
  std::cout << "SAnneal: " << results[0] << " L=" << results[1] << std::endl;

  // You can also do normal minuit2 stuff
  ROOT::Minuit2::FCNBase* migradfunc = fit.GetMinuit2FCNBase(data);
  std::vector<double> verrors;
  for (size_t i=0;i<initparams.size();i++)
    verrors.push_back(0.0); //FIXME
  ROOT::Minuit2::MnUserParameters mnParams(initparams,verrors);
  ROOT::Minuit2::MnMigrad migrad(*migradfunc,mnParams);  
  ROOT::Minuit2::FunctionMinimum theMin = migrad(); //FIXME maxfcn, tol
  ROOT::Minuit2::MnUserParameters migradresult = theMin.UserParameters();
  ROOT::Minuit2::MnMinos minos(*migradfunc,theMin);
  std::pair<double, double> errors = minos(0);

  std::cout << "Minos errors:" << std::endl;
  std::cout << migradresult.Params()[0] << " +" << errors.second << " -" << -1*errors.first << std::endl;

  return 0;

}
