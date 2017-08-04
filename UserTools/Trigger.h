#ifndef Trigger_H
#define Trigger_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <zmq.hpp>

class Trigger: public Tool {


 public:

  Trigger();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  int m_trigger_rate;
  long m_trigger_period;
  boost::posix_time::ptime m_last_trigger;
  zmq::socket_t* TriggerSend;

};


#endif
