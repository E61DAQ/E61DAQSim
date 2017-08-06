#ifndef FEESim_H
#define FEESim_H

#include <string>
#include <iostream>

#include "Tool.h"

class FEESim: public Tool {


 public:

  FEESim();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

 zmq::socket_t* sock;



};


#endif
