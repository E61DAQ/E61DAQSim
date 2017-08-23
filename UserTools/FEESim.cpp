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

 
  zmq::pollitem_t items [] = {
    { *sock, 0, ZMQ_POLLIN, 0 },
  };
  
  zmq::poll(&items[0], 1, 100);

  if (items [0].revents & ZMQ_POLLIN){
  
    //  std::cout<<"waiting to receive"<<std::endl;
    zmq::message_t rec;
    sock->recv(&rec);
    // std::cout<<"received"<<std::endl;
    
    int *numhits= new int(rand() %53);
    unsigned int *data=new unsigned int[(*numhits)*6];
    
    for(int i=0;i<(*numhits)*6;i++){
      
      data[i]=(rand() % 9999) + 46;
      
    }
    
  //std::cout<<"sent numhits="<<(*numhits)<<" hit[0]="<<data[0]<<std::endl;
    
    zmq::message_t ms2(&data[0], sizeof(unsigned int)*(*numhits)*6, NULL);
    
    
    zmq::message_t ms1(numhits,sizeof(int), NULL);
    //zmq::message_t ms1(sizeof(unsigned int));
    
    //memcpy(ms1.data(), &numhits, sizeof(unsigned int));
    
    //std::cout<<"sent numhits="<<(*numhits)<<" hit[0]="<<data[0]<<std::endl;
    
    sock->send(ms1,ZMQ_SNDMORE);
    sock->send(ms2);
    
    
    
    //std::cout<<"sent numhits="<<(*numhits)<<" hit[0]="<<data[0]<<std::endl;

    }
    return true;
}


bool FEESim::Finalise(){

  delete sock;
  sock=0;

  return true;
}
