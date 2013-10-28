#ifndef __PDFS_H__
#define __PDFS_H__

class TH1D;

namespace pdfs {

/** A flat pdf. */
TH1D* make_flat(double min, double max, int bins=25);

/** A Gaussian pdf. */
TH1D* make_gaussian(double min, double max, double mean, double sigma, int bins=25);

}  // namespace pdfs

#endif  // __PDFS_H__

