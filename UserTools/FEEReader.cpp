#include "FEEReader.h"

FEEReader::FEEReader():Tool(){}


bool FEEReader::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  
  // ThreadQuit = new zmq::socket_t(*(m_data->context), ZMQ_PUB);
  m_threads_running=true;

  m_variables.Get("NumberFEEs",m_num_FEEs);
  
  // pthread_attr_t  attrs;
  //  pthread_attr_init(&attrs);
  // pthread_attr_setstacksize(&attrs, 65536);

  m_data->m_FEEData=new std::vector<FEEData*>;
  
  for(int i=0; i<m_num_FEEs; i++){
    pthread_t id;
    FEEData* data=new FEEData(i);
    data->m_trignum=0;
    FEE_args* args=new FEE_args(m_data->context,i, m_data->m_FEEData,&m_threads_running);

    // int rc= pthread_create(&id, NULL, FEEReader::FEESimThread, args);
    int rc= pthread_create(&id, NULL, FEEReader::FEERemoteThread, args);

    if (rc){
      std::cout << "Error:unable to create thread," << rc <<" : "<<i<< std::endl;
      exit(-1);
    }

    m_threads.push_back(id);
    m_args.push_back(args);
    m_data->m_FEEData->push_back(data);
    usleep(100);
  }

  srand (time(NULL));

  m_data->InfoTitle="FEEReader";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();


  sleep(2);
  

  return true;
}


bool FEEReader::Execute(){

  if (m_data->m_triggered){
  bool received=false;

  int wrong=0;
  while(!received){
    
    bool ret=true;

    for(int i=0;i<m_num_FEEs;i++){
      //  std::cout<<i<<" : "<<m_data->m_FEEData.at(i)->m_trignum<<" : "<<m_data->m_trigger_num<<std::endl;
      ret=ret*(m_data->m_trigger_num==m_data->m_FEEData->at(i)->m_trignum);
	       // ||m_data->m_FEEData->at(i)->m_trignum ==0 );
      if(!ret){
	ret=true;
	wrong++;
      }
    }
    //std::cout<<"wrong="<<wrong<<" : trignum="<<m_data->m_trigger_num<< " : "<<m_data->m_FEEData.at(0)->m_trignum<<std::endl;
    if(wrong<=0)received=ret;
    wrong=0;
  }
  // std::cout<<"wrong="<<wrong<<" : trignum="<<m_data->m_trigger_num<< " : "<<m_data->m_FEEData->at(0)->m_trignum<<std::endl;
   
  }



  return true;
}


bool FEEReader::Finalise(){

  //std::cout<<"sending Quit"<<std::endl;
  m_threads_running=false;
  //zmq::message_t message(1);
  //snprintf ((char *) message.data(),1 , "%s" ,"1") ;
  //ThreadQuit->send(message);
  //snprintf ((char *) message.data(),1 , "%s" ,"1") ;
  //ThreadQuit->send(message);
  //snprintf ((char *) message.data(),1 , "%s" ,"1") ;
  //ThreadQuit->send(message);
  //std::cout<<"Quit sent"<<std::endl;

  //std::cout<<"joining threads"<<std::endl;
  for (int i=0;i<m_threads.size();i++)pthread_join(m_threads.at(i), NULL);
  m_threads.clear();

  //std::cout<<" threads joint"<<std::endl;

  for (int i=0;i<m_args.size();i++){
    delete m_args.at(i);
    m_args.at(i)=0;
  }

  m_args.clear();

  for (int i=0;i<m_num_FEEs;i++){
    delete m_data->m_FEEData->at(i);
    m_data->m_FEEData->at(i)=0;
  }
  m_data->m_FEEData->clear();
  delete m_data->m_FEEData;
  m_data->m_FEEData=0;


  //  delete ThreadQuit;
  //ThreadQuit=0;

  return true;
}

