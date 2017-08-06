#ifndef FEEReader_H
#define FEEReader_H

#include <string>
#include <iostream>
#include <stdlib.h>
#include "Tool.h"

#include <pthread.h>
#include <FEEData.h>

struct FEE_args{

  FEE_args(zmq::context_t* incontext,int inid, std::vector<FEEData*>* indata, bool* inrunning){

    context=incontext;
    FEEid=inid;
    data=indata;
    running=inrunning;
  }

  zmq::context_t* context;
  int FEEid;
  std::vector<FEEData*>* data;
  bool* running;
 
};



class FEEReader: public Tool {


 public:

  FEEReader();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  static void *FEESimThread(void *arg);
  static void *FEERemoteThread(void *arg);
  //  zmq::socket_t* ThreadQuit;
  
  int m_num_FEEs;
  std::vector<pthread_t> m_threads;
  std::vector<FEE_args*> m_args;
  bool m_threads_running;


};


#endif
