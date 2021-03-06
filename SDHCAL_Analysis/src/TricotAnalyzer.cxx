#define MAX_CHAMBER 8
#define MAX_ASIC 3
#define IDB(C,A) ((C-1)*MAX_ASIC+(A-1))
#define MIDB(C,A) (1<<IDB(C,A))
#define FLAGASIC(I,C,A) (I)|=MIDB(C,A)
#define ASICFLAGGED(I,C,A) ((I)&MIDB(C,A))
#include "TricotAnalyzer.h"
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
#include "DIFSnapShot.h"
#include "Shower.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dir.h>  
#include <sys/param.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>


uint32_t count_plan(uint32_t K,bool printit=false)
{
  uint32_t npl=0;
  for (uint32_t ipl=0;ipl<MAX_CHAMBER;ipl++)
    {
      uint32_t nas=0;
      for (uint32_t ia=0;ia<MAX_ASIC;ia++)
	if (ASICFLAGGED(K,ipl+1,ia+1)!=0)
	  {nas++;
	    if (printit) printf("Mask %d : %d %d  idb %d  midb %d flagged \n",K,ipl+1,ia+1,IDB(ipl+1,ia+1),MIDB(ipl+1,ia+1));}
      if (nas>=2) npl++;
    }
  return npl;
}
TricotAnalyzer::TricotAnalyzer() :nAnalyzed_(0),theMonitoringPeriod_(0),theMonitoringPath_("/dev/shm/Monitoring"),theSkip_(0),draw_(false)
{
  reader_=DHCalEventReader::instance();
  rootHandler_ =DCHistogramHandler::instance();
  this->initialise();
  _neff=0;_neff2=0;_neff3=0;_neff4=0;_nall=0;

 // if (evt_->getRunNumber()<731323)
 //   {
 //     _plid.insert(std::pair<uint8_t,uint8_t>(5,1));
 //     _plid.insert(std::pair<uint8_t,uint8_t>(94,2));
 //     _plid.insert(std::pair<uint8_t,uint8_t>(99,3));
 //     _plid.insert(std::pair<uint8_t,uint8_t>(128,4));
 //     _plid.insert(std::pair<uint8_t,uint8_t>(1,5));
 //     _plz.insert(std::pair<uint8_t,float>(5,10.));
 //     _plz.insert(std::pair<uint8_t,float>(94,29.));
 //     _plz.insert(std::pair<uint8_t,float>(99,45.));
 //     _plz.insert(std::pair<uint8_t,float>(128,61.));
 //     _plz.insert(std::pair<uint8_t,float>(1,77.));
 //     _pldx.insert(std::pair<uint8_t,float>(5,8.));
 //     _pldx.insert(std::pair<uint8_t,float>(94,0.));
 //     _pldx.insert(std::pair<uint8_t,float>(99,2.));
 //     _pldx.insert(std::pair<uint8_t,float>(128,0.));
 //     _pldx.insert(std::pair<uint8_t,float>(1,0.));
 //     _pldy.insert(std::pair<uint8_t,float>(5,0.));
 //     _pldy.insert(std::pair<uint8_t,float>(94,0.));
 //     _pldy.insert(std::pair<uint8_t,float>(99,3.));
 //     _pldy.insert(std::pair<uint8_t,float>(128,8.));
 //     _pldy.insert(std::pair<uint8_t,float>(1,0.));
 //   }
 // else
 //   {
#ifdef FIRSTTRY
      _pltyp.insert(std::pair<uint8_t,uint8_t>(5,1));
     _pltyp.insert(std::pair<uint8_t,uint8_t>(94,1));
     _pltyp.insert(std::pair<uint8_t,uint8_t>(99,1));
     _pltyp.insert(std::pair<uint8_t,uint8_t>(128,1));
     _pltyp.insert(std::pair<uint8_t,uint8_t>(1,1));
     _pltyp.insert(std::pair<uint8_t,uint8_t>(142,0));

     _plid.insert(std::pair<uint8_t,uint8_t>(142,0));
     _plid.insert(std::pair<uint8_t,uint8_t>(5,1));
     _plid.insert(std::pair<uint8_t,uint8_t>(94,2));
     _plid.insert(std::pair<uint8_t,uint8_t>(99,3));
     _plid.insert(std::pair<uint8_t,uint8_t>(128,4));
     _plid.insert(std::pair<uint8_t,uint8_t>(1,5));

     
     _pldif.insert(std::pair<uint8_t,uint8_t>(0,142));
     _pldif.insert(std::pair<uint8_t,uint8_t>(1,5));
     _pldif.insert(std::pair<uint8_t,uint8_t>(2,94));
     _pldif.insert(std::pair<uint8_t,uint8_t>(3,99));
     _pldif.insert(std::pair<uint8_t,uint8_t>(4,128));
     _pldif.insert(std::pair<uint8_t,uint8_t>(5,1));

     _plz.insert(std::pair<uint8_t,float>(142,10.));
     _plz.insert(std::pair<uint8_t,float>(5,29.));
     _plz.insert(std::pair<uint8_t,float>(94,48.));
     _plz.insert(std::pair<uint8_t,float>(99,64.));
     _plz.insert(std::pair<uint8_t,float>(128,80.));
     _plz.insert(std::pair<uint8_t,float>(1,96.));

     
     _pldx.insert(std::pair<uint8_t,float>(5,0.35));
     _pldx.insert(std::pair<uint8_t,float>(94,-0.1));
     _pldx.insert(std::pair<uint8_t,float>(99, -0.08));
     _pldx.insert(std::pair<uint8_t,float>(128,0.16));
     _pldx.insert(std::pair<uint8_t,float>(1,0.84));
     _pldx.insert(std::pair<uint8_t,float>(142,-10.));

     _pldy.insert(std::pair<uint8_t,float>(5,0.35));
     _pldy.insert(std::pair<uint8_t,float>(94,-0.1));
     _pldy.insert(std::pair<uint8_t,float>(99,0.25));
     _pldy.insert(std::pair<uint8_t,float>(128,-0.09));
     _pldy.insert(std::pair<uint8_t,float>(1,0.));
     _pldy.insert(std::pair<uint8_t,float>(142,-2.));

     _plaxi.insert(std::make_pair(0,-8.0));
     _playi.insert(std::make_pair(0,2.8));
     _plaxa.insert(std::make_pair(0,18.0));
     _playa.insert(std::make_pair(0,28.8));

     _plaxi.insert(std::make_pair(1,0.0));
     _playi.insert(std::make_pair(1,0.0));
     _plaxa.insert(std::make_pair(1,18.0));
     _playa.insert(std::make_pair(1,28.3));
     _plaxi.insert(std::make_pair(2,0.0));
     _playi.insert(std::make_pair(2,0.0));
     _plaxa.insert(std::make_pair(2,18.0));
     _playa.insert(std::make_pair(2,28.3));
     _plaxi.insert(std::make_pair(3,0.0));
     _playi.insert(std::make_pair(3,0.0));
     _plaxa.insert(std::make_pair(3,18.0));
     _playa.insert(std::make_pair(3,28.3));
     _plaxi.insert(std::make_pair(4,0.0));
     _playi.insert(std::make_pair(4,0.0));
     _plaxa.insert(std::make_pair(4,18.0));
     _playa.insert(std::make_pair(4,28.3));
     _plaxi.insert(std::make_pair(5,0.0));
     _playi.insert(std::make_pair(5,0.0));
     _plaxa.insert(std::make_pair(5,9.9));
     _playa.insert(std::make_pair(5,28.3));
     //   }

#endif


}



