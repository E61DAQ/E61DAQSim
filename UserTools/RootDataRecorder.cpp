#include "RootDataRecorder.h"

RootDataRecorder::RootDataRecorder():Tool(){}


bool RootDataRecorder::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;
  
  //  m_variables.Get("TTreeEventCap",m_TTreeEventCap); 
  m_variables.Get("TTreeTriggerCap",m_TTreeTriggerCap); 
  m_variables.Get("TFileTTreeCap",m_TFileTTreeCap);
  m_variables.Get("OutputPath",OutputPath);

  m_filepart=0;  
  std::stringstream tmp;
  tmp<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber;
  std::string filename=tmp.str();
  tmp.str("");
  tmp<<OutputPath<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber;
  OutFile=tmp.str();
  tmp<<"p0.root";
  TFile file(tmp.str().c_str(),"RECREATE","",1);
  file.Write();
  file.Close();
  
  //std::cout<< "outfile = "<<OutFile<<std::endl;
  
  Isend = new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  //std::cout<<"attempting bind"<<std::endl;
  Isend->bind("inproc://RootWriter");
  //std::cout<<"bound"<<std::endl;
  
  args=new rootw_thread_args(OutFile, filename, m_data->context,m_TFileTTreeCap, m_filepart,m_data->GetTTree("RunInformation"));
  
  h1 = new TThread("h1", RootDataRecorder::RootWriter, args);
  h1->Run();
  
  m_treenum=0;
  //  m_data->NumEvents=0;
  //std::cout<<"i d1 "<<std::endl;
  // std::cout<<m_data->carddata.at(0)->channels<<std::endl;

  //  m_card.PMTID=new int[4];
  //std::cout<<"i d2"<<std::endl;  

  //  m_card.Data=new uint16_t[160000];
  //m_card.Rates=new uint32_t[160000];
  //m_card.triggerCounts=new uint64_t[160000];

  //std::cout<<"i d3"<<std::endl;
  TTree *tree = new TTree("PMTData","PMTData");
  TBranch *br;
  //std::cout<<"i d4"<<std::endl;
  br=tree->Branch("CardID",&CardID,"CardID/I");
  br->SetCompressionLevel(1);
  br=tree->Branch("TrigNum",&TrigNum,"TrigNum/l");
  br->SetCompressionLevel(1);  
  br=tree->Branch("NumHits",&NumHits,"NumHits/I");
  br->SetCompressionLevel(1);  
  br=tree->Branch("Hits",hitsconv,"Hits[NumHits]/B");
  br->SetCompressionLevel(1);  
  m_data->AddTTree("PMTData",tree);


  m_data->InfoTitle="RootDataRecorderVariables";
  m_variables>>m_data->InfoMessage;
  m_data->GetTTree("RunInformation")->Fill();


  return true;
}


