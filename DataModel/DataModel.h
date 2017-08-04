#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <map>
#include <string>
#include <vector>

#include "TTree.h"

#include "Store.h"
#include "Logging.h"

#include <zmq.hpp>

#include <FEEData.h>

class DataModel {


 public:
  
  DataModel();
  TTree* GetTTree(std::string name);
  void AddTTree(std::string name,TTree *tree);
  void DeleteTTree(std::string name);

  Store vars;
  Logging *Log;

  zmq::context_t* context;

  bool m_triggered;
  unsigned long m_trigger_num;
  std::vector<FEEData*>* m_FEEData;

  std::string InfoTitle;
  std::string InfoMessage;

  long RunNumber;
  long SubRunNumber;
  int RunType;




 private:

  std::map<std::string,TTree*> m_trees; 
  
  
  
};



#endif
