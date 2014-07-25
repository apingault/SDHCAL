#ifndef _DimDbServer_h

#define _DimDbServer_h
#include <iostream>

#include <string.h>
#include<stdio.h>
#include "dis.hxx"
#include "OracleDIFDBManager.h"
#include "DIFReadoutConstant.h"

using namespace std;
#include <sstream>


class DimDbServer: public DimServer
{
public:
  DimDbServer();
  void allocateCommands();
  void commandHandler();
  enum State {ALIVED=1,DOWNLOADED=2,DELETED=3,FAILED=99};
 
  void doDownload(std::string state);
  void doDelete();
private:
  int32_t processStatus_;
  std::string state_;

  DIFDbInfo difInfos_[255];
  DimService* aliveService_; //State of the process 
  DimService* difServices_[255]; //State of the last register read 
  DimCommand *downloadCommand_;
  DimCommand *deleteCommand_;
  DimCommand *startCommand_;
 
  OracleDIFDBManager* theDBManager_;
};
#endif
