#include "Trigger.h"

Trigger::Trigger():Tool(){}


bool Trigger::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;

  m_data->m_triggered=false;
  m_data->m_trigger_num=0;

  m_variables.Get("TriggerRate",m_trigger_rate);
  m_trigger_period=floor((1.0/m_trigger_rate)*1000000);
  m_last_trigger=boost::posix_time::microsec_clock::local_time();

  TriggerSend = new zmq::socket_t(*(m_data->context), ZMQ_PUB);
  // TriggerSend->bind("inproc://Trigger");
  TriggerSend->bind("tcp://*:55555");

  m_data->InfoTitle="Trigger";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();


  return true;
}


bool Trigger::Execute(){

  if (!m_data->m_triggered){
    boost::posix_time::ptime current_time(boost::posix_time::microsec_clock::local_time());
    
    boost::posix_time::time_duration duration(current_time - m_last_trigger);
    
    if(duration.total_microseconds()>m_trigger_period){
      std::cout<<"time "<<duration.total_microseconds()<<" "<<m_trigger_period<<std::endl;
      
      m_data->m_triggered=true;
      m_data->m_trigger_num++;
      // unsigned long tmptrignum=m_data->m_trigger_num;
      
      //std::cout<<"set "<<m_data->m_trigger_num<<std::endl;
      zmq::message_t message(& m_data->m_trigger_num,sizeof  m_data->m_trigger_num, NULL);
      TriggerSend->send(message);
      //   TriggerSend->send(message);
      m_last_trigger=boost::posix_time::microsec_clock::local_time();

      std::stringstream eventstatus;
      eventstatus<<"Trigger events ="<<  m_data->m_trigger_num;
      m_data->vars.Set("Status", eventstatus.str());
      
    }
  }
  
  return true;
}


bool Trigger::Finalise(){

 m_data->m_triggered=false;

 delete TriggerSend;
 TriggerSend=0;
 
  return true;
}