void TricotAnalyzer::initialise()
{

  theTotalTime_=0.;
  memset(theTotalCount_,0,255*49*sizeof(uint32_t));
  theStartBCID_=0;  
}
void TricotAnalyzer::initJob(){presetParameters();}
void TricotAnalyzer::endJob(){
  if (theMonitoringPeriod_!=0)
    {
      rootHandler_->writeXML(theMonitoringPath_);
      //char c;c=getchar();putchar(c); if (c=='.') exit(0);;
		
    }
	
}
static TCanvas* TCPlot=NULL;
void TricotAnalyzer::presetParameters()
{
  std::map<std::string,MarlinParameter> m=reader_->getMarlinParameterMap();
  std::map<std::string,MarlinParameter>::iterator it;
  try
    {
      if ((it=m.find("Interactif"))!=m.end()) draw_=it->second.getBoolValue();
      if ((it=m.find("SkipEvents"))!=m.end()) theSkip_=it->second.getIntValue();
      if ((it=m.find("MonitoringPath"))!=m.end()) theMonitoringPath_=it->second.getStringValue();
      if ((it=m.find("MonitoringPeriod"))!=m.end()) theMonitoringPeriod_=it->second.getIntValue();

      DEBUG_PRINT("Interactif %d \n",draw_);

      //getchar();

    }
  catch (std::string s)
    {
      std::cout<<__PRETTY_FUNCTION__<<" error "<<s<<std::endl;
    }
	
}

static int nst=0;
static float allt=0.;
void TricotAnalyzer::processEvent()
{
  
  if (reader_->getEvent()==0) return;
  evt_=reader_->getEvent();
  std::cout<<" Processing "<<evt_->getRunNumber()<<"-"<<evt_->getEventNumber()<<std::endl;
  /*
  if (reader_->getRunHeader()!=0)
    std::cout<<reader_->getRunHeader()->getParameters().getStringVal("Setup")<<std::endl;
  */
  if (evt_->getEventNumber()<=theSkip_) return;
  if (nAnalyzed_==0)
    {
      std::stringstream filename("");    
      filename<<"./tricot_"<<evt_->getRunNumber()<<".dat";
      _fdOut= ::open(filename.str().c_str(),O_CREAT| O_RDWR | O_NONBLOCK,S_IRWXU);
      if (_fdOut<0)
	{
	  perror("No way to store to file :");
      //std::cout<<" No way to store to file"<<std::endl;
	  return;
	}  
    }
  nAnalyzed_++;
  TH1* hacqtime=rootHandler_->GetTH1("AcquisitionTime");

  
  TH2* hfr2=rootHandler_->GetTH2("HitFrequency");
  if (hacqtime==NULL)
    {
      hacqtime =rootHandler_->BookTH1( "AcquisitionTime",2000,0.,2.);

      hfr2=rootHandler_->BookTH2("HitFrequency",255,0.1,255.1,48,0.1,48.1);
    }
  //  printf("4\n");
   reader_->parseRawEvent();
  std::vector<DIFPtr*>::iterator itb =reader_->getDIFList().begin();
  if (itb==reader_->getDIFList().end()) return;
				      
  DIFPtr* dbase= (*itb);
  
  if ( theStartBCID_==0) theStartBCID_=dbase->getAbsoluteBCID();
  theEventTotalTime_=0; 
 
  //std::cout<<"Event  "<<dbase->getAbsoluteBCID()<<std::endl;
  bool goodevent=false;
  std::vector<uint32_t> gseed;
  gseed.clear();
  
  this->buildFrameMap(gseed);
  uint32_t nsg=0,nsb=0;
  for (std::vector<uint32_t>::iterator it=gseed.begin();it!=gseed.end();it++)
    {
      if ( (*it)<19) nsg++;
      if ((*it)<15) nsb++;
    }
  goodevent=(nsg==1) && (nsb==0);
  if (!goodevent) return;

  // Map study
  for( std::map<uint32_t,std::vector<ptrDifFrame> >::iterator im=_FrameMap.begin(),im_next=im;im!=_FrameMap.end();im++)
	{
	  if (im->first>18) continue;
	  std::bitset<16> difs;
	  difs.reset();
	  for (std::vector<ptrDifFrame>::iterator ih=im->second.begin();ih!=im->second.end();ih++)
	    difs.set((uint8_t) _plid[ih->first->getID()]);
	 
	  std::cout<<im->first<<" "<<difs.to_string().substr(16-_plid.size(),_plid.size())<<std::endl;

	  std::vector<ptrDifFrame> &v=im->second;
	  _currentSeed=im->first;
	  this->dumpSeed(v);
	  //this->processOneSeed(v);
	  
	  if (_npBuf>=3) {
	    this->buildPrincipal("/Track");
	    // this->ShowTracks();
	  }
	  for (std::vector<ptrDifFrame>::iterator ih=im->second.begin();ih!=im->second.end();ih++)
	    {
	      uint32_t difid=ih->first->getID();
	      DIFPtr* d=ih->first;
	      //std::cout<<"\t "<<difid<<" "<<(uint32_t) _plid[difid]<<" "<<d->getFrameTimeToTrigger(ih->second)<<" "<<d->getFrameAsicHeader(ih->second)<<std::endl;
	    }
	  for (int i=0;i<_plid.size();i++)
	    {
	      theTotalCount_[i][0]++;
	      if ((i==0) && !(difs.test(1) && difs.test(2) && difs.test(_plid.size()-1)) )
		{
		  //printf("%d %d %d %d \n",difs.test(i),difs.test(1), _plid.size()-1,difs.test(_plid.size()-1));
		  continue;
		}
	      if (i==(_plid.size()-1) && !(difs[0] && difs[_plid.size()-3]&& difs[_plid.size()-2]))
		  continue;
	      
	      if (i!=0 && i!=(_plid.size()-1) && !(difs[i-1] && difs[i+1])) continue;
	      theTotalCount_[i][1]++;
	      if (difs.test(i)) theTotalCount_[i][2]++;
	    }
	}
  for (int i=0;i<_plid.size();i++)
    printf("%2d %3d %6d %6d %6d %5.2f \n",i,_pldif[i],theTotalCount_[i][0],theTotalCount_[i][1],theTotalCount_[i][2],theTotalCount_[i][2]*100./theTotalCount_[i][1]);
	  

  //getchar();
  return;
#ifdef OLDWAYSTRIP  
#define DRAWPLOT
#ifdef DRAWPLOT
  if (TCPlot==NULL)
    {
      TCPlot=new TCanvas("TCPlotR","tcplotr1",1300,900);
      TCPlot->Modified();
      TCPlot->Draw();
      TCPlot->Divide(3,3);
    }

#endif
  // Geometry
   int ipl=0;

  
  memset(theCount_,0,255*49*sizeof(uint32_t));
  struct timespec tp0,tp1;
  clock_gettime(CLOCK_REALTIME,&tp0);
  int64_t tlast=0;

  // reader_->findTimeSeeds(5);

  // std::vector<uint32_t> seeds=reader_->getTimeSeeds();


  std::map<uint32_t,uint32_t > chti;
  chti.clear();
  for (std::vector<DIFPtr*>::iterator it = reader_->getDIFList().begin();it!=reader_->getDIFList().end();it++)
    {
      DIFPtr* d = (*it);
      if (d->getID()>255) continue;

      //printf("DIF %d BCID %d %d frames \n",d->getID(),d->getBCID(),d->getNumberOfFrames());
      for (uint32_t i=0;i<d->getNumberOfFrames();i++)
      {
	if (d->getID()<256)
	  {
	    //if (d->getID()==142)
	    //printf("%d ",d->getFrameTimeToTrigger(i));
	    int32_t idt=d->getBCID()-d->getFrameBCID(i);
	    if (idt<0)
	      printf(" error frame %d  FBCID %d  ASIC %d DIF %d  BCID %d\n",i,d->getFrameBCID(i),d->getFrameAsicHeader(i),d->getID(),d->getBCID());
	  }
	

	double t=d->getFrameTimeToTrigger(i)*2E-7;

	if (t>3.8) {
	  printf("Wrong Time DT  %f  FTTT %x BCID %lx  Frame BCID %lx \n",t,d->getFrameTimeToTrigger(i),d->getBCID(),d->getFrameBCID(i));
	  continue;
	}
	for (uint32_t j=0;j<64;j++)
	  {
	    if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1))) continue;
	    int asic=d->getFrameAsicHeader(i),channel=j+1;
	    

	
	    std::map<uint32_t,uint32_t >::iterator ich=chti.find(d->getFrameTimeToTrigger(i));
	    std::map<uint32_t,uint32_t >::iterator ichm=chti.find(d->getFrameTimeToTrigger(i)-1);
	    std::map<uint32_t,uint32_t >::iterator ichp=chti.find(d->getFrameTimeToTrigger(i)+1);
	    if (ich!=chti.end())
	      {
		FLAGASIC(ich->second,_plid[d->getID()],asic);
		continue;
	      }
	    if (ichm!=chti.end())
	      {
		FLAGASIC(ichm->second,_plid[d->getID()],asic);
		continue;
	      }
	    if (ichp!=chti.end())
	      {
		FLAGASIC(ichp->second,_plid[d->getID()],asic);
		continue;
	      }

	    uint32_t k=0;FLAGASIC(k,_plid[d->getID()],asic);
	//std::pair<uint32_t,uint32_t> p(d->getFrameTimeToTrigger(i),k);
	    chti.insert(std::make_pair(d->getFrameTimeToTrigger(i),k));
			
	  }
      }
      //printf("\n");
    }
  uint32_t nseeds=0;
  std::vector<uint32_t> seedm;
  seedm.clear();
  for (std::map<uint32_t,uint32_t>::iterator ich=chti.begin();ich!=chti.end();)
    {
      std::bitset<32> bs(ich->second);
      //std::cout<<bs.to_string();
      //printf(" %s seed %d ==> %d cnt %d \n",bs.to_string().c_str(),ich->first,ich->second,count_plan(ich->second));
      //count_plan(ich->second,true);
      if (count_plan(ich->second)>=1) {
	//std::cout<<  "     seed " <<ich->first<<" "<<std::setw(5)<<ich->second.count()<<" "<<ich->second<<std::endl;
	std::map<uint32_t,uint32_t>::iterator ichn=chti.find(ich->first+1);
      if (ichn==chti.end())
	{
	  nseeds++;
	  //std::cout<<"Good seed " <<ich->first<<" "<<std::setw(5)<<ich->second.count()<<" "<<ich->second<<std::endl;
	  if (ich->first>tlast) tlast=ich->first;
	  seedm.push_back(ich->first);
	  ich++;
	}
      else
	{
	  if (count_plan(ichn->second)>=1)
	    {
	      //std::cout<<"Mer0 seed " <<ich->first<<" "<<std::setw(5)<<ich->second.count()<<" "<<ich->second<<std::endl;
	      //std::cout<<"Mer1 seed " <<ichn->first<<" "<<std::setw(5)<<ichn->second.count()<<" "<<ichn->second<<std::endl;
	      ichn->second|=ich->second;
	      chti.erase(ich++);
	    }
	  else
	    {
	      nseeds++;
	      // std::cout<<"Good seed " <<ich->first<<" "<<std::setw(5)<<ich->second.count()<<" "<<ich->second<<std::endl;
	      if (ich->first>tlast) tlast=ich->first;
	      seedm.push_back(ich->first);
	      ich++;
	    }
	}
      }
     else
       ich++;
  
    }
