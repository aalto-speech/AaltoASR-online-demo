#ifndef ADAPTER_HH
#define ADAPTER_HH

#include "FeatureGenerator.hh"
#include "MllrTrainer.hh"
#include "HmmSet.hh"

/** Class for estimating MLLR adaptation matrix. */
class Adapter {
public:

  /** MLLR matrix estimator. */
  aku::MllrTrainer *m_mllr_trainer;

  /** HMM acoustic model */
  aku::HmmSet &m_model;

private:
  /** Adaptation tree representation needed for MllrTrainer */
  aku::RegClassTree m_rtree;

  /** Model transformer storing the cMLLR transformation(s) */
  aku::ModelTransformer m_model_trans;

  /** Number of frames currently in statistics */
  int m_num_adapt_frames;

public:
  
  /** Constructor.
   * \param model = acoustic hmm model 
   * \param feagen = feature generator used only for checking dimensions by 
   * MllrTrainer
   */
  Adapter(HmmSet &model);
  virtual ~Adapter();

  /** Add adaptation data to statistics, for estimating the adaptation matrix later on 
   * \param str = state history information from the decoder
   * \param features = Acoustic features of the current utterance
   */
  void add_adaptation_data(const std::string &str,
			   const std::vector<std::vector<double> > &features);

  /** Forget all previous adaptation information. */
  void reset();

  /** Get number of frames currently available for adaptation estimation */
  int get_num_adapt_frames(void) { return m_num_adapt_frames; }

  /** Estimate the adaptation matrix in \ref mllr_trainer object. */
  void compute_adaptation(void);
};

#endif /* ADAPTER_HH */
