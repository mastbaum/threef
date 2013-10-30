#include <MigradFitter.h>
#include <fitwrapper.h>

namespace FFF
{

  double MigradFitter::operator()( const std::vector<double>& lParams ) const {
    return fFW->GetNLL(lParams, fMinuitData);
  };

}