for (std::vector<uint32_t>::iterator is=seedm.begin();is!=seedm.end();is++)
 {
   std::map<uint32_t,uint32_t >::iterator ichn=chti.find((*is));
   if (ichn!=chti.end())
     if (count_plan(ichn->second)>=3 || (count_plan(ichn->second)>=1 &&ichn->first<21))
       {
	 goodevent=true;
       std::cout<<"find seed " <<ichn->first<<" "<<std::setw(5)<<count_plan(ichn->second)<<" "<<ichn->second<<std::endl;
       gseed.push_back(ichn->first);
       }
 }


 if (!goodevent) return;

 TH2* hplx=rootHandler_->GetTH2("/plx");
 TH2* hply=rootHandler_->GetTH2("/ply");
 if (hplx==NULL)
   {
     
     float _zmin=9999;
     float _zmax=-9999.;
     for (std::map<uint8_t,float>::iterator it=_plz.begin();it!=_plz.end();it++)
       {_zmin=(it->second<_zmin)?it->second:_zmin;_zmax=(it->second>_zmax)?it->second:_zmax;}
     _zmin-=10;
     _zmax+=10;
     uint32_t _nb=int(_zmax-_zmin)+1;
     printf("%d %f %f \n",_nb,_zmin,_zmax);
     //getchar();
     hplx=rootHandler_->BookTH2("/plx",_nb,_zmin,_zmax,100,-10.,40.);
     hply=rootHandler_->BookTH2("/ply",_nb,_zmin,_zmax,100,-10.,40.);
   }
 else
   {
     hplx->Reset();
     hply->Reset();
   }

 std::bitset<64> plhit(0);
 _npBuf=0;
  for (std::vector<DIFPtr*>::iterator it = reader_->getDIFList().begin();it!=reader_->getDIFList().end();it++)
    {
      DIFPtr* d = (*it);
      if (d->getID()>255) continue;
      std::stringstream s1;
      s1<<"/Plot"<<d->getID();
      TH2* hpl=rootHandler_->GetTH2(s1.str());
      if (hpl==NULL)
	{
	  hpl=rootHandler_->BookTH2(s1.str(),70,-10.,60.,70,-10.,70.);
	}
      else
	hpl->Reset();
   
      std::vector<float> va_,vb_;
      va_.clear();
      vb_.clear();
      std::vector<TStripCluster*> _cl;

      
     // Loop on frames
      uint32_t dmin=1000;
      uint32_t npd=0;
      for (uint32_t i=0;i<d->getNumberOfFrames();i++)
      {
	double t=d->getFrameTimeToTrigger(i)*2E-7;
	if (d->getFrameTimeToTrigger(i)<dmin) 
	  {
	    dmin=d->getFrameTimeToTrigger(i);
	    npd=0;
	    for (uint32_t j=0;j<64;j++)
	      {
		if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1))) continue;
		npd++;
	      }

	  }

	if (t>3.8) {
	  printf("Wrong Time %f %x %d %d %x %x \n",t,d->getFrameTimeToTrigger(i),d->getBCID(),d->getFrameBCID(i),d->getBCID(),d->getFrameBCID(i));
	  ipl++;
	  continue;
	}
	if (dbase->getDTC()==17 && d->getFrameTimeToTrigger(i)>80385 && d->getFrameTimeToTrigger(i)<80395 )
	  {
	    std::cout<<d->getID()<<" "<<d->getAbsoluteBCID()<<" "<<d->getFrameTimeToTrigger(i);
	    //getchar();
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
	s<<"/RAW/DIF"<<d->getID()<<"/Asic"<<d->getFrameAsicHeader(i);
	TH1* han=rootHandler_->GetTH1(s.str()+"/Hits");
	TH1* han20=rootHandler_->GetTH1(s.str()+"/Hits20");
	TH1* hfr=rootHandler_->GetTH1(s.str()+"/Frequency");
	TH1* hframetime=rootHandler_->GetTH1(s.str()+"/FrameTime");
	TProfile* hmul=(TProfile*) rootHandler_->GetTH1(s.str()+"/Multiplicity");
	if (han==NULL)
	  {
	    printf("booking %s \n",s.str().c_str());
	    han =rootHandler_->BookTH1(s.str()+"/Hits",64,0.1,64.1);
	    han20 =rootHandler_->BookTH1(s.str()+"/Hits20",64,0.1,64.1);
	    hfr =rootHandler_->BookTH1(s.str()+"/Frequency",64,0.1,64.1);
	    hframetime =rootHandler_->BookTH1(s.str()+"/FrameTime",2000,0.,2000.);
	    hmul =rootHandler_->BookProfile(s.str()+"/Multiplicity",64,0.1,64.1,0.5,5.);
	  }
	hframetime->Fill(d->getFrameTimeToTrigger(i)*1.);
	uint32_t npad=0;
	for (uint32_t j=0;j<64;j++)
	   {
	     if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1))) continue;
	     npad++;
	   }
	 for (uint32_t j=0;j<64;j++)
	   {
	     if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1))) continue;
	     han->Fill(j*1.);
	     hmul->Fill(j*1.,npad*1.);
	     if (d->getFrameTimeToTrigger(i)<20)
	       han20->Fill(j*1.);
	   }
      }

      
      std::stringstream s0;
      s0<<"/TIME/DIF"<<d->getID();
      TH1* hfratime=rootHandler_->GetTH1(s0.str()+"/FrameTime");
      TH1* hnpd=rootHandler_->GetTH1(s0.str()+"/Npad");
  

      TH2* hmit=rootHandler_->GetTH2(s0.str()+"/HitMapIT");
      if (hmit==NULL)
	{
	  hfratime =rootHandler_->BookTH1(s0.str()+ "/FrameTime",2000,0.,2000.);
	  hnpd =rootHandler_->BookTH1( s0.str()+"/Npad",64,0.1,64.1);
	  hmit=rootHandler_->BookTH2(s0.str()+"/HitMapIT",6*8,0.1,48.1,4*8,0.1,32.1);
	}

      hfratime->Fill(dmin*1.);
   

      if (dmin>13.5 && dmin<20.5)
	{
	  printf("DIF %d : \n",d->getID());
	  uint32_t difid=d->getID();
	  uint32_t plan=_plid[difid];
	  
	  float zp=_plz[difid];
	  float dxp=_pldx[difid];
	  float dyp=_pldy[difid];

	for (uint32_t i=0;i<d->getNumberOfFrames();i++)
	  {

	    if (d->getFrameTimeToTrigger(i)==dmin) 
	      {
		npd=0;

		for (uint32_t j=0;j<64;j++)
		  {
		    if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1))) continue;
		    int asic=d->getFrameAsicHeader(i),channel=j+1,x=0,y=0;
		    //if (d->getID()==142 && asic>=11)
		    //  {printf("======================================================>%d %d \n",d->getID(),asic);
		    //	asic=asic+2;}
		    DifGeom::PadConvert(asic,channel,x,y,2);

		    if (d->getID()==142 )
		      {
			printf("%d %d %f %d %d \n",asic,channel,zp,x,y);
		      hpl->Fill(x*1.,y*1.);
		      hplx->Fill(zp,x*1.);
		      hply->Fill(zp,y*1.);
		      
		      plhit.set(plan);
		      }
		    /*
		    _x[_npBuf]=x;
		    _y[_npBuf]=y;
		    _z[_npBuf]=zp;
		    _layer[_npBuf]=plan;
		    _npBuf++;
		    */
		    hmit->Fill(x*1.,y*1.,1.);
		    npd++;
		  }
		hnpd->Fill(npd*1.);

	      }
	    if (d->getID()==142) goto mplot;
	    if (d->getFrameTimeToTrigger(i)>13.5 &&  d->getFrameTimeToTrigger(i)<20.5)
	      
	      for (uint32_t j=0;j<64;j++)
		{
		  if (!(d->getFrameLevel(i,j,0) || d->getFrameLevel(i,j,1))) continue;
		  int asic=d->getFrameAsicHeader(i),channel=j+1;
		  //printf("(%d,%d,%d)-",d->getFrameTimeToTrigger(i),asic,channel);
		  bool ap=false;
		  for (std::vector<TStripCluster*>::iterator it=_cl.begin();it!=_cl.end();it++)
		    {ap=ap || (*it)->append(asic,channel); if (ap) break;}
		  if (!ap)
		    {
		      TStripCluster* sc=new TStripCluster(asic,channel);
		      _cl.push_back(sc);
		    }
	
		}
	  }
	
	// printf("Strips hit %d \n",_cl.size());
	// for (std::vector<TStripCluster*>::iterator it=_cl.begin();it!=_cl.end();it++)
	//   printf("(%d-%f)-",(*it)->asic(),(*it)->x());
	// printf("\n");
	hpl->Reset();
	uint32_t np=0;
	for (int i=0;i<_cl.size();i++)
	  for(int j=i+1;j<_cl.size();j++)
	    {
	      if(_cl[i]->asic()==_cl[j]->asic()) continue;
	      for(int k=j+1;k<_cl.size();k++)
		{
		  if(_cl[i]->asic()==_cl[k]->asic()) continue;
		  if(_cl[j]->asic()==_cl[k]->asic()) continue;
		  np++;
		}
	    }
	/*
	uint32_t difid=d->getID();
	uint32_t plan=_plid[difid];

	float zp=_plz[difid];
	float dxp=_pldx[difid];
	float dyp=_pldy[difid];
	*/
	for (int i=0;i<_cl.size();i++)
	  for(int j=i+1;j<_cl.size();j++)
	    {
	      if(_cl[i]->asic()==_cl[j]->asic()) continue;
	      for(int k=j+1;k<_cl.size();k++)
		{
		  if(_cl[i]->asic()==_cl[k]->asic()) continue;
		  if(_cl[j]->asic()==_cl[k]->asic()) continue;
	      
	      
		  float x1=(_cl[j]->b()-_cl[i]->b())/(_cl[i]->a()-_cl[j]->a());
		  float y1=_cl[i]->a()*x1+_cl[i]->b();
		  float x2=(_cl[k]->b()-_cl[i]->b())/(_cl[i]->a()-_cl[k]->a());
		  float y2=_cl[i]->a()*x2+_cl[i]->b();
		  float x3=(_cl[k]->b()-_cl[j]->b())/(_cl[j]->a()-_cl[k]->a());
		  float y3=_cl[j]->a()*x3+_cl[j]->b();
		  float x=(x1+x2+x3)/3.;
		  float y=(y1+y2+y3)/3.;
		  if (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))>2) continue;
		  if (sqrt((x1-x3)*(x1-x3)+(y1-y3)*(y1-y3))>2) continue;
		  if (sqrt((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2))>2) continue;
		  _cl[i]->setUsed();
		  _cl[j]->setUsed();
		  _cl[k]->setUsed();
		  x+=dxp;
		  y+=dyp;
		  //printf("(%f,%f)-",x,y);
		  hpl->Fill(x,y);
		  hplx->Fill(zp,x);
		  hply->Fill(zp,y);
		  plhit.set(plan);
		  _x[_npBuf]=x;
		  _y[_npBuf]=y;
		  _z[_npBuf]=zp;
		  _layer[_npBuf]=plan;
		  _npBuf++;
		}
	    }
	for (int i=0;i<_cl.size();i++)
	  {
	    if (_cl[i]->isUsed()) continue;
	  for(int j=i+1;j<_cl.size();j++)
	    {
	      if (_cl[j]->isUsed()) continue;
	      if(_cl[i]->asic()==_cl[j]->asic()) continue;
	      float x=(_cl[j]->b()-_cl[i]->b())/(_cl[i]->a()-_cl[j]->a());
	      float y=_cl[i]->a()*x+_cl[i]->b();
	      _cl[i]->setUsed();
	      _cl[j]->setUsed();
	      x+=_pldx[d->getID()];
	      y+=_pldy[d->getID()];

	      //printf("(%f,%f)-",x,y);
	      plhit.set(_plid[d->getID()]);
	      hpl->Fill(x,y);
	      hplx->Fill(_plz[d->getID()],x);
	      hply->Fill(_plz[d->getID()],y);
	      _x[_npBuf]=x;
	      _y[_npBuf]=y;
	      _z[_npBuf]=zp;
	      _layer[_npBuf]=plan;
	      _npBuf++;
	    }
	    
	  }
	//printf("\n");

	}
    mplot:
