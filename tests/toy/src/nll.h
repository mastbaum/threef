#ifndef __NLL_H__
#define __NLL_H__

#include <vector>

#include <fitwrapper.h>

class TH1D;

class MyData : public FFF::DataWrapper {
  public:
    MyData(){};
    ~MyData(){};

    std::vector<double> events;
};

class NLL : public FFF::FitWrapper {
  public:
    NLL(){};

    FFF::DataWrapper* GenerateData(const std::vector<double>& params) const;

    /**
     * The negative log likelihood function
     *
     * \param pdfs Array of PDF histograms
     * \param norms Current parameter vector
     * \param expectations Array of PDF normalizations
     * \param constraints Array of Gaussian constraints
     * \param data Array of data (energies)
     * \returns A double, the NLL
     */

    double GetNLL(const std::vector<double>& params, const FFF::DataWrapper* data) const;

    void SetPDFs(std::vector<TH1D*> pdfs){fPDFs = pdfs;};
    void SetExpectations(std::vector<double> expectations){fExpectations = expectations;};
    void SetConstraints(std::vector<double> constraints){fConstraints = constraints;};

  protected:
    std::vector<TH1D*> fPDFs;
    std::vector<double> fExpectations;
    std::vector<double> fConstraints;
};

#endif  // __NLL_H__

