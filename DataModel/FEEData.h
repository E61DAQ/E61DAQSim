#ifndef FEEDATA_H
#define FEEDATA_H

#include <map>
#include <string>
#include <vector>

#include <zmq.hpp>
#include <Hit.h>

class FEEData {

 public:

  FEEData(int id){m_id=id;};

  std::vector<char> Hits;
  int m_id;
  unsigned long m_trignum;

 private:

  
};



#endif
