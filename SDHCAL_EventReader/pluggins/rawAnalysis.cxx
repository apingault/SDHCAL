#define NX 36
#define NY 36

#include "rawAnalysis.hh"
#include "DIFUnpacker.h"
#include <TLine.h>
#include <TGraphErrors.h>
#include <TFitResult.h>
#include <TFitter.h>
#include <TF1.h>
#include <TPluginManager.h>
#include <stdint.h>
#include <math.h>
#include "TPolyLine3D.h"
#include "TVirtualPad.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
//#include <lapacke.h>
#include "DifGeom.h"
#include "jsonGeo.hh"



rawAnalysis::rawAnalysis() :nAnalyzed_(0),theMonitoringPeriod_(0),theMonitoringPath_("/dev/shm/Monitoring"),theSkip_(0),draw_(false),_geo(NULL),_monitor(NULL)
{
  reader_=DHCalEventReader::instance();
  rootHandler_ =DCHistogramHandler::instance();
  this->initialise();
}



void rawAnalysis::initialise()
{

  theTotalTime_=0.;
  memset(theTotalCount_,0,255*49*sizeof(uint32_t));
  theStartBCID_=0;  
}
void rawAnalysis::initJob(){
  // presetParameters();
  // if (_geo!=NULL)
  //   _monitor=new hitMonitor(_geo);
  char* wp=getenv("GEOMETRY");

  std::cout<<" INITJOB \n"<<wp<<std::flush;
  if (wp!=NULL)
    {
      _geo=new jsonGeo(std::string(wp));
    }
  else
    _geo=new jsonGeo("/home/acqilc/SDHCAL/SDHCAL_EventReader/pluggins/newgeo4_dome.json");
 _monitor=new hitMonitor(_geo);
}
void rawAnalysis::endJob(){
  if (theMonitoringPeriod_!=0)
    {
      rootHandler_->writeXML(theMonitoringPath_);
      //char c;c=getchar();putchar(c); if (c=='.') exit(0);;
		
    }
	
}
void rawAnalysis::presetParameters()
{
  std::map<std::string,MarlinParameter> m=reader_->getMarlinParameterMap();
  std::map<std::string,MarlinParameter>::iterator it;
  try
    {
      if ((it=m.find("Interactif"))!=m.end()) draw_=it->second.getBoolValue();
      if ((it=m.find("SkipEvents"))!=m.end()) theSkip_=it->second.getIntValue();
      if ((it=m.find("MonitoringPath"))!=m.end()) theMonitoringPath_=it->second.getStringValue();
      if ((it=m.find("MonitoringPeriod"))!=m.end()) theMonitoringPeriod_=it->second.getIntValue();

      INFO_PRINT("Interactif %d \n",draw_);
      //std::string geofile;
      if ((it=m.find("geometry"))!=m.end())
	_geo=new jsonGeo(it->second.getStringValue());
      

    }
  catch (std::string s)
    {
      std::cout<<__PRETTY_FUNCTION__<<" error "<<s<<std::endl;
    }
	
}

