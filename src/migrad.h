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
        : fitter(_fitter), data(_data), error_def(0.5) {}

    double operator()(const std::vector<double>& params) const;

    double Up() const { return error_def; }
    void SetErrorDef(double def) { error_def = def; }

  protected:
    Fitter* fitter;
    Dataset* data;
    double error_def;
};

}  // namespace aurore

#endif  // __AURORE_MIGRAD__

