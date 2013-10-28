#include <fitsimple.h>

FitSimple::FitSimple(double (*nllFunc)(const std::vector<double>&), void (*mcFunc)(const std::vector<double>&)) : fNLLFunc(nllFunc), fMCFunc(mcFunc)
{

}
