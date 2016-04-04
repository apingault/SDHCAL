#include "LBuilder.hh"
#include "basicwriter.hh"
#ifdef USE_LCIO
#include "LcioShmProcessor.hh"
#endif
LBuilder::LBuilder(std::string name) : _evb(NULL),_writer(NULL)
{
  _fsm=new levbdim::fsm(name);

  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("INITIALISED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("INITIALISE","CREATED","INITIALISED",boost::bind(&LBuilder::initialise, this,_1));
  _fsm->addTransition("CONFIGURE","INITIALISED","CONFIGURED",boost::bind(&LBuilder::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&LBuilder::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CONFIGURED",boost::bind(&LBuilder::stop, this,_1));
  _fsm->addTransition("HALT","RUNNING","INITIALISED",boost::bind(&LBuilder::halt, this,_1));
  _fsm->addTransition("DESTROY","INITIALISED","CREATED",boost::bind(&LBuilder::destroy, this,_1));
  _fsm->addTransition("STATUS","CONFIGURED","CONFIGURED",boost::bind(&LBuilder::status, this,_1));
  _fsm->addTransition("STATUS","RUNNING","RUNNING",boost::bind(&LBuilder::status, this,_1));

  //Start server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"LBuilder-"<<name;
  DimServer::start(s0.str().c_str()); 

}

void LBuilder::configure(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  _sources.clear();
  for (std::vector<levbdim::datasocket*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);

  Json::Value jc=m->content();
  const Json::Value& jsources = jc["sources"];
  for (Json::ValueConstIterator it = jsources.begin(); it != jsources.end(); ++it)
    {
      const Json::Value& source = *it;
      int32_t detid=source["detid"].asInt();
      int32_t sid=source["sourceid"].asInt();
    // rest as before
      LOG4CXX_INFO(_logLdaq,"Creating datasocket "<<detid<<" "<<sid);
      levbdim::datasocket* ds= new levbdim::datasocket(detid,sid,0x80000);
      ds->save2disk(_memorypath);
      _sources.push_back(ds);
      _evb->registerDataSource(detid,sid);
    }

}
void LBuilder::destroy(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_evb!=NULL)
    delete _evb;
  _evb=NULL;
  if (_writer!=NULL)
    delete _writer;
}
void LBuilder::initialise(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  if (_evb!=NULL)
    delete _evb;

  _memorypath = m->content()["memorypath"].asString();
  _filepath = m->content()["filepath"].asString();
  _proctype= m->content()["proctype"].asString();
  // Now create the builder

  _evb= new levbdim::shmdriver(_memorypath);
  _evb->createDirectories();
  _evb->cleanShm();


   Json::Value jc=m->content();
  const Json::Value& jsources = jc["proclist"];
  for (Json::ValueConstIterator it = jsources.begin(); it != jsources.end(); ++it)
    {
      const Json::Value& source = *it;
      std::string name=source.asString();
        // Add a writer
      if (name.compare("basicwriter")==0)
    {
      _writer= new levbdim::basicwriter(_filepath);
      _evb->registerProcessor(_writer);
    }
#ifdef USE_LCIO
  if (name.compare("lcio")==0)
    {
      _writer= new LcioShmProcessor(_filepath,"UNSETUP");
      _evb->registerProcessor(_writer);
    }
#endif

    }


  /*  
  // Add a writer
  if (_proctype.compare("basicwriter")==0)
    {
      _writer= new levbdim::basicwriter(_filepath);
      _evb->registerProcessor(_writer);
    }
#ifdef USE_LCIO
  if (_proctype.compare("lcio")==0)
    {
      _writer= new LcioShmProcessor(_filepath,"UNSETUP");
      _evb->registerProcessor(_writer);
    }
#endif
  */
}
void LBuilder::start(levbdim::fsmmessage* m)
{
    LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
    Json::Value jc=m->content();
    int32_t run=jc["run"].asInt();
    //_writer->start(run);
    _evb->start(run);
}
void LBuilder::stop(levbdim::fsmmessage* m)
{
  
    LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());

    _evb->stop();
    //_writer->stop();
}
void LBuilder::status(levbdim::fsmmessage* m)
{
  
    LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
    Json::Value rp;
    rp["run"]=_evb->run();
    rp["event"]=_evb->event();
    m->setAnswer(rp);
}
void LBuilder::halt(levbdim::fsmmessage* m)
{
  LOG4CXX_INFO(_logLdaq," CMD: "<<m->command());
  this->stop(m);
  //delete data sockets
  for (std::vector<levbdim::datasocket*>::iterator it=_sources.begin();it!=_sources.end();it++)
    delete (*it);
  _sources.clear();
}