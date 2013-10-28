#ifndef __NLL_H__
#define __NLL_H__

class TH1F;

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
double nll(std::vector<TH1F*> pdfs, std::vector<float> norms,
           std::vector<float> expectations, std::vector<float> constraints,
           std::vector<float> data);

#endif  // __NLL_H__

