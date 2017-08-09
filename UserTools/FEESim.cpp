#include "FEESim.h"

FEESim::FEESim():Tool(){}


bool FEESim::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  int port=0;
  m_variables.Get("Port",port);

  std::stringstream tmp;
  tmp<<"tcp://192.168.111.2:"<<port;
  
  sock=new zmq::socket_t (*(m_data->context), ZMQ_DEALER);
  sock->connect(tmp.str().c_str());

  return true;
}


bool FEESim::Execute(){

  zmq::message_t rec;
  sock->recv(&rec);

  int numhits= (rand() %53);
  char data[numhits*3];
  
  for(int i=0;i<numhits*3;i++){
    
    data[i]=(char)((rand() % 50) + 46);
    
  }

  std::cout<<"sent numhits="<<numhits<<" hit[0]="<<data[0]<<std::endl;
  
  zmq::message_t ms2(&data[0], sizeof data, NULL);
  
  
  zmq::message_t ms1(&numhits,sizeof numhits, NULL);

    std::cout<<"sent numhits="<<numhits<<" hit[0]="<<data[0]<<std::endl;
  
    sock->send(ms1);//,ZMQ_SNDMORE);
    sock->send(ms2);

 
    
    std::cout<<"sent numhits="<<numhits<<" hit[0]="<<data[0]<<std::endl;
  return true;
}


bool FEESim::Finalise(){

  delete sock;
  sock=0;

  return true;
}
