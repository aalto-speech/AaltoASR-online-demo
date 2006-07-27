#ifndef ADAPTER_HH
#define ADAPTER_HH

#include "FeatureGenerator.hh"
#include "MllrTrainer.hh"
#include "HmmSet.hh"

/** Class for estimating MLLR adaptation matrix. */
class Adapter {
public:

  /** MLLR matrix estimator. */
  MllrTrainer mllr;

  /** HMM acoustic model */
  HmmSet &model;

public:
  
  /** Constructor.
   * \param model = acoustic hmm model 
   * \param feagen = feature generator used only for checking dimensions by 
   * MllrTrainer
   */
  Adapter(HmmSet &model, FeatureGenerator &feagen);

  /** Estimate the adaptation matrix in \ref mllr object. 
   * \param str = state history information from the decoder
   */
  void adapt(const std::string &str,
             const std::vector<std::vector<float> > &features,
             LinTransformModule *mllr_mod);

  /** Forget all previous adaptation information. */
  void reset();

};

#endif /* ADAPTER_HH */
