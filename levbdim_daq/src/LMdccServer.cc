
#include "LMdccServer.hh"
#include "fileTailer.hh"

void LMdccServer::open(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());

  std::string device;
  if (m->content().isMember("device"))
    { 
      device=m->content()["device"].asString();
      this->parameters()["device"]=m->content()["device"];
    }
  else
    device=this->parameters()["device"].asString();

  doOpen(device);

  if (this->parameters().isMember("spillon") && _mdcc!=NULL)
    {
      _mdcc->setSpillOn(this->parameters()["spillon"].asInt()); 
    }
  if (this->parameters().isMember("spilloff") && _mdcc!=NULL)
    {
      _mdcc->setSpillOff(this->parameters()["spilloff"].asInt()); 
    }
  //_mdcc->maskTrigger();
  //_mdcc->resetCounter();
}
void LMdccServer::close(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       return;
    }
  _mdcc->close();
  _mdcc=NULL;
}
void LMdccServer::pause(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       return;
    }
  _mdcc->maskTrigger();
}
void LMdccServer::resume(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       return;
    }
  _mdcc->unmaskTrigger();
}
void LMdccServer::ecalpause(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       return;
    }
  _mdcc->maskEcal();
}
void LMdccServer::ecalresume(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       return;
    }
  _mdcc->unmaskEcal();
}
void LMdccServer::reset(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       return;
    }
  _mdcc->resetCounter();
}

void LMdccServer::c_joblog(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  uint32_t nlines=atol(request.get("lines","100").c_str());
  uint32_t pid=getpid();
  std::stringstream s;
  s<<"/tmp/dimjcPID"<<pid<<".log";
  std::stringstream so;
  fileTailer t(1024*512);
  char buf[1024*512];
  t.tail(s.str(),nlines,buf);
  so<<buf;
  response["STATUS"]="DONE";
  response["FILE"]=s.str();
  response["LINES"]=so.str();
}


void LMdccServer::c_pause(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->maskTrigger();
  response["STATUS"]="DONE";
}
void LMdccServer::c_resume(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->unmaskTrigger();
  response["STATUS"]="DONE";
}
void LMdccServer::c_ecalpause(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->maskEcal();
  response["STATUS"]="DONE";
}
void LMdccServer::c_ecalresume(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->unmaskEcal();
  response["STATUS"]="DONE";
}
void LMdccServer::c_calibon(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->calibOn();
  response["STATUS"]="DONE";
}
void LMdccServer::c_caliboff(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->calibOff();
  response["STATUS"]="DONE";
}
void LMdccServer::c_reloadcalib(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->reloadCalibCount();
  response["STATUS"]="DONE";
}

void LMdccServer::c_setcalibcount(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t nc=atol(request.get("nclock","5000000").c_str());
  _mdcc->setCalibCount(nc);

  response["STATUS"]="DONE";
  response["NCLOCK"]=nc;

} 

void LMdccServer::c_reset(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_mdcc==NULL)
    {
       LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
       response["STATUS"]="Please open MDC01 first";
       return;
    }
  _mdcc->resetCounter();
  response["STATUS"]="DONE";
}

void LMdccServer::c_readreg(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t adr=atol(request.get("address","2").c_str());
  uint32_t val =_mdcc->readRegister(adr);

  response["STATUS"]="DONE";
  response["ADDRESS"]=adr;
  response["VALUE"]=val;
} 
void LMdccServer::c_writereg(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t adr=atol(request.get("address","2").c_str());
  uint32_t value=atol(request.get("value","1234").c_str());
  _mdcc->writeRegister(adr,value);

  response["STATUS"]="DONE";
  response["ADDRESS"]=adr;
  response["VALUE"]=value;
} 
void LMdccServer::c_spillon(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t nc=atol(request.get("nclock","50").c_str());
  _mdcc->setSpillOn(nc);

  response["STATUS"]="DONE";
  response["NCLOCK"]=nc;

} 
void LMdccServer::c_spilloff(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t nc=atol(request.get("nclock","5000").c_str());
  _mdcc->setSpillOff(nc);

  response["STATUS"]="DONE";
  response["NCLOCK"]=nc;

} 
void LMdccServer::c_resettdc(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t nc=atol(request.get("value","0").c_str());
  _mdcc->resetTDC(nc&0xF);

  response["STATUS"]="DONE";
  //response["NCLOCK"]=nc;

} 