void* FEEReader::FEESimThread(void *arg) {

  FEE_args* args= static_cast<FEE_args*>(arg);
  
  //  zmq::socket_t Quit(*(args->context), ZMQ_SUB);
  //Quit.connect("inproc://FEEThreadQuit");
  //Quit.setsockopt(ZMQ_SUBSCRIBE,"",0);

  zmq::socket_t Triggersock(*(args->context), ZMQ_SUB);
  //  Triggersock.connect("inproc://Trigger");
  Triggersock.connect("tcp://localhost:55555");
  Triggersock.setsockopt(ZMQ_SUBSCRIBE,"",0);

  zmq::pollitem_t items [] = {
    //  { Quit, 0, ZMQ_POLLIN, 0 },
    { Triggersock, 0, ZMQ_POLLIN, 0 },
  };

  std::vector<FEEData*>* data=args->data;

  bool* running=args->running;  
  int FEEid=args->FEEid; 
  data->at(FEEid)->m_trignum=0;
  

  while(*running){

    zmq::poll(&items[0], 1, 100); 

    // if (items [0].revents & ZMQ_POLLIN) running=false;

    if (items [0].revents & ZMQ_POLLIN){
      zmq::message_t msg;
      Triggersock.recv(&msg);

      data->at(FEEid)->m_trignum=*(reinterpret_cast<unsigned long*>(msg.data()));

      
      int numhits= (rand() %53);
      data->at(FEEid)->Hits.clear();
      
      for(int i=0;i<numhits;i++){
	char tmp;
	tmp=(char)(rand() % 10);
	data->at(FEEid)->Hits.push_back(tmp);
	tmp=(char)(rand() % 10);
	data->at(FEEid)->Hits.push_back(tmp);
	tmp=(char)(rand() % 10);
	data->at(FEEid)->Hits.push_back(tmp);
      }
      ///generate data
   
    }



  
 
  }
    pthread_exit(NULL);
 
}


void* FEEReader::FEERemoteThread(void *arg) {

  FEE_args* args= static_cast<FEE_args*>(arg);
  int FEEid=args->FEEid; 

  std::stringstream address;
  address<<"tcp://*:"<<(60000+FEEid);
  zmq::socket_t FEE(*(args->context), ZMQ_DEALER);
  FEE.bind(address.str().c_str());
  

  zmq::socket_t Triggersock(*(args->context), ZMQ_SUB);
  //  Triggersock.connect("inproc://Trigger");
  Triggersock.connect("tcp://localhost:55555");
  Triggersock.setsockopt(ZMQ_SUBSCRIBE,"",0);

  zmq::pollitem_t items [] = {
    { Triggersock, 0, ZMQ_POLLIN, 0 },
  };

  std::vector<FEEData*>* data=args->data;

  bool* running=args->running;  
 
  data->at(FEEid)->m_trignum=0;
  

  while(*running){

    zmq::poll(&items[0], 1, 100); 

    // if (items [0].revents & ZMQ_POLLIN) running=false;

    if (items [0].revents & ZMQ_POLLIN){

     
      zmq::message_t msg;
      Triggersock.recv(&msg);

      data->at(FEEid)->m_trignum=*(reinterpret_cast<unsigned long*>(msg.data()));

      //     std::cout<<"sending trig"<<std::endl;
      zmq::message_t msg2(1);
      FEE.send(msg2);
      //std::cout<<"sent"<<std::endl;

      
      zmq::message_t rep;
      FEE.recv(&rep);
      
      int size=0;
      //      std::cout<<rep.data()<<" : "<<*((int*)(rep.data()))<<std::endl;
      size=*(reinterpret_cast<int*>(rep.data()));

      //printf("received numhits=%i \n",size);

      if (size>60) exit(0);     
      zmq::message_t rep2;
      FEE.recv(&rep2);
      
      //Data=new uint16_t[message.size()/(sizeof(uint16_t))];
      //std::memcpy(&Data[0], message.data(), message.size());
      // delete data->at(FEEid)->Hits;
      // data->at(FEEid)->Hits=0;
      //data->at(FEEid)->Hits=new std::vector<char> (rep.data(), rep.size()/ sizeof char );
      
      //  printf("received numhits=%i \n",size);
      
      
      data->at(FEEid)->Hits.clear();
      char* tmp=(reinterpret_cast<char*>(rep2.data()));
      // if(size<60){
	data->at(FEEid)->Hits.assign(tmp, tmp+size);
	
	// }
	//else size=0;
	//      if(size>0) printf("received numhits=%i  hit 0=%c\n",size,data->at(FEEid)->Hits.at(0));
      
      /*
      zmq::pollitem_t test [] = {
	{ FEE, 0, ZMQ_POLLIN, 0 },
      };

      for(int j=0;j<10;j++){
      zmq::poll(&test[0], 1, 10);

      if (test[0].revents & ZMQ_POLLIN){

	zmq::message_t rep;
	FEE.recv(&rep);
      }
      }
      */

      //if(size>0) std::cout<<"received numhits="<<size<<" hit 0="<<data->at(FEEid)->Hits.at(0)<<std::endl;
      /*
      int numhits= (rand() %53);
      data->at(FEEid)->Hits.clear();
      
      for(int i=0;i<numhits;i++){
	char tmp;
	tmp=(char)(rand() % 10);
	data->at(FEEid)->Hits.push_back(tmp);
	tmp=(char)(rand() % 10);
	data->at(FEEid)->Hits.push_back(tmp);
	tmp=(char)(rand() % 10);
	data->at(FEEid)->Hits.push_back(tmp);
      }
      ///generate data
      */
    }



  
 
  }
    pthread_exit(NULL);
 
}