#ifdef DRAWPLOT
      TCPlot->cd(_plid[d->getID()]);hpl->Draw("COLZ");ipl++;      
      TCPlot->Modified();
      TCPlot->Draw();
      TCPlot->Update();
#endif
      for (std::vector<TStripCluster*>::iterator it=_cl.begin();it!=_cl.end();it++)
	delete (*it);
    }
  if (_npBuf>=3) this->buildPrincipal("/Track");
#ifdef DRAWPLOT
  
  if (1)
    {
      TCPlot->cd(7);hplx->Draw("COLZ");
      TCPlot->cd(8);hply->Draw("COLZ");      
      TCPlot->Modified();
      TCPlot->Draw();
      TCPlot->Update();
      getchar();
    }
#endif
  hacqtime->Fill(theEventTotalTime_);
  theTotalTime_+=theEventTotalTime_;

  if (plhit[1] || plhit[2] || plhit[3] ||plhit[4]||plhit[5]) _nall++;
  if (plhit[1] && plhit[5])
    {
      _neff++;
      if (plhit[2]) _neff2++;
      if (plhit[3]) _neff3++;
      if (plhit[4]) _neff4++;
    }
  
  printf("Processing %d - %d GTC %d Total time %f Acquition time %f all %d [1-5] %d  [1-2-5]  %d [1-3-5] %d [1-4-5] %d \n",evt_->getRunNumber(),evt_->getEventNumber(),dbase->getGTC(),(dbase->getAbsoluteBCID()-theStartBCID_)*2E-7,theTotalTime_,_nall,_neff,_neff2,_neff3,_neff4);


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
	  s<<"/RAW/DIF"<<i+1<<"/Asic"<<j;
	  TH1* han=rootHandler_->GetTH1(s.str()+"/Hits");
	  TH1* hfr=rootHandler_->GetTH1(s.str()+"/Frequency");
	  if (han==NULL) continue;
	  for (uint32_t k=0;k<64;k++)
	     hfr->SetBinContent(k+1,han->GetBinContent(k+1)/theTotalTime_);


	  
	}
      
    }
  
  /*
  std::map<unsigned int,DifGeom>::iterator idg = reader_->getDifMap().find(d->getID());
  DifGeom& difgeom = idg->second;
  uint32_t chid = idg->second.getChamberId();
  */
  //  LCTOOLS::printParameters(rhcol->parameters());
  //DEBUG_PRINT("Time Stamp %d \n",evt_->getTimeStamp());
