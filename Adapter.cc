#include "str.hh"
#include "LinearAlgebra.hh" // For Vector
#include "Adapter.hh"

using namespace aku;

Adapter::Adapter(HmmSet &model)
  : m_model(model)
{
  // Initialize global cMLLR
  m_rtree.set_unit_mode(RegClassTree::UNIT_NO);
  m_rtree.initialize_root_node(&m_model);
  m_mllr_trainer = new MllrTrainer(&m_rtree, &m_model);
  
  m_model_trans.set_model(&m_model);
  m_model_trans.module("cmllr");

  m_num_adapt_frames = 0;
}


Adapter::~Adapter()
{
  delete m_mllr_trainer;
}

void
Adapter::add_adaptation_data(const std::string &str, 
               const std::vector<std::vector<double> > &features)
{
  m_model_trans.reset_transforms(); // Remove old adaptation

  std::vector<std::string> fields = str::split(str, " \t", true);
  if (fields.size() % 2 != 1) {
    fprintf(stderr, 
            "WARNING: Adapter::adapt(): invalid state history string\n");
    return;
  }

  bool ok = true;
  int start_frame = str::str2long(fields[0]);
  for (int i = 1; i < (int)fields.size(); i += 2) {
    assert(i + 1 < (int)fields.size());
    int state_index = str::str2long(fields[i]);
    int end_frame = str::str2long(fields[i+1]);
    if (!ok) {
      fprintf(stderr, 
              "ERROR: Adapter::adapt(): invalid start_frame or state_index\n");
      exit(1);
    }
    HmmState &state = m_model.state(state_index);

    assert(end_frame > start_frame);
    int dim = features.at(start_frame).size();
    Vector data(dim);
    for (int f = start_frame; f < end_frame; f++) {
      FeatureVec fea = FeatureVec(&data, dim);
      fea.set(features.at(f));
      m_mllr_trainer->collect_data(1, &state, fea);
      m_num_adapt_frames++;
    }
    start_frame = end_frame;
  }

  m_model_trans.load_transforms(); // Restore adaptation (if any)
}


void Adapter::compute_adaptation(void)
{
  m_model_trans.reset_transforms();
  m_mllr_trainer->calculate_transform(
    dynamic_cast< ConstrainedMllr* >(m_model_trans.module("cmllr")), 1, 0);
  m_model_trans.load_transforms();
}


void Adapter::reset(void)
{
  // Reset the adaptation
  m_model_trans.reset_transforms();
  // Reset the trainer
  delete m_mllr_trainer;
  m_mllr_trainer = new MllrTrainer(&m_rtree, &m_model);

  m_num_adapt_frames = 0;
}
