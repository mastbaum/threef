#include <vector>
#include <string>

#include <TNtuple.h>
#include <Minuit2/FCNBase.h>

namespace FFF
{
  // This class is just a fancy null pointer
  // You can subclass it with anything, we
  // just want to make sure we can delete it
  //
  class DataWrapper {
    public:
      DataWrapper(){};
      virtual ~DataWrapper(){};
  };



  class FitWrapper : private ROOT::Minuit2::FCNBase{
    public:
      FitWrapper(){};

      // MUST OVERRIDE PURE VIRTUAL METHODS BELOW
      virtual DataWrapper* GenerateData(const std::vector<double>& params) const = 0;
      virtual double GetNLL(const std::vector<double>& params, const DataWrapper* data) const = 0;
      // END PURE VIRTUAL METHODS

      void SetParamNames(std::vector<std::string> paramNames){fParamNames = paramNames;};


      std::vector<double> MigradFit(const std::vector<double>& initparams, const DataWrapper* data);
      std::vector<double> MCMCFit(const std::vector<double>& initparams, const DataWrapper* data, size_t steps=500000, float burnin=0.1, float jump=0.1);
      std::vector<double> SAnnealFit(const std::vector<double>& initparams, const DataWrapper* data, size_t steps=500000, float burnin=0.1, float jump=0.1);

      TNtuple* MCMCMapLikelihood(const std::vector<double>& initparams, const DataWrapper* data, size_t steps=500000, float burnin=0.1, float jump=0.1);


      std::vector<std::pair<double, double> > GetFrequentistIntervals(std::vector<std::pair<double, double> > param_limits,
          std::vector<size_t> grid_steps,
          const std::vector<double> initparams,
          const DataWrapper* data,
          double cl = 0.9,
          size_t nfake = 100);

    protected:
      double operator()( const std::vector<double>& lParams ) const {
        return GetNLL(lParams, fMinuitData);
      };
      double Up() const { return 0.5; }

      const DataWrapper* fMinuitData; 
      std::vector<std::string> fParamNames;
  };
}
