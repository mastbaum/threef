#ifndef __PDFS_H__
#define __PDFS_H__

class TH1F;

namespace pdfs {

/** A flat pdf. */
TH1F* make_flat(float min, float max, int bins=25);

/** A Gaussian pdf. */
TH1F* make_gaussian(float min, float max, float mean, float sigma, int bins=25);

}  // namespace pdfs

#endif  // __PDFS_H__

