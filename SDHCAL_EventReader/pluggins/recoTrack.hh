#ifndef _RECOTRACK_HH

#define _RECOTRACK_HH
#include <limits.h>
#include <stdint.h>
#include <vector>
#include <TLine.h>
#include "jsonGeo.hh"
#include "planeCluster.hh"

#include <Math/PositionVector3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3Dfwd.h>
#include <Math/DisplacementVector3D.h>
#include <ostream>
#include <bitset>

class recoTrack
{
public:
  recoTrack();
  ~recoTrack(){;}
  recoTrack(const recoTrack& t)
  {
    for (uint32_t i=0;i<t.size();i++)
      _points.push_back(t.at(i));
    _orig=t.orig();
    _dir=t.dir();
    _chi2=t.chi2();
    _pchi2=t.pchi2();
    zmin_=t.zmin();
    zmax_=t.zmax();
  }



  TLine* linex();
  TLine* liney();
  void clear();
  //double cap(recoTrack& t);
  void cap(recoTrack& t,double &d, ROOT::Math::XYZPoint &p1, ROOT::Math::XYZPoint &p2);
  void clean(float cut);
  void addPoint(ROOT::Math::XYZPoint* p);
  void remove(ROOT::Math::XYZPoint* p);
  double distance(ROOT::Math::XYZPoint* p);
  ROOT::Math::XYZPoint extrapolate(double z);
  void regression();
  inline bool isValid(){return _valid;}
  inline void setValid(bool t){_valid=t;}
  inline uint32_t size() const {return _points.size();}
  ROOT::Math::XYZPoint orig() const {return _orig;}
  ROOT::Math::XYZVector dir() const{return _dir;}
  ROOT::Math::XYZPoint* at(uint32_t i) const {return _points[i];}
  std::vector<ROOT::Math::XYZPoint*>& points() { return _points;}
  void Dump( std::ostream &os=std::cout );
  void setChi2(double chi2);
  inline double chi2() const {return _chi2;}
  inline double pchi2() const {return _pchi2;}
  inline float zmin() const { return zmin_;}
  inline float zmax() const { return zmax_;}
  friend std::ostream &operator<<( std::ostream &os, 
                                       const recoTrack &obj)
  {
    // write obj to stream
    os<<" X:"<<obj.orig().X();
    os<<" Y:"<<obj.orig().Y();
    os<<" Z:"<<obj.orig().Z();
    os<<" AX:"<<obj.dir().X();
    os<<" AY:"<<obj.dir().Y()<<std::endl;
    os<<" N points : "<<obj.size()<<std::endl;
    return os;
  }
  static void combine(std::vector<planeCluster*> pc,jsonGeo* g,std::vector<recoTrack*>& vtk);
  static void combinePoint(std::vector<recoPoint*> pc,jsonGeo* g,  std::vector<recoTrack*> &vtk);
  static void combinePoint1(std::vector<recoPoint*> pc,jsonGeo* g,  std::vector<recoTrack*> &vtk);
  void getChi2(std::vector<planeCluster*> clusters);
  void getChi2(std::vector<recoPoint*> clusters);

private:
  bool _valid;
  float zmin_,zmax_;
  std::vector<ROOT::Math::XYZPoint*> _points;
  ROOT::Math::XYZVector _dir;
  ROOT::Math::XYZPoint  _orig;
  double _chi2,_pchi2;

};
#endif