#endif
}
double mxmin[100];
double mxmax[100];
double mymin[100];
double mymax[100];


uint32_t TricotAnalyzer::buildPrincipal(std::string vdir)
{
  _tk.clear();
  uint32_t tkMinPoint_=2;
  uint32_t tkExtMinPoint_=2;
  uint32_t fch=0,lch=6;  
  std::stringstream st;
  st<<vdir<<"/";
  TH1* hnp0= rootHandler_->GetTH1(st.str()+"Np");
  TH1* hnp= rootHandler_->GetTH1(st.str()+"Npoints");
  TH1* hnpl= rootHandler_->GetTH1(st.str()+"Nplanes");
  TH1* hax= rootHandler_->GetTH1(st.str()+"ax");
  TH1* hay= rootHandler_->GetTH1(st.str()+"ay");
  TH1* hwt= rootHandler_->GetTH1(st.str()+"hitweight");

  if (hnp==NULL)
    {
      hnp0=  rootHandler_->BookTH1(st.str()+"Np",51,-0.1,50.9);
      hnp=  rootHandler_->BookTH1(st.str()+"Npoints",51,-0.1,50.9);
      hnpl=  rootHandler_->BookTH1(st.str()+"Nplanes",51,-0.1,50.9);
      hax=  rootHandler_->BookTH1(st.str()+"ax",200,-5.,5.);
      hay=  rootHandler_->BookTH1(st.str()+"ay",200,-5.,5.);


    }
  for (int ip=fch;ip<lch;ip++)
    {
      std::stringstream s;
      s<<st.str()<<"Plan"<<ip<<"/";
	      
      TH2* hall= rootHandler_->GetTH2(s.str()+"all");
      if (hall==NULL)
	{
	  hall= rootHandler_->BookTH2(s.str()+"all",100,-20.,80,100,-20.,80.);
	}
      for (uint32_t ic=0;ic<_npBuf;ic++)
	{
	  if (_layer[ic]!=ip) continue;
	  hall->Fill(_x[ic],_y[ic]);
	}
    }
  

  
  hnp0->Fill(_npBuf*1.);
  if (_npBuf>8) return 0;
  ShowerParams isha;
  //INFO_PRINT("Avant Principal %d\n",_npBuf);
  Shower::TPrincipalComponents(_npBuf,_x,_y,_z,_layer,(double*) &isha);



  //  if (theComputerTrack_->getTracks().size()>0) theNbTracks_++;
  uint32_t nmip=0;
  
  TrackInfo tk0;
  tk0.clear();
  double* x=isha.xm;
  double* v=isha.l2;
  // Z X
  double z0=x[2];
  double x0=x[0];
  double y0=x[1];

  double x1=x[0]+v[0];
  double y1=x[1]+v[1];
  double z1=x[2]+v[2];
  double ax,ay,bx,by;
  ax=(x1-x0)/(z1-z0);
  bx=x1-ax*z1;
  ay=(y1-y0)/(z1-z0);
  by=y1-ay*z1;
  tk0.set_ax(ax);
  tk0.set_bx(bx);
  tk0.set_ay(ay);
  tk0.set_by(by);
  //INFO_PRINT("Avant regression ");
  double cut=8.;
  for (uint32_t i=0;i<_npBuf;i++)
    {
      if (abs(tk0.closestApproach(_x[i],_y[i],_z[i]))<cut)
	{
	  // 3 hits on tag
	  if (_layer[i]>60)
	    printf(" bad stub %f %f %f %d \n",_x[i],_y[i],_z[i],_layer[i]);
	  if (!tk0.plane(_layer[i]))
	    tk0.add_point(_x[i],_y[i],_z[i],_layer[i]);

	}
      
    }
  
  if (tk0.size()<3)
    {
   return 0;
    }
  tk0.regression();
  //INFO_PRINT("Avant Second fit ");
  TrackInfo tk;_tk.clear();
  _tk.set_ax(tk0.ax());
  _tk.set_bx(tk0.bx());
  _tk.set_ay(tk0.ay());
  _tk.set_by(tk0.by());
  cut=8.;
  for (uint32_t i=0;i<_npBuf;i++)
    {
      if (abs(_tk.closestApproach(_x[i],_y[i],_z[i]))<cut)
	{
	  // 3 hits on tag
	  if (_layer[i]>60)
	    printf(" bad stub %f %f %f %d \n",_x[i],_y[i],_z[i],_layer[i]);
	  if (!_tk.plane(_layer[i]))
	    _tk.add_point(_x[i],_y[i],_z[i],_layer[i]);
	  if (_tk.size()>=3)
	    {
	      _tk.regression();
	      cut=8.;
	    }
	}
      
    }
  
  //INFO_PRINT("Apres second fit \n");
  if (_tk.size()<tkMinPoint_ || _tk.chi2x()>3 || _tk.chi2y()>3)
    {
   return 0;
    }

  //if (_tk.size()<minChambersInTime_) continue;
  //if (fabs(_tk.ax())<1.E-2) continue;
  //if (fabs(_tk.ax())<0.5 && fabs(_tk.ay())<0.5) theNbTracks_++;
  //this->draw(tk);
  //char c;c=getchar();putchar(c); if (c=='.') exit(0);

  

  // Calcul de l'efficacite

  // Track info
  
  hnp->Fill(_tk.size()*1.);

  hax->Fill(_tk.ax());
  hay->Fill(_tk.ay());

  for (int ip=fch;ip<=lch;ip++)
    if (_tk.plane(ip)) hnpl->Fill(ip*1.);
  //std::cout<<_tk.planes()<<std::endl;
  //getchar();
    
  for (uint32_t ip=fch;ip<=lch;ip++)
    {
      //INFO_PRINT("Plan %d studied \n",ip);
      TrackInfo tex;
	      
      _tk.exclude_layer(ip,tex);
      
      uint32_t npext=tex.size();
      
      if (npext<tkExtMinPoint_) continue; // Au moins 4 plans dans l'estrapolation touches 
      /*
      if (ip>1 && !tex.plane(ip-1)) continue;
      //if (ip>2 && !tex.plane(ip-2)) continue;
      if (ip==1 &&  !( tex.plane(ip+1) &&tex.plane(ip+2))) continue;
      if (ip<lch && !tex.plane(ip+1)) continue;
      if (ip==lch &&  !( tex.plane(ip-1) &&tex.plane(ip-2))) continue;
      //if (ip<(lch-1) && !tex.plane(ip+2)) continue;
      */
	//if (npext<minChambersInTime_) continue;
      
      std::stringstream s;
      s<<st.str()<<"Plan"<<ip<<"/";
	      
      TH2* hext= rootHandler_->GetTH2(s.str()+"ext");
      TH2* hfound= rootHandler_->GetTH2(s.str()+"found");
      TH2* hnear= rootHandler_->GetTH2(s.str()+"near");
      TH2* hfound1= rootHandler_->GetTH2(s.str()+"found1");
      TH2* hfound2= rootHandler_->GetTH2(s.str()+"found2");
      TH2* hmul= rootHandler_->GetTH2(s.str()+"mul");
      TH1* hdx= rootHandler_->GetTH1(s.str()+"dx");
      TH1* hdy= rootHandler_->GetTH1(s.str()+"dy");
      TH2* hdxy= rootHandler_->GetTH2(s.str()+"dxy");
      TH2* hmiss= rootHandler_->GetTH2(s.str()+"missing");
      float dz0=0.,distz=60.; // 2.8
      float xext=tex.xext(dz0+(ip-1)*distz);
      float yext =tex.xext(dz0+(ip-1)*distz); 
      if (hext==NULL)
	{
		  
	  double xi=1000,xa=-1000,yi=1000,ya=-1000;
	  mxmin[ip]=0.+_pldx[_pldif[ip]]-3.;
	  mxmax[ip]=30.+_pldx[_pldif[ip]]+3.;
	  mymin[ip]=0.+_pldy[_pldif[ip]]-3.;;
	  mymax[ip]=30.+_pldy[_pldif[ip]]+3.;;
	  xi=mxmin[ip];
	  yi=mymin[ip];
	  xa=mxmax[ip];
	  ya=mymax[ip];
	  int nx=50;
	  int ny=50.;

	  hext= rootHandler_->BookTH2(s.str()+"ext",nx,xi,xa,ny,yi,ya);
	  hfound= rootHandler_->BookTH2(s.str()+"found",nx,xi,xa,ny,yi,ya);
	  hnear= rootHandler_->BookTH2(s.str()+"near",nx,xi,xa,ny,yi,ya);
	  hfound1= rootHandler_->BookTH2(s.str()+"found1",nx,xi,xa,ny,yi,ya);
	  hfound2= rootHandler_->BookTH2(s.str()+"found2",nx,xi,xa,ny,yi,ya);
	  hmiss= rootHandler_->BookTH2(s.str()+"missing",nx,xi,xa,ny,yi,ya);
	  hmul= rootHandler_->BookTH2(s.str()+"mul",nx,xi,xa,ny,yi,ya);
	  hdxy= rootHandler_->BookTH2(s.str()+"dxy",50,-2.,2.,50,-2.,2.);
	  hdx=  rootHandler_->BookTH1(s.str()+"dx",400,-4.,4.);
	  hdy=  rootHandler_->BookTH1(s.str()+"dy",400,-4.,4.);
	}
      float chamberEdge_=0;
      xext=tex.xext(_plz[_pldif[ip]]);
      yext=tex.yext(_plz[_pldif[ip]]);

      //      if (xext<mxmin[ip]+chamberEdge_ || xext>mxmax[ip]-chamberEdge_) continue;
      //if (yext<mymin[ip]+chamberEdge_ || yext>mymax[ip]-chamberEdge_) continue;
      if (xext<_plaxi[ip]) continue;
      if (yext<_playi[ip]) continue;
      if (xext>_plaxa[ip]) continue;
      if (yext>_playa[ip]) continue;
      
      hext->Fill(xext,yext);
      //bool 
      float dist=1E9;
      bool th1=false,th2=false;
      float dxi,dyi,xn,yn,nhi=0;
      for (uint32_t ic=0;ic<_npBuf;ic++)
	{
	  if (_layer[ic]!=ip) continue;
	  //xext=tex.xext(_z[ic]);
	  //yext=tex.yext(_z[ic]);
	  float dx=_x[ic]-xext;
	  float dy=_y[ic]-yext;

	  double dap=tex.closestApproach(_x[ic],_y[ic],_z[ic]);
	  //  DEBUG_PRINT(" (%f,%f,%f) %f %f \n",x,y,z,dap,sqrt(dx*dx+dy*dy));
	  //getchar();
	  if (dap<dist)
	    {
		      
	      dist=dap;
	      dxi=dx;
	      dyi=dy;
	      nhi=1;
	      xn=_x[ic];
	      yn=_y[ic];
	      th1=false,th2=false;
	    }

	}
      // Cut a 1.5 au lieu de 6
      if (dist<6.)
	{
	  hdx->Fill(dxi);
	  hdy->Fill(dyi);
	  hdxy->Fill(dxi,dyi);
	  hmul->Fill(xext,yext,nhi*1.);
	  hfound->Fill(xext,yext);
	  hnear->Fill(xn,yn);
	  if (th1||th2)  hfound1->Fill(xext,yext);
	  if (th2)  hfound2->Fill(xext,yext);
	}
      else
	 hmiss->Fill(xext,yext);
    }



  //this->draw(tk);

	
   DEBUG_PRINT("==> MIPS hit %d -> %.2f\n",nmip,nmip*100./vrh.size()); 
 
 
 
   return 1;
}
class hitinfo {
public:
  hitinfo(){_plan=0;memset(padding,0,7);}
  float _x,_y,_z;
  uint8_t _type,_plan,_dif,_asic,_channel;
  uint8_t padding[7];
  
};
void TricotAnalyzer::dumpSeed(std::vector<ptrDifFrame> &vf)
{
  std::vector<hitinfo> vhi;
  vhi.clear();
   for (std::vector<ptrDifFrame>::iterator ip=vf.begin();ip!=vf.end();ip++)
    {
	  //uint32_t idh=imfind(ip->first->getID());
      DIFPtr* d=ip->first;
      uint32_t iframe=ip->second;
      //printf("DIF %d :  frame %d \n",d->getID(),iframe);
      uint32_t difid=d->getID();
      uint32_t plan=_plid[difid];
      
      float zp=_plz[difid];
      float dxp=_pldx[difid];
      float dyp=_pldy[difid];


      if (_pltyp[difid]==0)
	{
	  for (uint32_t j=0;j<64;j++)
	    {

	      if (!(d->getFrameLevel(iframe,j,0) || d->getFrameLevel(iframe,j,1))) continue;
	      hitinfo hi;
	      hi._z=_plz[difid];
	      hi._dif=difid;
	      hi._plan=plan;
	      hi._type=0;
	      int asic=d->getFrameAsicHeader(iframe),channel=j,ix=0,iy=0;
	      hi._asic=asic;
	      hi._channel=channel;
		    //if (d->getID()==142 && asic>=11)
		    //  {printf("======================================================>%d %d \n",d->getID(),asic);
		    //	asic=asic+2;}
	      DifGeom::PadConvert(asic,channel,ix,iy,2);
	      hi._x=ix;
	      hi._y=iy;
	      vhi.push_back(hi);
	    }
	}

      if (_pltyp[difid]==1)
	{
	  //continue;
	  for (uint32_t j=0;j<64;j++)
	    {
	      if (!(d->getFrameLevel(iframe,j,0) || d->getFrameLevel(iframe,j,1))) continue;
	      int asic=d->getFrameAsicHeader(iframe),channel=j+1,ix=0,iy=0;
		    //if (d->getID()==142 && asic>=11)
		    //  {printf("======================================================>%d %d \n",d->getID(),asic);
		    //	asic=asic+2;}
	      
	      // printf("DIF ID is %d \n",difid);
	      hitinfo hi;
	      hi._z=_plz[difid];
	      hi._dif=difid;
	      hi._plan=plan;
	      hi._type=1;
	      hi._asic=asic;
	      hi._channel=channel;
	      hi._x=0;
	      hi._y=0;
	      vhi.push_back(hi);
	    }
	 
	}
    }
   printf("%d %d %d %d\n",evt_->getRunNumber(),evt_->getEventNumber(),_currentSeed,vhi.size());
   if (_fdOut>0)
     {
       uint32_t run=evt_->getRunNumber();
       uint32_t event=evt_->getEventNumber();
       uint32_t ns=vhi.size();
       int ier=write(_fdOut,&run,sizeof(uint32_t));
       ier=write(_fdOut,&event,sizeof(uint32_t));
       ier=write(_fdOut,&_currentSeed,sizeof(uint32_t));
       ier=write(_fdOut,&ns,sizeof(uint32_t));
       for (int i=0;i<vhi.size();i++)
	 {
	 printf("%d %d %d %d %d %d %f %f %f\n",i,vhi[i]._type,vhi[i]._plan,vhi[i]._dif,vhi[i]._asic,vhi[i]._channel,vhi[i]._x,vhi[i]._y,vhi[i]._z);
	 ier=write(_fdOut,&vhi[i],sizeof(hitinfo));
	 }
     }
}
void TricotAnalyzer::processOneSeed(std::vector<ptrDifFrame> &vf)
{
  
  std::map<uint32_t,std::vector<TStripCluster> > mapTStripCluster;
  std::map<uint32_t,std::vector<PadCluster> > mapPadCluster;
  mapTStripCluster.clear();
  mapPadCluster.clear();
  for (std::vector<ptrDifFrame>::iterator ip=vf.begin();ip!=vf.end();ip++)
    {
	  //uint32_t idh=imfind(ip->first->getID());
      DIFPtr* d=ip->first;
      uint32_t iframe=ip->second;
      //printf("DIF %d :  frame %d \n",d->getID(),iframe);
      uint32_t difid=d->getID();
      uint32_t plan=_plid[difid];
      
      float zp=_plz[difid];
      float dxp=_pldx[difid];
      float dyp=_pldy[difid];

      

      if (_pltyp[difid]==0)
	{
	  for (uint32_t j=0;j<64;j++)
	    {
	      if (!(d->getFrameLevel(iframe,j,0) || d->getFrameLevel(iframe,j,1))) continue;
	      int asic=d->getFrameAsicHeader(iframe),channel=j,ix=0,iy=0;
		    //if (d->getID()==142 && asic>=11)
		    //  {printf("======================================================>%d %d \n",d->getID(),asic);
		    //	asic=asic+2;}
	      DifGeom::PadConvert(asic,channel,ix,iy,2);
	      
	      std::map<uint32_t,std::vector<PadCluster> >::iterator imp=mapPadCluster.find(difid);
	      if (imp==mapPadCluster.end())
		{
		  mapPadCluster.insert(std::make_pair(difid,std::vector<PadCluster>()));
		  imp=mapPadCluster.find(difid);
		}
	      
	      std::vector<PadCluster> &vs=imp->second;
	      //printf("%x vs size %d \n",&vs,vs.size());
	      bool append=false;
	      for (std::vector<PadCluster>::iterator ipc=vs.begin();ipc!=vs.end();ipc++)
		{
		  if (ipc->append(plan,ix,iy)) { append=true;break;}
		}
	      if (!append)
		{
		  PadCluster c(plan,ix,iy);
		  vs.push_back(PadCluster(plan,ix,iy));
		  //printf("after push %d %d %d \n",plan,ix,iy);
		  //c.dump();
		}
	    }
	}

      if (_pltyp[difid]==1)
	{
	  //continue;
	  for (uint32_t j=0;j<64;j++)
	    {
	      if (!(d->getFrameLevel(iframe,j,0) || d->getFrameLevel(iframe,j,1))) continue;
	      int asic=d->getFrameAsicHeader(iframe),channel=j+1,ix=0,iy=0;
		    //if (d->getID()==142 && asic>=11)
		    //  {printf("======================================================>%d %d \n",d->getID(),asic);
		    //	asic=asic+2;}
	      
	      // printf("DIF ID is %d \n",difid);
	      std::map<uint32_t,std::vector<TStripCluster> >::iterator imp=mapTStripCluster.find(difid);
	      if (imp==mapTStripCluster.end())
		{
		  mapTStripCluster.insert(std::make_pair(difid,std::vector<TStripCluster>()));
		  imp=mapTStripCluster.find(difid);
		}
	      
	      std::vector<TStripCluster> &vs=imp->second;
	      // printf("%x vs size %d \n",&vs,vs.size());
	      bool append=false;
	      if (vs.size()!=0)
		for (std::vector<TStripCluster>::iterator ipc=vs.begin();ipc!=vs.end();ipc++)
		  {
		    if (ipc->append(asic,channel)) { append=true;break;}
		  }
	      if (!append)
		{

		  vs.push_back(TStripCluster(asic,channel));
		  //  printf("after push %x vs size %d \n",&vs,vs.size());
		}
	    }
	 
	}
    }
  INFO_PRINT("Map build \n");
  //Bow build point
  _npBuf=0;
  for (  std::map<uint32_t,std::vector<TStripCluster> >::iterator imp=mapTStripCluster.begin();imp!=mapTStripCluster.end();imp++)
    {
      uint32_t difid=imp->first;
      std::vector<TStripCluster>& _cl=imp->second;

      uint32_t plan=_plid[difid];
      
      float zp=_plz[difid];
      float dxp=_pldx[difid];
      float dyp=_pldy[difid];	
      for (int i=0;i<_cl.size();i++)
	for(int j=i+1;j<_cl.size();j++)
	  {
	    if(_cl[i].asic()==_cl[j].asic()) continue;
	    for(int k=j+1;k<_cl.size();k++)
		{
		  if(_cl[i].asic()==_cl[k].asic()) continue;
		  if(_cl[j].asic()==_cl[k].asic()) continue;
		  //printf("Strip cluster conjonction ; %d %d %d \n",_cl[i].asic(),_cl[j].asic(),_cl[k].asic());
		  //printf("\t a ; %f %f %f \n",_cl[i].a(),_cl[j].a(),_cl[k].a());
		  //printf("\t b ; %f %f %f \n",_cl[i].b(),_cl[j].b(),_cl[k].b());
	      
		  float x1=(_cl[j].b()-_cl[i].b())/(_cl[i].a()-_cl[j].a());
		  float y1=_cl[i].a()*x1+_cl[i].b();
		  float x2=(_cl[k].b()-_cl[i].b())/(_cl[i].a()-_cl[k].a());
		  float y2=_cl[i].a()*x2+_cl[i].b();
		  float x3=(_cl[k].b()-_cl[j].b())/(_cl[j].a()-_cl[k].a());
		  float y3=_cl[j].a()*x3+_cl[j].b();
		  float x=(x1+x2+x3)/3.;
		  float y=(y1+y2+y3)/3.;
		  if (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))>2) continue;
		  if (sqrt((x1-x3)*(x1-x3)+(y1-y3)*(y1-y3))>2) continue;
		  if (sqrt((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2))>2) continue;
		  _cl[i].setUsed();
		  _cl[j].setUsed();
		  _cl[k].setUsed();
		  x+=dxp;
		  y+=dyp;
		  //printf("(%f,%f)-",x,y);
		  _x[_npBuf]=x;
		  _y[_npBuf]=y;
		  _z[_npBuf]=zp;
		  _layer[_npBuf]=plan;
		  _npBuf++;
		}
	    }
	for (int i=0;i<_cl.size();i++)
	  {
	    if (_cl[i].isUsed()) continue;
	  for(int j=i+1;j<_cl.size();j++)
	    {
	      if (_cl[j].isUsed()) continue;
	      if(_cl[i].asic()==_cl[j].asic()) continue;
	      float x=(_cl[j].b()-_cl[i].b())/(_cl[i].a()-_cl[j].a());
	      float y=_cl[i].a()*x+_cl[i].b();
	      _cl[i].setUsed();
	      _cl[j].setUsed();
	      x+=dxp;
	      y+=dyp;
	      _x[_npBuf]=x;
	      _y[_npBuf]=y;
	      _z[_npBuf]=zp;
	      _layer[_npBuf]=plan;
	      _npBuf++;
	    }
	  }
    }
  // pad cluster
	//printf("\n");
   for (  std::map<uint32_t,std::vector<PadCluster> >::iterator imp=mapPadCluster.begin();imp!=mapPadCluster.end();imp++)
    {
      uint32_t difid=imp->first;
      std::vector<PadCluster>& _cl=imp->second;

      uint32_t plan=_plid[difid];
      
      float zp=_plz[difid];
      float dxp=_pldx[difid];
      float dyp=_pldy[difid];
      for (int i=0;i<_cl.size();i++)
	{
	  if (_cl[i].isUsed()) continue;
	  _cl[i].setUsed();
	  //_cl[i].dump();
	  _x[_npBuf]=_cl[i].x()+dxp;
	  _y[_npBuf]=_cl[i].y()+dyp;
	  _z[_npBuf]=zp;
	  _layer[_npBuf]=plan;
	  _npBuf++;
	}
    }
    
   for (int i=0;i<_npBuf;i++)
     {
       printf("Plan %d : %f %f %f \n",_layer[i],_x[i],_y[i],_z[i]);
     }
   //getchar();
}
void TricotAnalyzer::buildFrameMap(std::vector<uint32_t> &seeds)
{
        // Fill std::map<uint32_t,std::vector<IMPL::RawCalorimeterHitImpl*> > _FrameMap_;
  _FrameMap.clear();
  /*
  for (std::vector<uint32_t>::iterator is=seeds.begin();is!=seeds.end();is++)
    {
      std::vector<ptrDifFrame> v;
      
      std::pair<uint32_t,std::vector<ptrDifFrame> > p((*is),v);
      _FrameMap.insert(p);
    }
  */
  std::map<uint32_t,std::vector<ptrDifFrame> >::iterator im=_FrameMap.end();
  for (std::vector<DIFPtr*>::iterator it = reader_->getDIFList().begin();it!=reader_->getDIFList().end();it++)
    {
      DIFPtr* d = (*it);
      if (d->getID()>255) continue;

     
      for (uint32_t i=0;i<d->getNumberOfFrames();i++)
      {
	if (d->getID()<256)
	  {
	    //printf("%d ",d->getFrameTimeToTrigger(i));
	    int32_t idt=d->getBCID()-d->getFrameBCID(i);
	    if (idt<0)
	      printf(" error frame %d  FBCID %d  ASIC %d DIF %d  BCID %d\n",i,d->getFrameBCID(i),d->getFrameAsicHeader(i),d->getID(),d->getBCID());
	  }
	ptrDifFrame hit(d,i);
	im=_FrameMap.find(d->getFrameTimeToTrigger(i));
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)-1);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)+1);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)-2);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)+2);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
#ifdef ENHANCED_TIME_WINDOW
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)-3);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)+3);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)-4);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
	im=_FrameMap.find(d->getFrameTimeToTrigger(i)+4);
	if (im!=_FrameMap.end()) {im->second.push_back(hit);continue;}
