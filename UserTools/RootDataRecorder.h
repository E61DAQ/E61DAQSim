#ifndef RootDataRecorder_H
#define RootDataRecorder_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Tool.h"
#include <zmq.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TThread.h"

#include <boost/progress.hpp>

struct rootw_thread_args{
  
  //root_thread_args(std::string outfile, TTree* intree){
  // OutFile=outfile;
  // tree=intree;
  //}

  rootw_thread_args(std::string outfile, std::string infilename, zmq::context_t* incontext, int inTFileTTreeCap, long &infilepart, TTree* inruninformation){
    
    OutFile=outfile;
    context=incontext;
    TFileTTreeCap=inTFileTTreeCap;
    filepart=&infilepart;
    filename=infilename;
    runinformation=inruninformation;
    
  }
  
  zmq::context_t* context;
  std::string OutFile;
  int TFileTTreeCap;
  long *filepart;
  std::string filename;
  TTree* runinformation;
 
};



class RootDataRecorder: public Tool {


 public:

  RootDataRecorder();
  bool Initialise(std::string configfile,DataModel &data);
  bool Execute();
  bool Finalise();


 private:

  static void *RootWriter(void* arg);

  zmq::socket_t *Isend;

  //  pthread_t thread[2];
  TThread *h1;
  rootw_thread_args *args;

  std::string OutputPath;
  std::string OutFile; //can be map
 
  //int m_TTreeEventCap;
  int m_TTreeTriggerCap;
  long m_treenum; // can make map in future if more trees needed
  int m_TFileTTreeCap;
  long m_filepart;
 
  //td::vector<char[3]> Hits;
  //char hitsconv[1000];
  char* hitsconv;
  int CardID;
  unsigned long TrigNum;
  int NumHits;

};


#endif
