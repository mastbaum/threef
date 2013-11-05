#ifndef __AURORE_NEYMAN__
#define __AURORE_NEYMAN__

namespace aurore {

class LikelihoodSpace;

/** Abstract base for interval-computing functors. */
class NeymanInterval {
  public:
    NeymanInterval(float _cl, LikelihoodSpace* _likelihood_space)
        : cl(_cl), likelihood_space(_likelihood_space) {}
    virtual ~NeymanInterval() {}

    virtual std::pair<double, double> operator()(std::string param) = 0;

  protected:
    float cl;  //!< Confidence level
    LikelihoodSpace* likelihood_space;
};

}  // namespace aurore

#endif  // __AURORE_NEYMAN__

