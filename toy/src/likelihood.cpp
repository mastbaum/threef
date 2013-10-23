#include <vector>
#include <string>
#include <utility>
#include <assert.h>
#include <TNtuple.h>
#include <TDirectory.h>
#include <TH1F.h>

std::vector<std::string> get_field_names(TNtuple* ntuple) {
  std::vector<std::string> names;
  for (int i=0; i<ntuple->GetListOfBranches()->GetEntries(); i++) {
    std::string name = ntuple->GetListOfBranches()->At(i)->GetName();
    names.push_back(name);
  }
  return names;
}

std::vector<float> get_best_fit(TNtuple* lspace, float& ml) {
  std::vector<std::string> field_names = get_field_names(lspace);
  float* args = lspace->GetArgs();
  size_t nparams = field_names.size() - 1;

  ml = 1e9;
  std::vector<float> best_fit(nparams);

  for (int i=0; i<lspace->GetEntries(); i++) {
    if (args[nparams] < ml) {
      ml = args[nparams];
      for (size_t j=0; j<nparams; j++) {
        best_fit[j] = args[j];
      }
    }
  }

  return best_fit;
}

std::vector<std::pair<float, float> > get_parameter_limits(TNtuple* ntuple) {
  std::vector<std::pair<float, float> > limits;
  std::vector<std::string> names = get_field_names(ntuple);

  for (size_t i=0; i<names.size(); i++) {
    if (names[i] == "likelihood") {
      continue;
    }

    ntuple->Draw((names[i] + ">>_h").c_str(), "", "goff");
    TH1F* h = dynamic_cast<TH1F*>(gDirectory->FindObject("_h"));
    assert(h);
    std::pair<int, int> l = std::make_pair<int, int>(h->GetXaxis()->GetXmin(),
                                                     h->GetXaxis()->GetXmax());
    limits.push_back(l);    
  }

  return limits;
}