void rawAnalysis::processEvent()
{
  
  if (reader_->getEvent()==0) return;
  evt_=reader_->getEvent();
  if (evt_->getEventNumber()<=theSkip_) return;
  TH1* hacqtime=rootHandler_->GetTH1("AcquisitionTime");
  TH2* hfr2=rootHandler_->GetTH2("HitFrequency");
  if (hacqtime==NULL)
    {
      hacqtime =rootHandler_->BookTH1( "AcquisitionTime",2000,0.,2.);

      hfr2=rootHandler_->BookTH2("HitFrequency",255,0.1,255.1,48,0.1,48.1);
    }
  printf("4\n");
   reader_->parseLevbdimEvent();
  std::vector<DIFPtr*>::iterator itb =reader_->getDIFList().begin();
  if (itb==reader_->getDIFList().end()) return;
				      
  DIFPtr* dbase= (*itb);
  
  if ( theStartBCID_==0) theStartBCID_=dbase->getAbsoluteBCID();
  theEventTotalTime_=0; 
 
  std::cout<<"Event  "<<dbase->getAbsoluteBCID()<<std::endl;
  
  memset(theCount_,0,255*49*sizeof(uint32_t));
  for (std::vector<DIFPtr*>::iterator it = reader_->getDIFList().begin();it!=reader_->getDIFList().end();it++)
    {
      DIFPtr* d = (*it);
      if (d->getID()>255) continue;
     // Loop on frames
      for (uint32_t i=0;i<d->getNumberOfFrames();i++)
      {
	double t=d->getFrameTimeToTrigger(i)*2E-7;

	if (t>3.8) {
	  //printf("Wrong Time %f %x \n",t,d->getFrameTimeToTrigger(i));
	  continue;
	}
	if (dbase->getDTC()==17 && d->getFrameTimeToTrigger(i)>80385 && d->getFrameTimeToTrigger(i)<80395 )
	  {
	    std::cout<<d->getID()<<" "<<d->getAbsoluteBCID()<<" "<<d->getFrameTimeToTrigger(i);
	    getchar();
	  }
	//printf("%d %d \n",d->getID(),d->getFrameAsicHeader(i));
	if (d->getFrameAsicHeader(i)>48) continue;
	theCount_[d->getID()-1][0]++;
	theCount_[d->getID()-1][d->getFrameAsicHeader(i)]++;
	if (t>theEventTotalTime_) theEventTotalTime_=t;
	// Fill ASICs histogram
	//if (theTotalTime_<1.) continue;
	//if ((theTotalCount_[d->getID()-1][d->getFrameAsicHeader(i)]/theTotalTime_)<700.) continue;
	std::stringstream s;
	s<<"/DIF"<<d->getID()<<"/Asic"<<d->getFrameAsicHeader(i);
	TH1* han=rootHandler_->GetTH1(s.str()+"/Hits");
	TH1* han20=rootHandler_->GetTH1(s.str()+"/Hits20");
	TH1* hfr=rootHandler_->GetTH1(s.str()+"/Frequency");
	TH1* hframetime=rootHandler_->GetTH1(s.str()+"/FrameTime");
	TH1* hmul=rootHandler_->GetTH1(s.str()+"/AsicMul");

	if (han==NULL)
	  {
	    printf("booking %s \n",s.str().c_str());
	    han =rootHandler_->BookTH1(s.str()+"/Hits",64,0.1,64.1);
	    han20 =rootHandler_->BookTH1(s.str()+"/Hits20",64,0.1,64.1);
	    hfr =rootHandler_->BookTH1(s.str()+"/Frequency",64,0.1,64.1);
	    hframetime =rootHandler_->BookTH1(s.str()+"/FrameTime",2000,0.,2000.);
	    hmul =rootHandler_->BookTH1(s.str()+"/AsicMul",65,0.,65.);
      
	  }
	hframetime->Fill(d->getFrameTimeToTrigger(i)*1.);
	uint32_t ninasic=0;
	 for (uint32_t j=0;j<64;j++)
	   {
	     if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1)))
	       continue;
	     else
	       ninasic++;
	     han->Fill(j*1.);
	     if (d->getFrameTimeToTrigger(i)<20)
	       han20->Fill(j*1.);
	   }
	 hmul->Fill(ninasic*1.);
      }
    }
  hacqtime->Fill(theEventTotalTime_);
  theTotalTime_+=theEventTotalTime_;
  printf("Processing %d - %d GTC %d Total time %f Acquition time %f\n",evt_->getRunNumber(),evt_->getEventNumber(),dbase->getGTC(),(dbase->getAbsoluteBCID()-theStartBCID_)*2E-7,theTotalTime_);
  if (evt_->getEventNumber()%100 ==0)
    rootHandler_->writeSQL();

  // Now loop on DIF
  for (uint32_t i=0;i<255;i++)
    {
      if (!theCount_[i][0]) continue;
      for (uint32_t j=0;j<49;j++)
	theTotalCount_[i][j]+=theCount_[i][j];
      uint32_t difid=i+1;
    }
  for (uint32_t i=0;i<255;i++)
    {
      if (!theTotalCount_[i][0]) continue;
      for (uint32_t j=1;j<49;j++)
	{
	  hfr2->SetBinContent(i+1,j,theTotalCount_[i][j]/theTotalTime_);
	  if (theTotalTime_<1.) continue;
	  if ((theTotalCount_[i][j]/theTotalTime_)<0.) continue;
	  std::stringstream s;
	  s<<"/DIF"<<i+1<<"/Asic"<<j;
	  TH1* han=rootHandler_->GetTH1(s.str()+"/Hits");
	  TH1* hfr=rootHandler_->GetTH1(s.str()+"/Frequency");
	  if (han==NULL) continue;
	  for (uint32_t k=0;k<64;k++)
	     hfr->SetBinContent(k+1,han->GetBinContent(k+1)/theTotalTime_);


	  
	}
      
    }


  // STudy RAwCalorimeters hits
  if (_geo!=NULL && _monitor!=NULL)
    {
      IMPL::LCCollectionVec* rhcold=NULL;
      IMPL::LCCollectionVec* rhcol=NULL;
      try {
      rhcold=(IMPL::LCCollectionVec*) reader_->getEvent()->getCollection("DHCALRawHits");
      } catch(...)
	{
	  std::cout<<" No DHCALrawHits \n";
	}
   
      reader_->parseLevbdimEvent();

      //reader_->flagSynchronizedFrame();
      std::vector<uint32_t> seed;
      seed.clear();
      //reader_->findDIFSeeds(minChambersInTime_);
      //rhcol=reader_->createRawCalorimeterHits(reader_->getDIFSeeds());
      rhcol=reader_->createRawCalorimeterHits(seed);
      if (rhcold!=0)
	printf("DHCALRAWHITS %d ne col %d \n",rhcold->getNumberOfElements(),rhcol->getNumberOfElements());
      if (rhcol!=NULL)
	{
	  if (rhcol->getNumberOfElements()<2500)
	    {
	      _monitor->FillTimeAsic(rhcol);

	      _monitor->DIFStudy(rhcol);
	    }
	}
      delete rhcol;
    }
  /*
  std::map<unsigned int,DifGeom>::iterator idg = reader_->getDifMap().find(d->getID());
  DifGeom& difgeom = idg->second;
  uint32_t chid = idg->second.getChamberId();
  */
  //  LCTOOLS::printParameters(rhcol->parameters());
  //DEBUG_PRINT("Time Stamp %d \n",evt_->getTimeStamp());

}
extern "C" 
{
    // loadDHCALAnalyzer function creates new LowPassDHCALAnalyzer object and returns it.  
    DHCALAnalyzer* loadAnalyzer(void)
    {
      return (new rawAnalysis);
    }
    // The deleteDHCALAnalyzer function deletes the LowPassDHCALAnalyzer that is passed 
    // to it.  This isn't a very safe function, since there's no 
    // way to ensure that the object provided is indeed a LowPassDHCALAnalyzer.
    void deleteAnalyzer(DHCALAnalyzer* obj)
    {
      delete obj;
    }
}
