#ifndef DECODER_HH
#define DECODER_HH

#include <Toolbox.hh>
#include "msg.hh"
#include "conf.hh"

class Decoder {
public:
  void init(conf::Config &conf);
  void reset();
  void run();
  void message_result(bool guaranteed);

  bool verbose;
  Toolbox t;
  msg::InQueue in_queue;
  msg::OutQueue out_queue;
  HistoryVector hist_vec;
};

#endif /* DECODER_HH */