bool RootDataRecorder::Execute(){
  // if(m_data->Restart==1)Finalise();
  // else if(m_data->Restart==2)Initialise("",*m_data);
  // else{
  //std::cout<<"Debug 1 i="<<m_data->NumEvents<<std::endl;
  if(m_data->m_triggered){

    //    boost::progress_timer t;

    //m_data->NumEvents++;
 
  TTree *tree=m_data->GetTTree("PMTData");
  //std::cout<<"Debug 2"<<std::endl;

  //  if (tree->GetEntriesFast()>m_TTreeEventCap){
  if (!(m_data->m_trigger_num % m_TTreeTriggerCap)){
    //std::cout<<"Debug 3"<<std::endl;
    //    root_thread_args *args=new root_thread_args(OutFile,tree);
    //std::cout<<"Debug 4 "<<tree<<std::endl;
     
    std::stringstream TTreepointer;
    TTreepointer<<"TTree "<<tree;
    
    //std::cout<<"sending "<<TTreepointer.str()<<std::endl;
    zmq::message_t message(TTreepointer.str().length()+1);
    snprintf ((char *) message.data(), TTreepointer.str().length()+1 , "%s" ,TTreepointer.str().c_str()) ;
    Isend->send(message);

    //m_data->trigdata->tree->GetEntry(0);
    //std::cout<<"testing event size = "<<m_data->trigdata->EventSize<<std::endl;
   
    //std::cout<<"sent "<<std::endl;
    //    pthread_create (&thread[0], NULL, RootDataRecorder::WriteOut, args
    //std::cout<<"Debug 5"<<std::endl;
    m_treenum++;
    std::stringstream tmp;
    // tmp<<"PMTData"<<m_treenum;
    tmp<<"PMTData";
    //std::cout<<"Debug 6 m_tmp = "<<tmp.str()<<std::endl;
    tree=new TTree(tmp.str().c_str(),tmp.str().c_str());
    //std::cout<<"Debug 7"<<std::endl;
    m_data->AddTTree("PMTData",tree);
    //std::cout<<"Debug 8"<<std::endl;
    TBranch *br;
 
    br=tree->Branch("CardID",&CardID,"CardID/I");
    br->SetCompressionLevel(1);
    br=tree->Branch("TrigNum",&TrigNum,"TrigNum/l");
    br->SetCompressionLevel(1);  
    br=tree->Branch("NumHits",&NumHits,"NumHits/I");
    br->SetCompressionLevel(1);  
    br=tree->Branch("Hits",hitsconv,"Hits[NumHits]/B");
    br->SetCompressionLevel(1);  

  }


  //std::cout<<"Debug 10 "<<m_data->carddata.size()<<std::endl;
  for(int i=0;i<m_data->m_FEEData->size();i++){
    //std::cout<<"Debug 11"<<std::endl;
    CardID=m_data->m_FEEData->at(i)->m_id;
    TrigNum=m_data->m_FEEData->at(i)->m_trignum;
    //std::cout<<"trignum= "<< TrigNum<<" : "<<m_data->m_FEEData->at(i)->m_trignum<<std::endl;
    //its.clear();
    // Hits.insert( Hits.begin(), m_data->m_FEEData->at(i)->Hits.begin(), m_data->m_FEEData->at(i)->Hits.end() );
    //NumHits=Hits.size()*3;
    NumHits=(m_data->m_FEEData->at(i)->Hits.size());
    hitsconv=&m_data->m_FEEData->at(i)->Hits[0];
    tree->SetBranchAddress("Hits",hitsconv);

    // for(int j=0;j<m_data->m_FEEData->at(i)->Hits.size();j++){
      //    strcat(hitsconv,m_data->m_FEEData->at(i)->Hits.at(j));
      // }
    //  memcpy(m_card.triggerCounts, m_data->carddata.at(i)->triggerCounts, m_card.triggerNumber*sizeof(uint64_t));
    // memcpy(m_card.Rates, m_data->carddata.at(i)->Rates, m_card.triggerNumber*sizeof(uint32_t));
    //m_card.triggerCounts=m_data->carddata.at(i)->triggerCounts;
    //m_card.Rates=m_data->carddata.at(i)->Rates;

    
    //    memcpy(m_card.Data, m_data->carddata.at(i)->Data, m_card.fullbuffsize*(sizeof(uint16_t)) );
    //m_card.Data=m_data->carddata.at(i)->Data;


    /*
//std::cout<<"Debug 12"<<std::endl;
    for(int j=0;j<m_card.channels;j++){
      //std::cout<<"Debug 13"<<std::endl;

     (m_card.PMTID)[j]=(m_data->carddata.at(i)->PMTID)[j]; // these pointers have changed hence the crash
     //std::cout<<"Debug 14"<<std::endl;
      
    }	
    //std::cout<<"Debug 15"<<std::endl;
    
    for(int j=0;j<m_card.fullbuffsize;j++){
      //std::cout<<"Debug 16"<<std::endl;
      
      (m_card.Data[j])=(m_data->carddata.at(i)->Data)[j];
      //  std::cout<<m_data->carddata.at(i)->Data[j]<<" Debug 17 "<<m_card.Data[j]<<std::endl;

    }
    
    */
    //std::cout<<"Debug 18 i= "<<m_data->NumEvents<<std::endl;
     tree->Fill();
     //std::cout<<"Debug 19"<<std::endl;
  }    
  
  //std::cout<<"Debug 20"<<std::endl;
  
  ///////////////////////trigger data////////////////
  
  //std::cout<<"Filling tree event size = "<<m_data->trigdata->EventSize<<std::endl;
  
 m_data->m_triggered=false;
  }
  //  }

  return true;
}


bool RootDataRecorder::Finalise(){


  std::string send="Quit 0x00";
  zmq::message_t message(send.length()+1);
  //std::cout<<"sending "<<send<<std::endl;

  snprintf ((char *) message.data(), send.length()+1 , "%s" ,send.c_str()) ;
  Isend->send(message);
  //std::cout<<"sent quit "<<send<<std::endl;
  //TThread::Ps();
  h1->Join();
  //std::cout<<"joined "<<send<<std::endl;
  //  (void) pthread_join(thread[0], NULL);
  m_filepart++;  
  std::stringstream tmp;
  tmp<<OutFile<<"p"<<m_filepart<<".root";
  TFile file(tmp.str().c_str(),"UPDATE","",1);
  
  //TTree *treeorig=m_data->GetTTree("PMTData");
  //TTree *tree=treeorig->CloneTree(); 
  TTree *tree=m_data->GetTTree("PMTData");  
  tree->Write();
  // treeorig=m_data->GetTTree("RunInformation");
  //tree=treeorig->CloneTree();
  tree=m_data->GetTTree("RunInformation");
  tree->Write();

  //std::cout<<"starting write of trigtree"<<std::endl;
  //m_data->trigdata->Write();

  //std::cout<<"written main trees "<<send<<std::endl;


  file.Write();
  file.Close();
  // std::stringstream compcommand;
  //compcommand<<"sleep 5 && tar -cf "<<tmp.str()<<".tar -C /data/output/ "<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber<<"p"<<m_filepart<<".root &";//" && rm "<<tmp.str()<<" &"; 
  ////std::cout<<compcommand.str()<<std::endl;
   
  //  system("sleep 10  &");
   // sleep(5);
  //std::cout<<"starting cleanup "<<send<<std::endl;

  m_data->DeleteTTree("PMTData");
  m_data->DeleteTTree("RunInformation");
  tree=0;


  //test remove //delete mrdtree;
  // test removemrdtree=0;

  //  Isend->close();

  // too delete properly 

  delete h1;
  h1=0;

  delete Isend;
  Isend=0;
  
  delete args;
  args=0;

  //  delete m_card.PMTID;
  // m_card.PMTID=0;
  
  //delete m_card.Data;
  //m_card.Data=0;
  //std::cout<<"finnished "<<send<<std::endl;
  return true;
}

