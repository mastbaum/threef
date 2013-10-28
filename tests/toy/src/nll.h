#ifndef __NLL_H__
#define __NLL_H__

#include <fitwrapper.h>

class TH1D;

class NLL : public FitWrapper {
  public:
    NLL(){};

    void GenerateData(const std::vector<double>& params);

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

    double GetNLL(const std::vector<double>& params) const;

    void SetPDFs(std::vector<TH1D*> pdfs){fPDFs = pdfs;};
    void SetExpectations(std::vector<double> expectations){fExpectations = expectations;};
    void SetConstraints(std::vector<double> constraints){fConstraints = constraints;};
    void SetData(std::vector<double> data){fData = data;};

  protected:
    std::vector<TH1D*> fPDFs;
    std::vector<double> fExpectations;
    std::vector<double> fConstraints;
    std::vector<double> fData;
};

#endif  // __NLL_H__

