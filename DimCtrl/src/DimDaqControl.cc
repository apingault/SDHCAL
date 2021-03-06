
#include "DimDaqControlServer.h"
#include "DimDaqControl.h"
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dir.h>  
#include <sys/param.h>  
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "ShmProxy.h"
#include <string.h>



DimDaqControl::DimDaqControl(std::string dns) :theDNS_(dns)  
{
 
}
DimDaqControl::~DimDaqControl()
{
 
  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    delete it->second;
}

void DimDaqControl::scandns()
{
  // Look for DB server
  DimBrowser* dbr=new DimBrowser(); 
  char *service, *format; 
  int type;
  // Get DB service
  cout<<"On rentre dans scandns "<<endl;

 char *server,*node;
  dbr->getServers( ); 
  cout<<"On sort \n";
  while(dbr->getNextServer(server, node)) 
    { 
      cout << server << " @ " << node << endl; 
    }
  cout<<"0"<<endl;
  dbr->getServices("/DB/*/DOWNLOAD" ); 
  cout<<"1\n";
  while(type = dbr->getNextService(service, format)) 
    { 
      cout << service << " -  " << format << endl; 
      std::string ss;
      ss.assign(service);
      size_t n=ss.find("/DOWNLOAD");
      cout<<ss.substr(0,n)<<endl;
      theDBPrefix_=ss.substr(0,n);
    } 
  // Get the CCC prefix
  cout<<"2\n";
  dbr->getServices("/DCS/*/STATUS" ); 
  while(type = dbr->getNextService(service, format)) 
    { 
      cout << service << " -  " << format << endl; 
      std::string ss;
      ss.assign(service);
      size_t n=ss.find("/STATUS");
      theCCCPrefix_=ss.substr(0,n);
    } 

  theWriterPrefix_="";
  dbr->getServices("/DSP/*/STATUS" ); 
  while(type = dbr->getNextService(service, format)) 
    { 
      cout << service << " -  " << format << endl; 
      std::string ss;
      ss.assign(service);
      size_t n=ss.find("/STATUS");
      theWriterPrefix_=ss.substr(0,n);

 
    } 

  theZupPrefix_="";
  dbr->getServices("/DZUP/*/STATUS" ); 
  while(type = dbr->getNextService(service, format)) 
    { 
      cout << service << " -  " << format << endl; 
      std::string ss;
      ss.assign(service);
      size_t n=ss.find("/STATUS");
      theZupPrefix_=ss.substr(0,n);
    } 
  std::string theProxyPrefix_="";
  dbr->getServices("/DSP/*/EVENT" ); 
  while(type = dbr->getNextService(service, format)) 
    { 
      cout << service << " -  " << format << endl; 
      std::string ss;
      ss.assign(service);
      size_t n=ss.find("/EVENT");
      theProxyPrefix_=ss;
    } 

  dbr->getServers( ); 
  while(dbr->getNextServer(server, node)) 
    { 
      cout << server << " @ " << node << endl; 
      


      if (strncmp(server,"DimDIFServer",12)!=0) continue;
      std::string ss;
      ss.assign(node);
      size_t n=ss.find(".");
      std::string toto;
      toto=ss.substr(0,n);

      std::stringstream s0;
      s0<<"/DDS/"<<toto;
      DimDDSClient* d=new DimDDSClient(toto,s0.str());
      std::pair<std::string,DimDDSClient*> p(toto,d);
      theDDSCMap_.insert(p);
      
    }
      
  runInfo_=new DimInfo("/DB/RUNFROMDB",theCurrentRun_,this);
  dbstateInfo_=new DimInfo("/DB/DBSTATE",theCurrentState_,this);
  eventInfo_=new DimInfo(theProxyPrefix_.c_str(),theCurrentEvent_,this);

}
int DimDaqControl::getCurrentRun()
{ return theCurrentRun_;}
int DimDaqControl::getCurrentEvent()
{ return theCurrentEvent_;}
char* DimDaqControl::getCurrentState()
{return theCurrentState_;}
void DimDaqControl::infoHandler()
{
   DimInfo *curr = getInfo(); // get current DimInfo address 
   if (curr->getSize()==1) return;
   if (curr==runInfo_)
     {
       theCurrentRun_=curr->getInt();
       //std::cout<<"The current Run is "<<theCurrentRun_<<std::endl;
       return;
     }
   if (curr==eventInfo_)
     {
       theCurrentEvent_=curr->getInt();
       //std::cout<<"The current Run is "<<theCurrentRun_<<std::endl;
       return;
     }
   if (curr==dbstateInfo_)
     {

       strcpy(theCurrentState_,curr->getString());
       //std::cout<<"The current DbState is "<<theCurrentState_<<std::endl;
       return;
     }

}

