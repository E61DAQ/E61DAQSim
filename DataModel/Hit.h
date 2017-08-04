#ifndef HIT_H
#define HIT_H

#include <map>
#include <string>
#include <vector>

#include <zmq.hpp>

class Hit {

 public:

  Hit(char in[3]){*bytes=*in;};
  char bytes[3];

 private:


};



#endif