#endif
	std::vector<ptrDifFrame> v;
      
	std::pair<uint32_t,std::vector<ptrDifFrame> > p(d->getFrameTimeToTrigger(i),v);
      _FrameMap.insert(p);
      }
    }
      seeds.clear();
      // Check that at least nasic_min DIfs are hit per seed
      for( std::map<uint32_t,std::vector<ptrDifFrame> >::iterator im=_FrameMap.begin(),im_next=im;im!=_FrameMap.end();im=im_next)
	{
	  ++im_next;
	  std::bitset<16> difs;
	  difs.reset();
	  for (std::vector<ptrDifFrame>::iterator ih=im->second.begin();ih!=im->second.end();ih++)
	    difs.set((uint8_t) _plid[ih->first->getID()]);
	  // if (difs.count()>=nasic_min)
	  //   printf("seed %d ndif  %d \n",im->first,difs.count());
	  if (difs.count()<2)
	    _FrameMap.erase(im);
	  else
	    {
	      //std::cout<<im->first<<" "<<difs.to_string()<<std::endl;
	      /*
	      if (im->first<20)
		for (std::vector<ptrDifFrame>::iterator ih=im->second.begin();ih!=im->second.end();ih++)
		  {
		    uint32_t difid=ih->first->getID();
		    DIFPtr* d=ih->first;
		    std::cout<<"\t "<<difid<<" "<<(uint32_t) _plid[difid]<<" "<<d->getFrameTimeToTrigger(ih->second)<<" "<<d->getFrameAsicHeader(ih->second)<<std::endl;
		  }
	      */
	    seeds.push_back(im->first);
	    }
	}
      //printf("%s EventMap size %d seeds %d \n",__PRETTY_FUNCTION__,_FrameMap.size(),theTimeSeeds_.size());


}

