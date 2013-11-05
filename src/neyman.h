#ifndef __AURORE_NEYMAN__
#define __AURORE_NEYMAN__

namespace aurore {

class Fitter;
class LikelihoodSpace;

/** Abstract base for interval-computing functors. */
class NeymanInterval {
  public:
    NeymanInterval(float _cl, Fitter* _fitter,
                   LikelihoodSpace* _likelihood_space)
        : cl(_cl), fitter(_fitter), likelihood_space(_likelihood_space) {}
    virtual ~NeymanInterval() {}

    virtual std::pair<double, double> operator()(std::string param) = 0;

  protected:
    float cl;  //!< Confidence level
    Fitter* fitter;
    LikelihoodSpace* likelihood_space;
};

}  // namespace aurore

#endif  // __AURORE_NEYMAN__

