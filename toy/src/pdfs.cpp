#include <TRandom.h>
#include <TH1F.h>
#include <pdfs.h>

namespace pdfs {

TH1F* make_flat(float min, float max, int bins) {
  TH1F* h = new TH1F("h", "", bins, min, max);
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

TH1F* make_gaussian(float min, float max, float mean, float sigma, int bins) {
  TH1F* h = new TH1F("hs", "Energy spectrum", bins, min, max);
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