void TricotAnalyzer::ShowTracks()
{
  if (TCPlot==NULL)
    {
      TCPlot=new TCanvas("TCPlotR","tcplotr1",900,900);
      TCPlot->Modified();
      TCPlot->Draw();
      TCPlot->Divide(1,2);
    }

  TH2* htx = rootHandler_->GetTH2("tx");
  TH2* hty = rootHandler_->GetTH2("ty");
  if (htx==NULL)
    {
      htx =rootHandler_->BookTH2("tx",100,0.,100.,100,-15.,85.);
      hty =rootHandler_->BookTH2("ty",100,0.,100.,100,-15.,85.);

    }
  else
    {
      htx->Reset();
      hty->Reset();
    }

  for (int i=0;i<_npBuf;i++)
    {
      htx->Fill(_z[i],_x[i]);
      hty->Fill(_z[i],_y[i]);
    }
  TCPlot->cd(1);
  htx->Draw("COLZ");
  if (_tk.size())
    {
      printf("Chi2 ====> %f %f \n",_tk.chi2x(),_tk.chi2y());
      TLine* l = new TLine(0.,0.*_tk.ax()+_tk.bx(),100.,100.*_tk.ax()+_tk.bx());
      l->SetLineColor(4);
      l->Draw("SAME");
    }
  TCPlot->Update();
  TCPlot->Modified();

  TCPlot->cd(2);
  hty->Draw("COLZ");
    if (_tk.size())
    {
      TLine* l = new TLine(0.,0.*_tk.ay()+_tk.by(),100.,100.*_tk.ay()+_tk.by());
      l->SetLineColor(4);
      l->Draw("SAME");
    }

  TCPlot->Update();
  TCPlot->Modified();
  TCPlot->Update();
  TCPlot->Modified();


  char c;c=getchar();putchar(c); if (c=='.') exit(0);;

  
}
