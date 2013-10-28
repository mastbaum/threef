#include <vector>
#include <string>

#include <TNtuple.h>
#include <Minuit2/FCNBase.h>

class FitWrapper : private ROOT::Minuit2::FCNBase{
  public:
    FitWrapper(){};

    // MUST OVERRIDE PURE VIRTUAL METHODS BELOW
    virtual void GenerateData(const std::vector<double>& params) = 0;
    virtual double GetNLL(const std::vector<double>& params) const = 0;
    // END PURE VIRTUAL METHODS
    
    void SetParamNames(std::vector<std::string> paramNames){fParamNames = paramNames;};


    std::vector<double> MigradFit(const std::vector<double>& initparams);
    std::vector<double> MCMCFit(const std::vector<double>& initparams, size_t steps=500000, float burnin=0.1, float jump=0.1);
    std::vector<double> SAnnealFit(const std::vector<double>& initparams, size_t steps=500000, float burnin=0.1, float jump=0.1);

    TNtuple* MCMCMapLikelihood(const std::vector<double>& initparams, size_t steps=500000, float burnin=0.1, float jump=0.1);


    std::vector<std::pair<double, double> > GetFrequentistIntervals(std::vector<std::pair<double, double> > param_limits,
                        std::vector<size_t> grid_steps,
                        std::vector<double> initparams,
                        double cl = 0.9,
                        size_t nfake = 100) {
 
  protected:
      double operator()( const std::vector<double>& lParams ) const {
        return GetNLL(lParams);
      };
      double Up() const { return 0.5; }

    std::vector<std::string> fParamNames;
};
