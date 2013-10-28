#include <TRandom.h>
#include <TH1D.h>
#include <pdfs.h>

namespace pdfs {

TH1D* make_flat(double min, double max, int bins) {
  TH1D* h = new TH1D("h", "", bins, min, max);
  h->SetDirectory(NULL);
  h->SetXTitle("Energy");
  h->SetYTitle("Counts per bin");
  for (size_t i=0; i<500000; i++) {
      h->Fill(gRandom->Uniform(min, max));
  }
  h->Scale(1.0/h->Integral());
  h->SetDirectory(0);
  h->SetLineColor(kGreen);
  h->SetLineWidth(2);

  return h;
}

TH1D* make_gaussian(double min, double max, double mean, double sigma, int bins) {
  TH1D* h = new TH1D("hs", "Energy spectrum", bins, min, max);
  h->SetDirectory(NULL);
  for (size_t i=0; i<500000; i++) {
      h->Fill(gRandom->Gaus(mean, sigma));
  }
  h->Scale(1.0/h->Integral());
  h->SetDirectory(0);
  h->SetLineColor(kRed);
  h->SetLineWidth(2);

  return h;
}

}  // namespace pdfs

