
#include "LGenesysServer.hh"
#include "fileTailer.hh"



void LGenesysServer::configure(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  uint32_t port;
  std::string device;
  if (m->content().isMember("device"))
    { 
      device=m->content()["device"].asString();
      this->parameters()["device"]=m->content()["device"];
    }
  else
    device=this->parameters()["device"].asString();
  if (m->content().isMember("port"))
    { 
      port=m->content()["port"].asInt();
      this->parameters()["port"]=m->content()["port"];
    }
  else
    port=this->parameters()["port"].asInt();
 this->Open(device,port);
 this->read(m);
 std::cout<<"reponse=> "<<m->content()["answer"]<<std::endl;
}
void LGenesysServer::on(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  this->Switch(1);
  this->read(m);
}
void LGenesysServer::off(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  this->Switch(0);
  this->read(m);
}
void LGenesysServer::read(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  this->Read();
  Json::Value r;
  r["vset"]=_status[0];
  r["vout"]=_status[1];
  r["iout"]=_status[2];
  m->setAnswer(r);
}

void LGenesysServer::c_joblog(Mongoose::Request &request, Mongoose::JsonResponse &response)
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
void LGenesysServer::c_on(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_zup==NULL)
    {
      LOG4CXX_ERROR(_logLdaq," Genesys not created ");
      response["STATUS"]="FAILED";
      return;
    }
  
  _zup->ON();
  response["STATUS"]="DONE";
  Json::Value jrep;
  jrep["state"]="ON";
  jrep["vset"]=_zup->ReadVoltageSet();
  jrep["vread"]=_zup->ReadVoltageUsed();
  jrep["iread"]=_zup->ReadCurrentUsed();
  if (_zup->ReadCurrentUsed()<0.1)
    jrep["state"]="OFF";
  _status[0]=_zup->ReadVoltageSet();
  _status[1]=_zup->ReadVoltageUsed();
  _status[2]=_zup->ReadCurrentUsed();

  response["INFO"]=jrep;

}
void LGenesysServer::c_off(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_zup==NULL)
    {
      LOG4CXX_ERROR(_logLdaq," Genesys not created ");
      response["STATUS"]="FAILED";
      return;
    }
  
  _zup->OFF();
  response["STATUS"]="DONE";
  Json::Value jrep;
  jrep["state"]="ON";
  jrep["vset"]=_zup->ReadVoltageSet();
  jrep["vread"]=_zup->ReadVoltageUsed();
  jrep["iread"]=_zup->ReadCurrentUsed();
  if (_zup->ReadCurrentUsed()<0.1)
    jrep["state"]="OFF";
  _status[0]=_zup->ReadVoltageSet();
  _status[1]=_zup->ReadVoltageUsed();
  _status[2]=_zup->ReadCurrentUsed();

  response["INFO"]=jrep;

}
void LGenesysServer::c_status(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  if (_zup==NULL)
    {
      LOG4CXX_ERROR(_logLdaq," Genesys not created ");
      response["STATUS"]="FAILED";
      return;
    }
  
  _zup->INFO();
  response["STATUS"]="DONE";
  Json::Value jrep;
  jrep["state"]="ON";
  jrep["vset"]=_zup->ReadVoltageSet();
  jrep["vread"]=_zup->ReadVoltageUsed();
  jrep["iread"]=_zup->ReadCurrentUsed();
  if (_zup->ReadCurrentUsed()<0.1)
    jrep["state"]="OFF";
  _status[0]=_zup->ReadVoltageSet();
  _status[1]=_zup->ReadVoltageUsed();
  _status[2]=_zup->ReadCurrentUsed();

  response["INFO"]=jrep;

}

LGenesysServer::LGenesysServer(std::string name) : levbdim::baseApplication(name),_zup(NULL)
{
  //_fsm=new levbdim::fsm(name);
  //_fsm=new fsmweb(name);
  _fsm=this->fsm();
// Register state
  //_fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("ON");
  _fsm->addState("OFF");

  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&LGenesysServer::configure, this,_1));
  _fsm->addTransition("TON","CONFIGURED","ON",boost::bind(&LGenesysServer::on, this,_1));
  _fsm->addTransition("TON","OFF","ON",boost::bind(&LGenesysServer::on, this,_1));
  _fsm->addTransition("TOFF","CONFIGURED","OFF",boost::bind(&LGenesysServer::off, this,_1));
  _fsm->addTransition("TOFF","ON","OFF",boost::bind(&LGenesysServer::off, this,_1));
  _fsm->addTransition("TREAD","ON","ON",boost::bind(&LGenesysServer::read, this,_1));
  _fsm->addTransition("TREAD","OFF","OFF",boost::bind(&LGenesysServer::read, this,_1));
  _fsm->addTransition("TREAD","CONFIGURED","CONFIGURED",boost::bind(&LGenesysServer::read, this,_1));

  _fsm->addCommand("JOBLOG",boost::bind(&LGenesysServer::c_joblog,this,_1,_2));
  _fsm->addCommand("ON",boost::bind(&LGenesysServer::c_on,this,_1,_2));
  _fsm->addCommand("OFF",boost::bind(&LGenesysServer::c_off,this,_1,_2));
  _fsm->addCommand("STATUS",boost::bind(&LGenesysServer::c_status,this,_1,_2));
  std::stringstream s0;
  s0.str(std::string());
  s0<<"/DZUP/"<<name<<"/STATUS";
  memset(_status,0,3*sizeof(float));
  _zsStatus = new DimService(s0.str().c_str(),"F:3",_status,3*sizeof(float));
  s0.str(std::string());
  s0<<"LGenesysServer-"<<name;
  DimServer::start(s0.str().c_str()); 

  char* wp=getenv("WEBPORT");
  if (wp!=NULL)
    {
      std::cout<<"Service "<<name<<" started on port "<<atoi(wp)<<std::endl;
    _fsm->start(atoi(wp));
    }

	

}

void LGenesysServer::Open(std::string s, uint32_t port)
{
  
  if (_zup!=NULL)
    delete _zup;
  _zup= new Genesys(s,port);

  _zup->INFO();
  this->Read();
}
void LGenesysServer::Read()
{
  if (_zup==NULL)
    {
      LOG4CXX_ERROR(_logLdaq," Genesys not created ");
      return;
    }
  sleep((unsigned int) 1);
  _status[0]=_zup->ReadVoltageSet();
  _status[1]=_zup->ReadVoltageUsed();
  _status[2]=_zup->ReadCurrentUsed();
  this->publishStatus();
}

void LGenesysServer::Switch(uint32_t mode)
{
  if (_zup==NULL)
    {
      LOG4CXX_ERROR(_logLdaq," Genesys not created ");
      return;
    }
  if (mode==0)
    {
      LOG4CXX_INFO(_logLdaq,"Switching OFF "<<mode);
      _zup->OFF();
    }
  else
    {
      LOG4CXX_INFO(_logLdaq,"Switching ON "<<mode);
      _zup->ON();
    }
  this->Read();
}
