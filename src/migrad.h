#ifndef __AURORE_MIGRAD__
#define __AURORE_MIGRAD__

#include <vector>
#include <Minuit2/FCNBase.h>

namespace aurore {

class Fitter;
class Dataset;

class MigradFCN : public ROOT::Minuit2::FCNBase {
  public:
    MigradFCN(Fitter* _fitter, Dataset* _data)
        : fitter(_fitter), data(_data) {}

    double operator()(const std::vector<double>& params) const;

    double Up() const { return 0.5; }

  protected:
    Fitter* fitter;
    Dataset* data;
};

}  // namespace aurore

#endif  // __AURORE_MIGRAD__