/*
void* RootDataRecorder::WriteOut(void* arg){
  //  std::cout<<"Debug a1"<<std::endl;
  root_thread_args* args= static_cast<root_thread_args*>(arg);
  //std::cout<<"Debug a2"<<std::endl;

  std::cout<<args->OutFile<<std::endl;
  //  std::cout<<"Debug a3"<<std::endl;

  TFile file(args->OutFile.c_str(),"UPDATE");
  //  std::cout<<"Debug a4 "<<args->tree<<std::endl;

  args->tree->Write();
  //std::cout<<"Debug a5"<<std::endl;

  file.Write();
  //std::cout<<"Debug a6"<<std::endl;

  file.Close();
  //std::cout<<"Debug a7"<<std::endl;

  delete args->tree;
  //std::cout<<"Debug a8"<<std::endl;

  args->tree=0;
  std::cout<<"Debug a9"<<std::endl;
  
  return (NULL);
  
}

*/


void* RootDataRecorder::RootWriter(void* arg){
  
  rootw_thread_args* args= static_cast<rootw_thread_args*>(arg);
 
  zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
  //std::cout<<"attempting connect"<<std::endl; 
 Ireceive.connect("inproc://RootWriter");
 //std::cout<<"connected"<<std::endl;

  bool running=true;
  *(args->filepart)=0;
  int treenum=0;

  while (running){
    //std::cout<<"T  Debug 1"<<std::endl;
    zmq::message_t comm;
    //std::cout<<"T  Debug 2"<<std::endl;
    Ireceive.recv(&comm);
    
    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    long long unsigned int arg2;    
    //lon arg2="";
    TTree *tree;
    //    TTree *treeorig;    
    iss>>arg1>>std::hex>>arg2;
    
    //std::cout<<"arg1 = "<<arg1<<std::endl;
    //std::cout<<"arg2 = "<<arg2<<std::endl;
    //std::cout<<"T  Debug 3"<<std::endl;
    if (arg1=="TTree"){
      //std::cout<<"T  Debug 4"<<std::endl;
      treenum++;
     
      if(treenum>args->TFileTTreeCap){
	(*(args->filepart))+=1;
	treenum=1;	
      }
      //treeorig=reinterpret_cast<TTree *>(arg2);
      tree=reinterpret_cast<TTree *>(arg2); 
      std::stringstream tmp;
      tmp<<args->OutFile<<"p"<<*(args->filepart)<<".root";
      TFile file(tmp.str().c_str(),"UPDATE","",1);
      //std::cout<<"tree = "<<tree<<std::endl;
      //tree=treeorig->CloneTree();
      tree->Write();
      TTree *ri=args->runinformation->CloneTree();
      //TTree *ri=args->runinformation;
      ri->Write();
      //std::cout<<"saved waterpmt tree"<<std::endl;
   
   
      //std::cout<<"T  Debug 5"<<std::endl;
      file.Write();
      //std::cout<<"T Debug 6"<<std::endl;
      file.Close();
      //std::cout<<"T  Debug 7"<<std::endl;
      //delete test;
      // test=0;
      //std::cout<<"T  Debug 8"<<std::endl;
      //delete treeorig;

      // std::stringstream compcommand;
      //compcommand<<"tar -cf "<<tmp.str()<<".tar -C /data/output/ "<<args->filename<<"p"<<args->filepart<<".root &";//" && rm "<<tmp.str()<<" &";                                           
      //std::cout<<compcommand.str()<<std::endl;                                    
      // system(compcommand.str().c_str());
      
      //std::cout<<"T  Debug 7"<<std::endl;
      //sleep(5);
      //std::cout<<"T  Debug 8"<<std::endl;
      //test remove
      //delete tree;
      // ri->~TTree();
      //std::cout<<"T  Debug 9"<<std::endl;
      //test remove //tree=0;
      //std::cout<<"T  Debug 10"<<std::endl;
      //test remove //delete mrdtree;
      //test //remove mrdtree=0;
      
    }
    
    else if(arg1=="Quit"){
      running=false;
      //std::cout<<"T  Debug 12"<<std::endl;
    }
    //std::cout<<"T  Debug 13"<<std::endl;
  }
  //std::cout<<"T  Debug 14"<<std::endl;
  
  //std::cout<<"exiting thread"<<std::endl;
  //pthread_exit(NULL);
  //std::cout<<"exit"<<std::endl;
}
