#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <assert.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <generator.h>
#include <anneal.h>
#include <likelihood.h>
#include <nll.h>

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
    float mean = h->GetMean();
    float rms = h->GetRMS();
    std::pair<int, int> l = std::make_pair<int, int>(h->GetXaxis()->GetXmin(),
                                                     h->GetXaxis()->GetXmax());
    delete h;
    limits.push_back(l);    
  }

  return limits;
}


TH2F* grid_scan_contour(std::vector<std::pair<float, float> > param_limits,
                        std::vector<size_t> grid_steps,
                        std::vector<TH1F*> pdfs,
                        std::vector<std::string> names,
                        std::vector<float> rates,
                        std::vector<float> constraints,
                        std::vector<float> data,
                        float ml, float cl,
                        size_t nfake) {
  assert(param_limits.size() == grid_steps.size());
  assert(param_limits.size() == 2);  // :(

  std::vector<float> delta;
  for (size_t i=0; i<param_limits.size(); i++) {
    delta.push_back((param_limits[i].second - param_limits[i].first) / grid_steps[i]);
  }

  TH2F* hcont = \
    new TH2F("hcont", "Contour", grid_steps[0], param_limits[0].first, param_limits[0].second,
                                 grid_steps[1], param_limits[1].first, param_limits[1].second);

  for (size_t i=0; i<grid_steps[0]; i++) {
    float a = param_limits[0].first + i * delta[0];
    for (size_t j=0; j<grid_steps[1]; j++) {
      float b = param_limits[1].first + j * delta[1];
      std::vector<float> norms_here;
      norms_here.push_back(a);
      norms_here.push_back(b);

      // Ld = L(Ns, Ti | data)
      float ld = nll(pdfs, norms_here, rates, constraints, data);
      float rd = ld - ml;

      std::vector<float> rs;
      for (size_t k=0; k<nfake; k++) {
        std::vector<float> d = make_fake_data(pdfs, norms_here, constraints);

        // Lx = L(Ns, Ti | fake data)
        float lx = nll(pdfs, norms_here, rates, constraints, d);

        // Lf = L(hat(Ns), hat(Ti) | fake data)
        TNtuple* lspacef = anneal(pdfs, names, norms_here, constraints, d, 50, 25);
        float lf;
        std::vector<float> bff = get_best_fit(lspacef, lf);
        delete lspacef;

        float rf = lf - lx;
        rs.push_back(rf);
      }

      // Find Rc s.t. P(R < Rc) = 0.9
      std::sort(rs.begin(), rs.end());
      float rc = rs[(int)(rs.size() * cl)];

      std::cout << (rd < rc ? "A" : "R") << " "
                << "(" << a << ", " << b << "), "
                << "Rc = " << rc << ", "
                << "Rd = " << rd << std::endl;

      // If R(Ns, Ti | data) <= Rc, this point is in the contour
      if (rd <= rc) {
        hcont->Fill(a, b, 10000);
      }
    }

    TCanvas c1;
    hcont->Draw("box");
    c1.SaveAs("cc.pdf");
  }

  return hcont;
}

