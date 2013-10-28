// Just a hack to get around bug with CUDA and TMath header

namespace tmath{
  double Exp(double x);
  double Power(double x, double y);
}
