#include <vector>
#include <string>

#include <TNtuple.h>
#include <Minuit2/FCNBase.h>

#include <fitwrapper.h>

class FitSimple : public FitWrapper{
  public:
    FitSimple(double (*nllFunc)(const std::vector<double>&), void (*mcFunc)(const std::vector<double>&));

    // We override the required FitWrapper funcs
    void GenerateData(const std::vector<double>& params){fMCFunc(params);};
    double GetNLL(const std::vector<double>& params) const {return fNLLFunc(params);};
    // end required
    
  protected:

    double (*fNLLFunc)(const std::vector<double>&);
    void (*fMCFunc)(const std::vector<double>&);
};
