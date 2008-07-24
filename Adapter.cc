#include "str.hh"
#include "Adapter.hh"

Adapter::Adapter(HmmSet &model, FeatureGenerator &feagen)
  : mllr(model, feagen),
    model(model)
{
}

void
Adapter::adapt(const std::string &str, 
               const std::vector<std::vector<double> > &features,
               LinTransformModule *mllr_mod)
{
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
    HmmState &state = model.state(state_index);

    assert(end_frame > start_frame);
    for (int f = start_frame; f < end_frame; f++) {
      FeatureVec fea = FeatureVec();
      fea.set(features.at(f));
      // FIXME: what should the prior be?
      // FeatureVec fea(&(features.at(f)), features.at(f).size());
      // mllr.find_probs(&state, fea);
      mllr.find_probs(1/(double)model.num_states(), &state, fea);
    }
    start_frame = end_frame;
  }
  mllr.calculate_transform(mllr_mod);
}
