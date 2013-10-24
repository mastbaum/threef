#include <assert.h>
#include <TH1F.h>
#include <TMath.h>
#include <nll.h>

double nll(std::vector<TH1F*> pdfs, std::vector<float> norms, 
           std::vector<float> expectations, std::vector<float> constraints,
           std::vector<float> data) {
    double result = 0;
    for (size_t i=0; i<norms.size(); i++) {
        // hard prior
        if (norms[i] < 0) {
            return 1e20;
        }

        result += norms[i];

        // gaussian constraints
        if (constraints[i] > 0) {
            result += 0.5 * TMath::Power((norms[i]-expectations[i]) /
                                          constraints[i], 2);
        }
    }

    // L -= sum over events ( log ( sum over pdfs ( N_j P_j(x_i)) ) )
    for (int i=0; i<data.size(); i++) {
        double s = 0;
        for (size_t j=0; j<norms.size(); j++) {
            s += norms[j] * pdfs[j]->Interpolate(data[i]);
        }
        if (s != s) {
            s = 1e-20;
            assert(false);
        }
        result -= TMath::Log(s);
    }

    return result;
}