void DimDaqControl::initialiseWriter(std::string dir)
{
  std::stringstream s0;
  
  s0.str(std::string());
  s0<<theWriterPrefix_<<"/DESTROY";
  DimClient::sendCommand(s0.str().c_str(),(int) 1);
  sleep((unsigned int) 1);
  s0.str(std::string());
  s0<<theWriterPrefix_<<"/INITIALISE";
  DimClient::sendCommand(s0.str().c_str(),(int) 1);
  sleep((unsigned int) 1);
  s0.str(std::string());
  s0<<theWriterPrefix_<<"/DIRECTORY";
  DimClient::sendCommand(s0.str().c_str(),dir.c_str());

}
void DimDaqControl::download(std::string state)
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theDBPrefix_<<"/DELETE";
  DimClient::sendCommand(s0.str().c_str(),state.c_str());
  sleep((unsigned int) 1);
  s0.str(std::string());
  s0<<theDBPrefix_<<"/DOWNLOAD";
  DimClient::sendCommand(s0.str().c_str(),state.c_str());
  sleep((unsigned int) 5);
}

void DimDaqControl::initialiseZup(std::string device,int port)
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theZupPrefix_<<"/OPEN";
  DimClient::sendCommand(s0.str().c_str(),device.c_str()); 

}

void DimDaqControl::on()
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theZupPrefix_<<"/ON";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 

}
void DimDaqControl::off()
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theZupPrefix_<<"/OFF";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 

}
void DimDaqControl::print()
{
  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      it->second->print();
    }
}

void DimDaqControl::getDifInfo()
{
  _nd_=0;
  
  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      std::map<uint32_t,DimDIFDataHandler*> difmap=it->second->getDIFMap();
      std::string pc=it->first;
      for (std::map<uint32_t,DimDIFDataHandler*>::iterator id=difmap.begin();id!=difmap.end();id++)
	{
      //printf("\t DIF %d %x %s \n",it->first,it->second,it->second->getState());
	  DIFStatus& s=id->second->getStatus();
	  printf(" %d %d %x %d %lld %lld %s \n",s.id,s.status,s.slc,s.gtc,s.bcid,s.bytes,id->second->getState()); 
	  _dif_[_nd_]=s.id;
	  _slc_[_nd_]=s.slc;
	  _state_[_nd_]=id->second->getState();
	  _host_[_nd_]=it->first;
	  _gtc_[_nd_]=s.gtc;
	  _bcid_[_nd_]=s.bcid;
	  _bytes_[_nd_]=s.bytes;
	  _nd_++;
	}
    }
}



void DimDaqControl::doScan(DimDDSClient* c)
{
  // cout <<"Calling scan \n";
  c->scanDevices();
}
void DimDaqControl::scan()
{
  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doScan, this,it->second));
    }
  g.join_all();
	
}
void DimDaqControl::doInitialise(DimDDSClient* c)
{
  c->initialise();
}
void DimDaqControl::initialise()
{

  std::stringstream s0;
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/INITIALISE";
  DimClient::sendCommand(s0.str().c_str(),"DCCCCC01"); 
  //  bsem_.lock();
  // theCCCClient_->doInitialise("DCCCCC01");
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/CONFIGURE";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/STOP";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 
  sleep((uint32_t) 1);
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/CCCRESET";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 


  sleep((uint32_t) 1);
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/DIFRESET";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 



  sleep((uint32_t) 1);
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/CCCRESET";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 

  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doInitialise, this,it->second));
    }
  g.join_all();
	
}
void DimDaqControl::doRegisterstate(DimDDSClient* c)
{
  c->setDBState(theCtrlReg_,theState_);
}
void DimDaqControl::registerstate(uint32_t ctr,std::string sta)
{
  theState_=sta;
  theCtrlReg_=ctr;
  //std::cout<<"SETTING DB "<<theState_<<" "<<theCtrlReg_<<std::endl;
  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doRegisterstate, this,it->second));
    }
  g.join_all();
}
void DimDaqControl::doSetGain(DimDDSClient* c)
{
  c->setGain(theCalibrationGain_);
}
void DimDaqControl::setGain(uint32_t ga)
{
  theCalibrationGain_=ga;

  //std::cout<<"SETTING DB "<<theState_<<" "<<theCtrlReg_<<std::endl;
  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doSetGain, this,it->second));
    }
  g.join_all();
}

void DimDaqControl::doSetThresholds(DimDDSClient* c)
{
  c->setThresholds(theCalibrationThresholds_[0],theCalibrationThresholds_[1],theCalibrationThresholds_[2]);
}
void DimDaqControl::setThresholds(uint32_t b0,uint32_t b1,uint32_t b2)
{
  theCalibrationThresholds_[0]=b0;
  theCalibrationThresholds_[1]=b1;
  theCalibrationThresholds_[2]=b2;

  //std::cout<<"SETTING DB "<<theState_<<" "<<theCtrlReg_<<std::endl;
  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doSetThresholds, this,it->second));
    }
  g.join_all();
}