void LMdccServer::c_beamon(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t nc=atol(request.get("nclock","5000000").c_str());
  _mdcc->setBeam(nc);

  response["STATUS"]="DONE";
  response["NCLOCK"]=nc;

}
void LMdccServer::c_setspillregister(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t nc=atol(request.get("value","0").c_str());
  _mdcc->setSpillRegister(nc);

  response["STATUS"]="DONE";
  response["VALUE"]=nc;

} 
void LMdccServer::c_settrigext(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  uint32_t delay=atol(request.get("delay","20").c_str());
  uint32_t busy=atol(request.get("busy","20").c_str());
  _mdcc->setTriggerDelay(delay);
  _mdcc->setTriggerBusy(busy);

  response["STATUS"]="DONE";
  response["DELAY"]=delay;
  response["BUSY"]=busy;

} 

void LMdccServer::c_status(Mongoose::Request &request, Mongoose::JsonResponse &response)
{

  if (_mdcc==NULL)    {response["STATUS"]="NO Mdcc created"; return;}
  Json::Value rc;
  rc["version"]=_mdcc->version();
  rc["id"]=_mdcc->id();
  rc["mask"]=_mdcc->mask();
  rc["spill"]=_mdcc->spillCount();
  rc["busy1"]=_mdcc->busyCount(1);
  rc["busy2"]=_mdcc->busyCount(2);
  rc["busy3"]=_mdcc->busyCount(3);
  rc["busy4"]=_mdcc->busyCount(4);
  rc["busy5"]=_mdcc->busyCount(5);
  rc["busy6"]=_mdcc->busyCount(6);
  rc["busy7"]=_mdcc->busyCount(7);
  rc["busy8"]=_mdcc->busyCount(8);
  rc["busy9"]=_mdcc->busyCount(9);
  rc["busy10"]=_mdcc->busyCount(10);
  rc["busy11"]=_mdcc->busyCount(11);
  rc["busy12"]=_mdcc->busyCount(12);
  rc["spillon"]=_mdcc->spillOn();
  rc["spilloff"]=_mdcc->spillOff();
  rc["ecalmask"]=_mdcc->ecalmask();
  rc["beam"]=_mdcc->beam();
  rc["calib"]=_mdcc->calibCount();
  rc["spillreg"]=_mdcc->spillRegister();
  rc["trigdelay"]=_mdcc->triggerDelay();
  rc["trigbusy"]=_mdcc->triggerBusy();
  response["COUNTERS"]=rc;
  response["STATUS"]="DONE";


} 



void LMdccServer::cmd(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  std::string cmd_name=m->content()["name"].asString();

  
  
  if (_mdcc==NULL)
    {
      LOG4CXX_ERROR(_logLdaq,"Please open MDC01 first");
      return;
    }
  if (cmd_name.compare("STATUS")==0)
    {
      LOG4CXX_INFO(_logLdaq," execute: "<<cmd_name);
      Json::Value rc;
      rc["version"]=_mdcc->version();
      rc["id"]=_mdcc->id();
      rc["mask"]=_mdcc->mask();
      rc["spill"]=_mdcc->spillCount();
      rc["busy1"]=_mdcc->busyCount(1);
      rc["busy2"]=_mdcc->busyCount(2);
      rc["busy3"]=_mdcc->busyCount(3);
      rc["spillon"]=_mdcc->spillOn();
      rc["spilloff"]=_mdcc->spillOff();
      rc["ecalmask"]=_mdcc->ecalmask();
      rc["beam"]=_mdcc->beam();
      m->setAnswer(rc);

      return;
    }
  if (cmd_name.compare("WRITEREG")==0)
    {
      LOG4CXX_INFO(_logLdaq," execute: "<<cmd_name);
      uint32_t adr=m->content()["address"].asInt();
      uint32_t val=m->content()["value"].asInt();
      _mdcc->writeRegister(adr,val);
      return;
    }
  if (cmd_name.compare("READREG")==0)
    {
      LOG4CXX_INFO(_logLdaq," execute: "<<cmd_name);
      uint32_t adr=m->content()["address"].asInt();

      uint32_t val=_mdcc->readRegister(adr);
      Json::Value r;
      r["address"]=adr;
      r["value"]=val;
      m->setAnswer(r);
      return;
    }
  if (cmd_name.compare("SPILLON")==0)
    {
      LOG4CXX_INFO(_logLdaq," execute: "<<cmd_name);
      uint32_t nc=m->content()["nclock"].asUInt();

      _mdcc->setSpillOn(nc);
      return;
    }
  if (cmd_name.compare("SPILLOFF")==0)
    {
      LOG4CXX_INFO(_logLdaq," execute: "<<cmd_name);
      uint32_t nc=m->content()["nclock"].asUInt();

      _mdcc->setSpillOff(nc);
      return;
    }
  if (cmd_name.compare("BEAM")==0)
    {
      LOG4CXX_INFO(_logLdaq," execute: "<<cmd_name);
      uint32_t nc=m->content()["nclock"].asUInt();

      _mdcc->setBeam(nc);
      return;
    }

}




