#include <fitsimple.h>

namespace FFF
{

  FitSimple::FitSimple(double (*nllFunc)(const std::vector<double>&, const DataWrapper* data), DataWrapper* (*mcFunc)(const std::vector<double>&)) : fNLLFunc(nllFunc), fMCFunc(mcFunc)
  {
  }

}
