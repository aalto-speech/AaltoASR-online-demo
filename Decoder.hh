#ifndef DECODER_HH
#define DECODER_HH

#include <Toolbox.hh>
#include "msg.hh"
#include "conf.hh"

class Decoder {
public:
  Decoder();
  void init(conf::Config &conf);
  void reset();
  void run();
  void send_state_history();
  void message_result(bool send_all);

  bool verbose;
  Toolbox t;
  msg::InQueue in_queue;
  msg::OutQueue out_queue;
  HistoryVector hist_vec;
  int frame;
  bool paused;
  bool adaptation;

  LMHistory *last_guaranteed_history;
};

#endif /* DECODER_HH */
