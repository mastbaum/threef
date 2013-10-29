#include <vector>
#include <string>

#include <TNtuple.h>
#include <Minuit2/FCNBase.h>

#include <fitwrapper.h>

namespace FFF
{
  // This class is used if you have a simple application
  // and don't want to subclass FitWrapper yourself.
  // Instead you can just define a likelihood function and 
  // a monte carlo function and pass them to a FitSimple
  // object, which subclasses FitWrapper for you

  class FitSimple : public FitWrapper{
    public:
      FitSimple(double (*nllFunc)(const std::vector<double>&, const DataWrapper* data), DataWrapper* (*mcFunc)(const std::vector<double>&));

      // We override the required FitWrapper funcs
      DataWrapper* GenerateData(const std::vector<double>& params) const {return fMCFunc(params);};
      double GetNLL(const std::vector<double>& params, const DataWrapper* data) const {return fNLLFunc(params, data);};
      // end required

    protected:

      double (*fNLLFunc)(const std::vector<double>&, const DataWrapper* data);
      DataWrapper* (*fMCFunc)(const std::vector<double>&);
  };
}
