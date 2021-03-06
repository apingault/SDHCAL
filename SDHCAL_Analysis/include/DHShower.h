#ifndef _DHSHOWER_H

#define _DHSHOWER_H
#include <limits.h>
#include <stdint.h>
#include <vector>
#include <map>

#include <Array3D.h>
#include <Amas.h>



class DHShower
{
public:
  DHShower(RecoHit *h);
  ~DHShower();
  void clear();
  bool append(RecoHit* h,float dist_cut);
  void Add(RecoHit* h);
  double Distance(RecoHit* h);
  void summarizeNumberOfHits();
  uint32_t getNumberOfHits(uint32_t plan,uint32_t threshold);
  double getCorrectedNumberOfHits(uint32_t plan,uint32_t threshold,std::map<uint32_t,double*> &corr);   
  uint32_t getReduceNumberOfHits(uint32_t threshold,uint32_t fp=0,uint32_t lp=100);
  uint32_t getNumberOfHits(uint32_t threshold);  
  uint32_t getEdge(uint32_t threshold);
  uint32_t getCore(uint32_t threshold);
  uint32_t getMip(uint32_t threshold);
  uint32_t getNumberOfMips(uint32_t plan);
  uint32_t getFDHits(uint32_t* v,uint32_t thr);
  uint32_t getFDHitsN(uint32_t* v,uint32_t thr);
  std::map<uint32_t,std::vector<RecoHit*> >& getPlans(){return thePlans_;}
  void PlayMatrix(uint32_t fp=1,uint32_t lp=60);
  void transverseProfile(uint32_t plan,uint32_t &nh,double &xb,double &yb, double &l0, double &l1,double* v0,double *v1,double &n9,double &n25);
  static void computePrincipalComponents(std::vector<RecoHit*> &v, double result[21]);
  double getl1(){return l1_;}
  double getl2(){return l2_;}
  double getl3(){return l3_;}
  double* getv1(){return v1_;}
  double* getv2(){return v2_;}
  double* getv3(){return v3_;}
  double* getxm(){return xm_;}
  void setSelected(bool t){selected_=t;}
  bool isSelected(){return selected_;}
  double closestDistance(DHShower& sh);
  uint32_t getFirstPlan(){return firstPlan_;}
  uint32_t getLastPlan(){return lastPlan_;}
  void drawDisplay();
  inline uint32_t getNGood(){return ng_;}
  inline uint32_t getNAfter(){return nafter_;}
  inline double getZFirstAmas(){return zfirst_;}
  inline double getZLastAmas(){return zlast_;}
  inline std::vector <Amas>& getAmas(){return theAmas_;} 
private:
  std::map<uint32_t,std::vector<RecoHit*> > thePlans_;
  double l1_,l2_,l3_;
  double v1_[3],v2_[3],v3_[3];
  double xm_[3];
  bool selected_;
  uint32_t firstPlan_,lastPlan_;
  uint32_t ng_,nafter_;
  double zfirst_,zlast_;
  std::vector <Amas> theAmas_;
  uint32_t theHitNumber_[540];
};
#endif
