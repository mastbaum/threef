#ifndef __AURORE_NEYMAN__
#define __AURORE_NEYMAN__

namespace aurore {

class LikelihoodSpace;

/** Abstract base for interval-computing functors. */
class Interval {
  public:
    Interval(LikelihoodSpace* _likelihood_space)
        : likelihood_space(_likelihood_space) {}
    virtual ~Interval() {}

    virtual std::pair<double, double> operator()(float cl, std::string param) = 0;

  protected:
    LikelihoodSpace* likelihood_space;
};

}  // namespace aurore

#endif  // __AURORE_NEYMAN__

