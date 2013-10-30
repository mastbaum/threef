#ifndef __MIGRADFITTER_H__
#define __MIGRADFITTER_H__

#include <vector>
#include <string>

#include <TNtuple.h>
#include <Minuit2/FCNBase.h>

namespace FFF
{
  class FitWrapper;
  class DataWrapper;

  class MigradFitter : public ROOT::Minuit2::FCNBase{
    public:
      MigradFitter(FitWrapper* fw, const DataWrapper* data){fFW = fw;fMinuitData = data;};

      std::vector<double> Fit(const std::vector<double>& initparams, const DataWrapper* data);

      double operator()( const std::vector<double>& lParams ) const;
      double Up() const { return 0.5; };

    protected:

      FitWrapper* fFW;
      const DataWrapper* fMinuitData; 
  };
}

#endif