LMdccServer::LMdccServer(std::string name) : levbdim::baseApplication(name),_mdcc(NULL)
{

  
 

  //_fsm=new levbdim::fsm(name);
  _fsm=this->fsm();

  
// Register state
  _fsm->addState("PAUSED");
  _fsm->addState("RUNNING");

  _fsm->addTransition("OPEN","CREATED","PAUSED",boost::bind(&LMdccServer::open, this,_1));
  _fsm->addTransition("RESUME","PAUSED","RUNNING",boost::bind(&LMdccServer::resume, this,_1));
  _fsm->addTransition("RESET","PAUSED","PAUSED",boost::bind(&LMdccServer::reset, this,_1));
  _fsm->addTransition("PAUSE","RUNNING","PAUSED",boost::bind(&LMdccServer::pause, this,_1));
  _fsm->addTransition("PAUSE","PAUSED","PAUSED",boost::bind(&LMdccServer::pause, this,_1));
  _fsm->addTransition("DESTROY","PAUSED","CREATED",boost::bind(&LMdccServer::close, this,_1));
  
  _fsm->addTransition("CMD","PAUSED","PAUSED",boost::bind(&LMdccServer::cmd, this,_1));
  _fsm->addTransition("CMD","RUNNING","RUNNING",boost::bind(&LMdccServer::cmd, this,_1));
  _fsm->addTransition("ECALPAUSE","PAUSED","PAUSED",boost::bind(&LMdccServer::ecalpause, this,_1));
  _fsm->addTransition("ECALPAUSE","RUNNING","RUNNING",boost::bind(&LMdccServer::ecalpause, this,_1));
  _fsm->addTransition("ECALRESUME","PAUSED","PAUSED",boost::bind(&LMdccServer::ecalresume, this,_1));
  _fsm->addTransition("ECALRESUME","RUNNING","RUNNING",boost::bind(&LMdccServer::ecalresume, this,_1));


 _fsm->addCommand("JOBLOG",boost::bind(&LMdccServer::c_joblog,this,_1,_2));
 _fsm->addCommand("PAUSE",boost::bind(&LMdccServer::c_pause,this,_1,_2));
 _fsm->addCommand("RESUME",boost::bind(&LMdccServer::c_resume,this,_1,_2));
 _fsm->addCommand("RESET",boost::bind(&LMdccServer::c_reset,this,_1,_2));
 _fsm->addCommand("ECALPAUSE",boost::bind(&LMdccServer::c_ecalpause,this,_1,_2));
 _fsm->addCommand("ECALRESUME",boost::bind(&LMdccServer::c_ecalresume,this,_1,_2));
 _fsm->addCommand("WRITEREG",boost::bind(&LMdccServer::c_writereg,this,_1,_2));
 _fsm->addCommand("READREG",boost::bind(&LMdccServer::c_readreg,this,_1,_2));
 _fsm->addCommand("STATUS",boost::bind(&LMdccServer::c_status,this,_1,_2));
 _fsm->addCommand("SPILLON",boost::bind(&LMdccServer::c_spillon,this,_1,_2));
 _fsm->addCommand("SPILLOFF",boost::bind(&LMdccServer::c_spilloff,this,_1,_2));
 _fsm->addCommand("BEAMON",boost::bind(&LMdccServer::c_beamon,this,_1,_2));

 _fsm->addCommand("RESETTDC",boost::bind(&LMdccServer::c_resettdc,this,_1,_2));
 _fsm->addCommand("CALIBON",boost::bind(&LMdccServer::c_calibon,this,_1,_2));
 _fsm->addCommand("CALIBOFF",boost::bind(&LMdccServer::c_caliboff,this,_1,_2));
 _fsm->addCommand("RELOADCALIB",boost::bind(&LMdccServer::c_reloadcalib,this,_1,_2));
 _fsm->addCommand("SETCALIBCOUNT",boost::bind(&LMdccServer::c_setcalibcount,this,_1,_2));
 _fsm->addCommand("SETSPILLREGISTER",boost::bind(&LMdccServer::c_setspillregister,this,_1,_2));
 _fsm->addCommand("SETTRIGEXT",boost::bind(&LMdccServer::c_settrigext,this,_1,_2));


 
  std::stringstream s0;
  s0.str(std::string());
  s0<<"LMdccServer-"<<name;
  DimServer::start(s0.str().c_str()); 

  char* wp=getenv("WEBPORT");
  if (wp!=NULL)
    {
      std::cout<<"Service "<<name<<" started on port "<<atoi(wp)<<std::endl;
    _fsm->start(atoi(wp));
    }


}


void LMdccServer::doOpen(std::string s)
{
  std::cout<<"calling open "<<std::endl;
  if (_mdcc!=NULL)
    delete _mdcc;
  _mdcc= new MDCCHandler(s);
  _mdcc->open();
  //std::cout<<" Open Ptr "<<_mdcc<<std::endl;
}