void DimDaqControl::doConfigure(DimDDSClient* c)
{
  c->configure();
}
void DimDaqControl::configure()
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theCCCPrefix_<<"/CCCRESET";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 


  sleep((uint32_t) 1);
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/DIFRESET";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 



  sleep((uint32_t) 1);
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/CCCRESET";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 
  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doConfigure, this,it->second));
    }
  g.join_all();
	
}
void DimDaqControl::doStart(DimDDSClient* c)
{
  c->start();
}
void DimDaqControl::start()
{
  // Register a new run
  DimClient::sendCommand("/DB/NEWRUN",(int) 1);

  sleep((unsigned int) 5);
  boost::thread_group g;
  int32_t ndif=0;
  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      std::map<uint32_t,DimDIFDataHandler*>& m=it->second->getDIFMap();
      for (std::map<uint32_t,DimDIFDataHandler*>::iterator id=m.begin();id!=m.end();id++)
	{
	  ndif++;
	}
      cout<<"=====================================>"<<it->first<<" "<<ndif<<endl;
      g.create_thread(boost::bind(&DimDaqControl::doStart, this,it->second));
    }
  g.join_all();


  // Start Writing operation
  std::stringstream s0;

  s0.str(std::string());
  s0<<theWriterPrefix_<<"/START";
  DimClient::sendCommand(s0.str().c_str(),ndif); 


  // Start CCC
  s0.str(std::string());
  s0<<theCCCPrefix_<<"/START";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 	


}

void DimDaqControl::doStop(DimDDSClient* c)
{

  c->stop();
}
void DimDaqControl::stop()
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theCCCPrefix_<<"/STOP";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 	

  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doStop, this,it->second));
    }
  g.join_all();
	

  // Stop Writing operation


  s0.str(std::string());
  s0<<theWriterPrefix_<<"/STOP";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 


}

void DimDaqControl::pause()
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theCCCPrefix_<<"/PAUSE";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 	
}

void DimDaqControl::resume()
{
  std::stringstream s0;

  s0.str(std::string());
  s0<<theCCCPrefix_<<"/RESUME";
  DimClient::sendCommand(s0.str().c_str(),(int) 1); 	
}




void DimDaqControl::doDestroy(DimDDSClient* c)
{
  c->destroy();
}
void DimDaqControl::destroy()
{
  boost::thread_group g;

  for (std::map<std::string,DimDDSClient*>::iterator it=theDDSCMap_.begin();it!=theDDSCMap_.end();it++)
    {
      g.create_thread(boost::bind(&DimDaqControl::doDestroy, this,it->second));
    }
  g.join_all();
	
}

/*
void DimDaqControl::commandHandler()
{
  DimCommand *currCmd = getCommand();
  printf(" J'ai recu %s COMMAND  \n",currCmd->getName());
  if (currCmd==browseCommand_)
    {
      this->scandns();
      processStatus_=DimDaqControl::BROWSED;
      aliveService_->updateService();
      return ;

    }

  if (currCmd==scanCommand_)
    {
      this->scan();
      processStatus_=DimDaqControl::SCANNED;
      aliveService_->updateService();
      return ;

    }
  if (currCmd==initialiseCommand_)
    {
      this->initialise();
      processStatus_=DimDaqControl::INITIALISED;
      aliveService_->updateService();
      return ;

    }

  if (currCmd==registerstateCommand_)
    {
      char* data= (char*) currCmd->getData();
      memcpy(&theCtrlReg_,data,sizeof(uint32_t));
      char* cdata= &data[4];
      theState_.assign(cdata);
      this->registerstate();
      processStatus_=DimDaqControl::DBREGISTERED;
      aliveService_->updateService();
      return ;

    }
  if (currCmd==configureCommand_)
    {
      this->configure();
      processStatus_=DimDaqControl::CONFIGURED;
      aliveService_->updateService();
      return ;

    }
  if (currCmd==startCommand_)
    {
      this->start();
      processStatus_=DimDaqControl::STARTED;
      aliveService_->updateService();
      return ;

    }
  if (currCmd==stopCommand_)
    {
      this->stop();
      processStatus_=DimDaqControl::STOPPED;
      aliveService_->updateService();
      return ;

    }
  if (currCmd==destroyCommand_)
    {
      this->destroy();
      processStatus_=DimDaqControl::DESTROYED;
      aliveService_->updateService();
      return ;

    }
  cout<<"Unknown command \n";
    
  return ;
}
   
*/
