#!/usr/bin/env python
import os
import socks
import socket
import httplib, urllib,urllib2
from urllib2 import URLError, HTTPError
import json
from copy import deepcopy

import time
import argparse

def parseReturn(command,sr):
    if (command=="jobStatus"):
        #s=r1.read()
        #print s["answer"]
        sj=json.loads(sr)
        #sj=s
        #ssj=json.loads(sj["answer"]["ANSWER"])
        ssj=sj["answer"]["ANSWER"]
        print "\033[1m %6s %15s %25s %20s \033[0m" % ('PID','NAME','HOST','STATUS')
        for x in ssj:
            if (x['DAQ']=='Y'):
                print "%6d %15s %25s %20s" % (x['PID'],x['NAME'],x['HOST'],x['STATUS'])
    if (command=="hvStatus"):
        sj=json.loads(sr)
        ssj=sj["answer"]["ANSWER"]
        #print ssj
        print "\033[1m %5s %10s %10s %10s %10s \033[0m" % ('Chan','VSET','ISET','VOUT','IOUT')
        for x in ssj:
            print "#%.4d %10.2f %10.2f %10.2f %10.2f" % (x['channel'],x['vset'],x['iset'],x['vout'],x['iout'])
    if (command=="LVStatus"):
        sj=json.loads(sr)
        
        ssj=sj["answer"]["LVSTATUS"]
        print "\033[1m %10s %10s %10s \033[0m" % ('VSET','VOUT','IOUT')
        print " %10.2f %10.2f %10.2f" % (ssj['vset'],ssj['vout'],ssj['iout'])
    if (command=="status" and not results.verbose):

        sj=json.loads(sr)
        ssj=sj["answer"]["diflist"]
        print "\033[1m %4s %5s %6s %12s %12s %15s  %s \033[0m" % ('DIF','SLC','EVENT','BCID','BYTES','SERVER','STATUS')

        for d in ssj:
            #print d
            #for d in x["difs"]:
            print '#%4d %5x %6d %12d %12d %15s %s ' % (d["id"],d["slc"],d["gtc"],d["bcid"],d["bytes"],d["host"],d["state"])
    if (command=="dbStatus" ):
        sj=json.loads(sr)
        ssj=sj["answer"]
        print "\033[1m %10s %10s \033[0m" % ('Run','State')
        print " %10d %s " % (ssj['run'],ssj['state'])
    if (command=="shmStatus" ):
        sj=json.loads(sr)
        ssj=sj["answer"]
        print "\033[1m %10s %10s \033[0m" % ('Run','Event')
        print " %10d %10d " % (ssj['run'],ssj['event'])
    if (command=="state"):
        sj=json.loads(sr)
        print "\033[1m State \033[0m :",sj["STATE"]
        scm=""
        for z in sj["CMD"]:
            scm=scm+"%s:" % z["name"]
        scf=""
        for z in sj["FSM"]:
            scf=scf+"%s:" % z["name"]

        print "\033[1m Commands \033[0m :",scm
        print "\033[1m F S M \033[0m :",scf
    if (command=="triggerStatus"):
          
        sj=json.loads(sr)
        ssj=sj["answer"]["COUNTERS"]
        print "\033[1m %10s %10s %10s %10s %12s %12s %10s %10s %10s \033[0m" % ('Spill','Busy1','Busy2','Busy3','SpillOn','SpillOff','Beam','Mask','EcalMask')
        print " %10d %10d %10d %10d  %12d %12d %12d %10d %10d " % (ssj['spill'],ssj['busy1'],ssj['busy2'],ssj['busy3'],ssj['spillon'],ssj['spilloff'],ssj['beam'],ssj['mask'],ssj['ecalmask'])
    if (command=="difLog" or command=="cccLog" or command=="mdccLog" or command =="zupLog"):
          
        sj=json.loads(sr)
        print  "\033[1m %s \033[0m" % sj["answer"]["FILE"]
        ssj=sj["answer"]["LINES"]
        print ssj
        #print "\033[1m %10s %10s %10s %10s %12s %12s %10s %10s %10s \033[0m" % ('Spill','Busy1','Busy2','Busy3','SpillOn','SpillOff','Beam','Mask','EcalMask')
        #print " %10d %10d %10d %10d  %12d %12d %12d %10d %10d " % (ssj['spill'],ssj['busy1'],ssj['busy2'],ssj['busy3'],ssj['spillon'],ssj['spilloff'],ssj['beam'],ssj['mask'],ssj['ecalmask'])

        
def executeFSM(host,port,prefix,cmd,params):
   if (params!=None):
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       #if (name!=None):
       #    lq['name']=name
       #if (value!=None):
       #    lq['value']=value
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       lq={}
       
       lq["content"]=json.dumps(params,sort_keys=True)
       #for x,y in params.iteritems():
       #    lq["content"][x]=y
       lq["command"]=cmd           
       lqs=urllib.urlencode(lq)
       #print lqs
       saction = '/%s/FSM?%s' % (prefix,lqs)
       myurl=myurl+saction
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       return r1.read()

def executeCMD(host,port,prefix,cmd,params):
   if (params!=None and cmd!=None):
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       #if (name!=None):
       #    lq['name']=name
       #if (value!=None):
       #    lq['value']=value
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       lq={}
       lq["name"]=cmd
       for x,y in params.iteritems():
           lq[x]=y
       lqs=urllib.urlencode(lq)
       saction = '/%s/CMD?%s' % (prefix,lqs)
       myurl=myurl+saction
       #print myurl
       req=urllib2.Request(myurl)
       try:
           r1=urllib2.urlopen(req)
       except URLError, e:
           p_rep={}
           p_rep["STATE"]="DEAD"
           return json.dumps(p_rep,sort_keys=True)
       else:
           return r1.read()
   else:
       myurl = "http://"+host+ ":%d/%s/" % (port,prefix)
       #conn = httplib.HTTPConnection(myurl)
       #print myurl
       req=urllib2.Request(myurl)
       try:
           r1=urllib2.urlopen(req)
       except URLError, e:
           p_rep={}
           p_rep["STATE"]="DEAD"
           return json.dumps(p_rep,sort_keys=True)
       else:
           return r1.read()
    

#
# Check the configuration
#

class fdaqClient:
  """
  Handle all application definition and parameters
  """
  def __init__(self):
      self.p_conf=None
      self.daq_url=None
      self.daq_file=None
      self.parseConfig()
      self.daqhost=None
      self.daqport=None
      self.slowhost=None
      self.slowport=None
      self.daq_par={}
      
      for x,y in self.p_conf["HOSTS"].iteritems():
          for p in y:
              port=0
              
              for e in p["ENV"]:
                  if (e.split("=")[0]=="WEBPORT"):
                      port=int(e.split("=")[1])
              if (p["NAME"]=="FDAQ"):
                  self.daqhost=x
                  self.daqport=port
                  if ("PARAMETER" in p):
                      self.daq_par.update(p["PARAMETER"])
                      if ("s_ctrlreg" in self.daq_par):
                          self.daq_par["ctrlreg"]=int(self.daq_par["s_ctrlreg"],16)
              
              if (p["NAME"]=="WSLOW"):
                  self.slowhost=x
                  self.slowport=port
              if (p["NAME"]=="DBSERVER"):
                  if ("PARAMETER" in p):
                      self.daq_par["db"]=p["PARAMETER"]
              if (p["NAME"]=="WRITER"):
                  if ("PARAMETER" in p):
                      self.daq_par["builder"]=p["PARAMETER"]
              if (p["NAME"]=="ZUPSERVER"):
                  if ("PARAMETER" in p):
                      self.daq_par["zup"]=p["PARAMETER"]
              if (p["NAME"]=="CCCSERVER"):
                  if ("PARAMETER" in p):
                      self.daq_par["ccc"]=p["PARAMETER"]
              if (p["NAME"]=="MDCCSERVER"):
                  if ("PARAMETER" in p):
                      self.daq_par["mdcc"]=p["PARAMETER"]
                      
      #print self.daqhost,self.daqport,self.daq_par
      #print self.slowhost,self.slowport

      
  def parseConfig(self):
    dm=os.getenv("DAQURL","NONE")
    if (dm!="NONE"):
        self.daq_url=dm
        req=urllib2.Request(dm)
        try:
            r1=urllib2.urlopen(req)
        except URLError, e:
            p_rep={}
            re
        else:
            self.p_conf=json.loads(r1.read())
    dm=os.getenv("DAQFILE","NONE")
    if (dm!="NONE"):
        self.daq_file=dm
        with open(dm) as data_file:    
            self.p_conf = json.load(data_file)
 
  def jc_create(self):
    lcgi={}
    if (self.daq_url!=None):
        lcgi["url"]=self.daq_url
    else:
        if (self.daq_file!=None):
            lcgi["file"]=self.daq_file
    for x,y in self.p_conf["HOSTS"].iteritems():
        #print x,"  found"
        sr=executeFSM(x,9999,"LJC-%s" % x,"INITIALISE",lcgi)
        print sr
            
  def jc_start(self):
    lcgi={}
    for x,y in self.p_conf["HOSTS"].iteritems():
        #print x,"  found"
        sr=executeFSM(x,9999,"LJC-%s" % x,"START",lcgi)
        print sr

  def jc_kill(self):
    lcgi={}
    for x,y in self.p_conf["HOSTS"].iteritems():
        #print x," found"
        sr=executeFSM(x,9999,"LJC-%s" % x,"KILL",lcgi)
        print sr
            
  def jc_destroy(self):
    lcgi={}
    for x,y in self.p_conf["HOSTS"].iteritems():
        # print x," found"
        sr=executeFSM(x,9999,"LJC-%s" % x,"DESTROY",lcgi)
        print sr

  def jc_status(self):
    lcgi={}

    for x,y in self.p_conf["HOSTS"].iteritems():
        print "HOST ",x
        sr=executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi)
        sj=json.loads(sr)
        ssj=sj["answer"]["JOBS"]
        print "\033[1m %6s %15s %25s %20s \033[0m" % ('PID','NAME','HOST','STATUS')
        for x in ssj:
            print "%6d %15s %25s %20s" % (x['PID'],x['NAME'],x['HOST'],x['STATUS'])
        
            
  def jc_restart(self,host,jobname,jobpid):
    lcgi={}
    lcgi["processname"]=jobname
    lcgi["pid"]=jobpid
    sr=executeCMD(host,9999,"LJC-%s" % host,"RESTARTJOB",lcgi)
    print sr
    
  def daq_create(self):
      lcgi={}
      if (self.daq_url!=None):
          lcgi["url"]=self.daq_url
      else:
          if (self.daq_file!=None):
              lcgi["file"]=self.daq_file
      for x,y in self.p_conf["HOSTS"].iteritems():
          for p in y:
              if (p["NAME"] != "FDAQ"):
                  continue;
              print x,p["NAME"]," process found"
              port=0
              for e in p["ENV"]:
                  if (e.split("=")[0]=="WEBPORT"):
                      port=int(e.split("=")[1])
              if (port==0):
                  continue
              p_rep={}
              surl="http://%s:%d/" % (x,port)
              req=urllib2.Request(surl)
              try:
                  r1=urllib2.urlopen(req)
                  p_rep=json.loads(r1.read())
              except URLError, e:
                  print surl,e
                  p_rep={}
              print x,port,p["NAME"],p_rep
              if ("STATE" in p_rep):
                  if (p_rep["STATE"]=="VOID"):
                      sr=executeFSM(x,port,p_rep["PREFIX"],"CREATE",lcgi)
                      #print sr
  def daq_list(self):
      lcgi={}
      if (self.daq_url!=None):
          lcgi["url"]=self.daq_url
      else:
          if (self.daq_file!=None):
              lcgi["file"]=self.daq_file
      for x,y in self.p_conf["HOSTS"].iteritems():
          print "HOST ",x
          print "\033[1m %12s %12s %8s %8s %20s \033[0m" % ('NAME','INSTANCE','PORT','PID','STATE')

          for p in y:
              #print x,p["NAME"]," process found"
              port=0
              for e in p["ENV"]:
                  if (e.split("=")[0]=="WEBPORT"):
                      port=int(e.split("=")[1])
              if (port==0):
                  continue
              p_rep={}
              surl="http://%s:%d/" % (x,port)
              req=urllib2.Request(surl)
              try:
                  r1=urllib2.urlopen(req)
                  p_rep=json.loads(r1.read())
                  print "%12s %12s %8d %8d %20s" % (p["NAME"],p_rep["PREFIX"],port,p_rep["PID"],p_rep["STATE"])
              except URLError, e:
                  print surl,e
                  p_rep={}
              
              
  def daq_discover(self):
      lcgi={}
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","DISCOVER",lcgi)
      print sr
      
  def daq_setparameters(self):
      lcgi={}
      lcgi["PARAMETER"]=json.dumps(self.daq_par,sort_keys=True)
      
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","SETPARAM",lcgi)
      print sr
      
  def daq_getparameters(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","GETPARAM",lcgi)
      print sr
  def daq_downloaddb(self,state):
      if ("db" in self.daq_par):
        self.daq_par["db"]["dbstate"]=state
      lcgi["state"]=dbstate
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","DOWNLOADDB",lcgi) 
  def daq_forceState(self,name):
      lcgi={}
      lcgi["state"]=name
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","FORCESTATE",lcgi)
      print sr
      
  def daq_services(self):

      lcgi={}
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","PREPARE",lcgi)
      print sr
      
  def daq_lvon(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","LVON",lcgi)
      print sr
      
  def daq_lvoff(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","LVOFF",lcgi)
      print sr
      
  def daq_lvstatus(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","LVSTATUS",lcgi)
      print sr

  def daq_initialise(self):
      lcgi={}
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","INITIALISE",lcgi)
      print sr

  def daq_configure(self):
      lcgi=self.daq_par
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","CONFIGURE",lcgi)
      print sr

  def daq_start(self):
      lcgi={}
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","START",lcgi)
      print sr

  def daq_stop(self):
      lcgi={}
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","STOP",lcgi)
      print sr

  def daq_destroy(self):
      lcgi={}
      sr=executeFSM(self.daqhost,self.daqport,"FDAQ","DESTROY",lcgi)
      print sr

  def daq_status(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","DIFSTATUS",lcgi)
      return sr
      
  def daq_evbstatus(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","EVBSTATUS",lcgi)
      return sr    
      
  def daq_dbstatus(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","DBSTATUS",lcgi)
      return sr    
      
  def daq_state(self):
      p_rep={}
      req=urllib2.Request("http://%s:%d/" % (self.daqhost,self.daqport))
      try:
         r1=urllib2.urlopen(req)
         p_rep=json.loads(r1.read())
      except URLError, e:
         print "no connection to DAQ"
         exit(0)
      if ("STATE" in p_rep):
         print p_rep["STATE"]
     
  def daq_ctrlreg(self,ctrl):
      lcgi={}
      lcgi["value"]=ctrl
      self.daq_par["ctrlreg"]=ctrl
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","CTRLREG",lcgi)
      print sr    
      
  def daq_setgain(self,gain):
      lcgi={}
      lcgi["GAIN"]=gain
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","SETGAIN",lcgi)
      print sr    
      
  def daq_setthreshold(self,b0,b1,b2):
      lcgi={}
      lcgi["B0"]=b0
      lcgi["B1"]=b1
      lcgi["B2"]=b2
      
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","SETTHRESHOLD",lcgi)
      print sr    
      

  def trig_status(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","TRIGGERSTATUS",lcgi)
      print sr  

  def trig_reset(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","RESETCOUNTERS",lcgi)
      print sr  

  def trig_pause(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","PAUSE",lcgi)
      print sr  

  def trig_resume(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","RESUME",lcgi)
      print sr  

  def ecal_pause(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","ECALPAUSE",lcgi)
      print sr  

  def ecal_resume(self):
      lcgi={}
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","ECALRESUME",lcgi)
      print sr  

  def trig_spillon(self,clock):
      lcgi={}
      lcgi["clock"]=clock
      
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","SPILLON",lcgi)
      print sr    

  def trig_spilloff(self,clock):
      lcgi={}
      lcgi["clock"]=clock
      
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","SPILLOFF",lcgi)
      print sr    
      
  def trig_beam(self,clock):
      lcgi={}
      lcgi["clock"]=clock
      
      sr=executeCMD(self.daqhost,self.daqport,"FDAQ","BEAMON",lcgi)
      print sr    
                  
parser = argparse.ArgumentParser()

# configure all the actions
grp_action = parser.add_mutually_exclusive_group()
# JOB Control
grp_action.add_argument('--available',action='store_true',help='Check availability of daq,jobcontrol and slowcontrol')
grp_action.add_argument('--webstatus',action='store_true',help='Check availability of daq,jobcontrol,slowcontrol and Ecal web servers')
grp_action.add_argument('--jc-create',action='store_true',help='Create the DimJobControlInterface object to control processes')
grp_action.add_argument('--jc-kill',action='store_true',help='kill all controled processes')
grp_action.add_argument('--jc-destroy',action='store_true',help='de-initialise ljc controller')
grp_action.add_argument('--jc-start',action='store_true',help='start all controled processes described in $DAQCONFIG jsonfile variable')
grp_action.add_argument('--jc-restart',action='store_true',help='restart one job with --jobname=name --jobpid=pid --host=hostname')
grp_action.add_argument('--jc-status',action='store_true',help='show the status all controled processes')

#DAQ preparation
grp_action.add_argument('--daq-create',action='store_true',help='Create the Daq object to access DIf/CCC/EVB')
grp_action.add_argument('--daq-discover',action='store_true',help='trigger a scan of the DNS of the Daq')
grp_action.add_argument('--daq-setparameters',action='store_true',help='send the parameters described in $DAQCONFIG file to the DAQ')
grp_action.add_argument('--daq-getparameters',action='store_true',help='get the parameters described in $DAQCONFIG file to the DAQ')
grp_action.add_argument('--daq-forceState',action='store_true',help='force the sate name of the Daq with the --state option, ex --forceState --state=DISCOVERED')
grp_action.add_argument('--daq-services',action='store_true',help='Triggers teh download of the DB state, the initialisation of the Zup and of the CCC according to $DAQCONFIG values (compulsary before first initialise)')
# LV
grp_action.add_argument('--daq-lvon',action='store_true',help='put Zup LV ON')
grp_action.add_argument('--daq-lvoff',action='store_true',help='put Zup LV OFF')
grp_action.add_argument('--daq-lvstatus',action='store_true',help='Dump LV status')
# Running
grp_action.add_argument('--daq-initialise',action='store_true',help=' initialise the DAQ')
grp_action.add_argument('--daq-configure',action='store_true',help=' configure the DAQ')
grp_action.add_argument('--daq-status',action='store_true',help=' display DAQ status of all DIF')
grp_action.add_argument('--daq-state',action='store_true',help=' display DAQ state')
grp_action.add_argument('--daq-evbstatus',action='store_true',help=' display event builder status')
grp_action.add_argument('--daq-startrun',action='store_true',help=' start the run')
grp_action.add_argument('--daq-stoprun',action='store_true',help=' stop the run')
grp_action.add_argument('--daq-destroy',action='store_true',help='destroy the DIF readout, back to the PREPARED state')
grp_action.add_argument('--daq-downloaddb',action='store_true',help='download the dbsate specified in --dbstate=state')
grp_action.add_argument('--daq-dbstatus',action='store_true',help='get current run and state from db')
grp_action.add_argument('--daq-ctrlreg',action='store_true',help='set the ctrlregister specified with --ctrlreg=register')

# Calibration
grp_action.add_argument('--daq-setgain',action='store_true',help='change the gain and reconfigure chips with --gain=xxx')
grp_action.add_argument('--daq-setthreshold',action='store_true',help='change the threholds and reconfigure chips with --B0=xxx --B1=yyy --B2=zzz')




# TRIGGER
grp_action.add_argument('--trig-status',action='store_true',help=' display trigger counter status')
grp_action.add_argument('--trig-reset',action='store_true',help=' reset trigger counter')
grp_action.add_argument('--trig-pause',action='store_true',help=' trigger soft veto')
grp_action.add_argument('--trig-resume',action='store_true',help=' release trigger soft veto ')
grp_action.add_argument('--ecal-pause',action='store_true',help=' Ecal soft veto')
grp_action.add_argument('--ecal-resume',action='store_true',help=' release Ecal soft veto')
grp_action.add_argument('--trig-spillon',action='store_true',help=' set spill nclock on with --clock=nc (20ns)')
grp_action.add_argument('--trig-spilloff',action='store_true',help=' set spill nclock off with --clock=nc (20ns) ')
grp_action.add_argument('--trig-beam',action='store_true',help=' set beam length to nclock with --clock=nc (20ns) ')




# SLC
grp_action.add_argument('--slc-create',action='store_true',help='Create the DimSlowControl object to control WIENER crate and BMP sensor')
grp_action.add_argument('--slc-initialisesql',action='store_true',help='initiliase the mysql access specified with --account=login/pwd@host:base')
grp_action.add_argument('--slc-loadreferences',action='store_true',help='load in the wiener crate chambers references voltage download from DB')
grp_action.add_argument('--slc-hvstatus',action='store_true',help='display hvstatus of all channel of the wiener crate')
grp_action.add_argument('--slc-ptstatus',action='store_true',help='display the P and T from the BMP183 readout')
grp_action.add_argument('--slc-setperiod',action='store_true',help='set the readout period of Wiener and BMP with --period=second(s)')
grp_action.add_argument('--slc-setvoltage',action='store_true',help='set the voltage V of channel i to k with --first=i --last=k --voltage=V')
grp_action.add_argument('--slc-setcurrent',action='store_true',help='set the current limit I (microA) of channel i to k with --first=i --last=k --current=I')
grp_action.add_argument('--slc-hvon',action='store_true',help='set the voltage ON  of channel i to k with --first=i --last=k ')
grp_action.add_argument('--slc-hvoff',action='store_true',help='set the voltage OFF  of channel i to k with --first=i --last=k ')
grp_action.add_argument('--slc-store',action='store_true',help='start the data storage in the mysql DB at period p  with --period=p (s) ')
grp_action.add_argument('--slc-store-stop',action='store_true',help='stop the data storage in the mysql DB ')
grp_action.add_argument('--slc-check',action='store_true',help='start the voltage tuning wrt references at period p  with --period=p (s) ')
grp_action.add_argument('--slc-check-stop',action='store_true',help='stop the voltage tuning ')




# Arguments
# DAQ & trigger
parser.add_argument('--config', action='store', dest='config',default=None,help='json config file')
parser.add_argument('--socks', action='store', type=int,dest='sockport',default=None,help='use SOCKS port ')
parser.add_argument('--dbstate', action='store', default=None,dest='dbstate',help='set the dbstate')
parser.add_argument('--ctrlreg', action='store', default=None,dest='ctrlreg',help='set the Control register in hexa')
parser.add_argument('--version', action='version', version='%(prog)s 1.0')
parser.add_argument('--state', action='store', type=str,default=None,dest='fstate',help='set the Daq state')
parser.add_argument('--clock', action='store',type=int, default=None,dest='clock',help='set the number of 20 ns clock')
parser.add_argument('--directory', action='store', type=str,dest='directory',default=None,help='shm publisher directory')

parser.add_argument('--gain', action='store', type=int,default=None,dest='gain',help='set the gain for chips')
parser.add_argument('--B0', action='store', type=int,default=None,dest='B0',help='set the B0 for chips')
parser.add_argument('--B1', action='store', type=int,default=None,dest='B1',help='set the B1 for chips')
parser.add_argument('--B2', action='store', type=int,default=None,dest='B2',help='set the B2 for chips')



# Slow
parser.add_argument('--channel', action='store',type=int, default=None,dest='channel',help='set the hvchannel')
parser.add_argument('--first', action='store',type=int, default=None,dest='first',help='set the first hvchannel')
parser.add_argument('--last', action='store',type=int, default=None,dest='last',help='set the last hvchannel')
parser.add_argument('--voltage', action='store',type=float, default=None,dest='voltage',help='set the hv voltage')
parser.add_argument('--current', action='store',type=float, default=None,dest='current',help='set the hv current')
parser.add_argument('--account', action='store', default=None,dest='account',help='set the Slow Control mysql account')
parser.add_argument('--period', action='store',type=int, default=None,dest='period',help='set the temporistaion period (s)')

# Job
parser.add_argument('--lines', action='store',type=int, default=None,dest='lines',help='set the number of lines to be dump')
parser.add_argument('--host', action='store', dest='host',default=None,help='host for log')
parser.add_argument('--jobname', action='store', dest='jobname',default=None,help='job name')
parser.add_argument('--jobpid', action='store', type=int,dest='jobpid',default=None,help='job pid')



parser.add_argument('-v','--verbose',action='store_true',default=False,help='Raw Json output')

results = parser.parse_args()



 
    
#print results
#exit(0)
# Analyse results
# set the connection mode
if (results.sockport==None):
    sp=os.getenv("SOCKPORT","Not Found")
    if (sp!="Not Found"):
        results.sockport=int(sp)


if (results.sockport !=None):
    socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS5, "127.0.0.1", results.sockport)
    socket.socket = socks.socksocket
    #print "on utilise sock",results.sockport


    
# fill parameters
#p_conf=parseConfig()
#print p_conf

fdc=fdaqClient();





# analyse the command
lcgi={}
r_cmd=None
if (results.daq_create):
    r_cmd='createDaq'
    fdc.daq_create()
    exit(0)

elif (results.webstatus):
    p_res={}
    #if (hasattr(conf,'ecalhost')):
        #sre=executeCMD(conf.ecalhost,conf.ecalport,"ecalWeb",None,None)
        #sje=json.loads(sre)
        #p_res["ECAL"]=sje["STATE"]
    #else:
        #p_res["ECAL"]="DEAD"
    #srd=executeCMD(conf.daqhost,conf.daqport,"FDAQ",None,None)
    #srs=executeCMD(conf.slowhost,conf.slowport,"WSLOW",None,None)
    #srj=executeCMD(conf.jobhost,conf.jobport,"WJOB",None,None)
    #sjd=json.loads(srd)
    #sjs=json.loads(srs)
    #sjj=json.loads(srj)
    #p_res["DAQ"]=sjd["STATE"]
    #p_res["SLOW"]=sjs["STATE"]
    #p_res["JOB"]=sjj["STATE"]
    
    print json.dumps(p_res)
    exit(0)
elif(results.available):
    r_cmd='available'
    fdc.daq_list()
    #srd=executeCMD(conf.daqhost,conf.daqport,"WDAQ",None,None)
    #if (results.verbose):
        #print srd
    #else:
        #print ">>>>>>>>>>>>>>>> DAQ <<<<<<<<<<<<<<<<<<"
        #parseReturn("state",srd)
    #srs=executeCMD(conf.slowhost,conf.slowport,"WSLOW",None,None)
    #if (results.verbose):
        #print srs
    #else:
        #print ">>>>>>>>>>>>>>>> SLOWCONTROL <<<<<<<<<<<<<<<<<<"
        #parseReturn("state",srs)
    #srj=executeCMD(conf.jobhost,conf.jobport,"WJOB",None,None)
    #if (results.verbose):
        #print srj
    #else:
        #print ">>>>>>>>>>>>>>>> JOB CONTROL <<<<<<<<<<<<<<<<<<"
        #parseReturn("state",srj)
    exit(0)
elif(results.jc_create):
    r_cmd='createJobControl'
    fdc.jc_create();
    exit(0)
elif(results.jc_kill):
    r_cmd='jobKillAll'
    fdc.jc_kill()
    exit(0)
elif(results.jc_start):
   
    r_cmd='jobStartAll'
    fdc.jc_start()
    exit(0)
elif(results.jc_destroy):
   
    r_cmd='jobDestroy'
    fdc.jc_destroy()
    exit(0)
elif(results.jc_restart):
    lcgi.clear();
    if (results.host==None):
        print "set the host "
        exit(0)
    if (results.jobname==None):
        print "set the jobname "
        exit(0)
    if (results.jobpid==None):
        print "set the jobpid "
        exit(0)
    fdc.jc_restart(results.host,results.jobname,results.jobpid)
    r_cmd='jobReStart'
    exit(0)
elif(results.jc_status):
    sr=fdc.jc_status()
    exit(0)
elif(results.daq_state):
    r_cmd='state'
    fdc.daq_state();
    #print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    exit(0)
elif(results.daq_discover):
    r_cmd='Discover'
    fdc.daq_discover()
    fdc.daq_setparameters()
    exit(0)
elif(results.daq_setparameters):
    r_cmd='setParameters'
    fdc.daq_setparameters()
    exit(0)
elif(results.daq_getparameters):
    r_cmd='getParameters'
    fdc.daq_getparameters()
    exit(0)


elif(results.daq_forceState):
    r_cmd='forceState'
    if (results.fstate!=None):
        fdc.daq_forceState(results.fstate)
    else:
        print 'Please specify the state --state=STATE'
    exit(0)
elif(results.daq_services):
    r_cmd='prepareServices'
    fdc.daq_services()
    exit(0)
elif(results.daq_lvon):
    r_cmd='LVON'
    fdc.daq_lvon()
    exit(0)

elif(results.daq_lvoff):
    r_cmd='LVOFF'
    fdc.daq_lvoff()
    exit(0)
elif(results.daq_lvstatus):
    r_cmd='LVStatus'
    fdc.daq_lvstatus()
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    exit(0)
elif(results.daq_initialise):
    r_cmd='initialise'
    fdc.daq_initialise()
    exit(0)

elif(results.daq_configure):
    r_cmd='configure'
    #if (hasattr(conf,'ecalhost')):
        #if (hasattr(conf,'ecaldetid') and hasattr(conf,'ecalsourceid') ):
            #for x in conf.ecalsourceid:
                #lcgi.clear()
                #lcgi['detid']=conf.ecaldetid
                #lcgi['sourceid']=x
                #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","REGISTERDS",lcgi)
                #print sr
    fdc.daq_configure()
    exit(0)

elif(results.daq_status):
    r_cmd='status'
    sr=fdc.daq_status()
    if (results.verbose):
        print sr
    else:
        parseReturn(r_cmd,sr)
    exit(0)
elif(results.daq_evbstatus):
    r_cmd='shmStatus'
    sr=fdc.daq_evbstatus()
    if (results.verbose):
        print sr
    else:
        parseReturn(r_cmd,sr)
    exit(0)
elif(results.daq_setgain):
    r_cmd='SETGAIN'
    if (results.gain==None):
        print 'Please specify the gain --gain=value'
        exit(0)
    fdc.daq_setgain(results.gain)
    exit(0)

elif(results.daq_setthreshold):
    r_cmd='SETTHRESHOLD'
    if (results.B0==None):
        print 'Please specify the B0 --B0=value'
        exit(0)
    if (results.B1==None):
        print 'Please specify the B1 --B1=value'
        exit(0)
    if (results.B2==None):
        print 'Please specify the B2 --B2=value'
        exit(0)
    fdc.setthreshold(results.B0,results.B1,results.B2)
    exit(0)

elif(results.daq_startrun):
    r_cmd='start'
    fdc.daq_start()
    exit(0)

elif(results.daq_stoprun):
    r_cmd='stop'
    fdc.daq_stop()
    exit(0)
elif(results.daq_destroy):
    r_cmd='destroy'
    fdc.daq_destroy()
    exit(0)
elif(results.daq_dbstatus):
    r_cmd='dbStatus'
    sr=fdc.daq_dbstatus()
    if (results.verbose):
        print sr
    else:
        parseReturn(r_cmd,sr)
    exit(0)

elif(results.daq_downloaddb):
    r_cmd='downloadDB'
    if (results.dbstate!=None):
        lcgi['state']=results.dbstate
    else:
        print 'Please specify the state --dbstate=STATE'
        exit(0)
    fdc.daq_downloaddb(results.dbstate)
    exit(0)
elif(results.daq_ctrlreg):
    r_cmd='setControlRegister'
    lct=0
    if (results.ctrlreg!=None):
        lct=int(results.ctrlreg,16)
    else:
        print 'Please specify the value --ctrlreg=0X######'
        exit(0)
    fdc.daq_ctrlreg(lct)
    exit(0)

elif(results.trig_status):
    r_cmd='triggerStatus'
    fdc.trig_status()
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    exit(0)

elif(results.trig_beam):
    r_cmd='triggerBeam'
    if (results.clock!=None):
        fdc.trig_beam(results.clock)
    else:
        print 'Please specify the number of clock --clock=xx'
    exit(0)


elif(results.trig_spillon):
    r_cmd='triggerSpillOn'
    if (results.clock!=None):
        fdc.trig_spillon(results.clock)
    else:
        print 'Please specify the number of clock --clock=xx'
    exit(0)
 

elif(results.trig_spilloff):
    r_cmd='triggerSpillOff'
    if (results.clock!=None):
        fdc.trig_spilloff(results.clock)
    else:
        print 'Please specify the number of clock --clock=xx'
    exit(0) 

elif(results.ecal_pause):
    r_cmd='pauseEcal'
    fdc.ecal_pause()
    exit(0)

elif(results.ecal_resume):
    r_cmd='resumeEcal'
    fdc.ecal_resume()
    exit(0)

elif(results.trig_reset):
    r_cmd='resetTrigger'
    fdc.trig_reset()
    exit(0)
elif(results.trig_pause):
    r_cmd='pause'
    ffdc.trig_pause()
    exit(0)
elif(results.trig_resume):
    r_cmd='resume'
    fdc.trig_resume()
    exit(0)
elif(results.slc_create):
    r_cmd='createSlowControl'
    #lcgi['jsonfile']=conf.jsonfile
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    sr=executeFSM(fdc.slowhost,fdc.slowport,"WSLOW","DISCOVER",lcgi)
    print sr
    exit(0)

elif(results.slc_initialisesql):
    r_cmd='initialiseDB'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    if (results.account!=None):
        lcgi['account']=results.account
    else:
        print 'Please specify the MYSQL account --account=log/pwd@host:base'
        exit(0)
    sr=executeFSM(fdc.slowhost,fdc.slowport,"WSLOW","INITIALISE",lcgi)
    print sr
    exit(0)
elif(results.slc_loadreferences):
    r_cmd='loadReferences'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","LOADREFERENCES",lcgi)
    print sr
    exit(0)
elif(results.slc_hvstatus):
    r_cmd='hvStatus'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    lcgi['channel']=99;
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","HVREADCHANNEL",lcgi)
    if (results.verbose):
        print sr
    else:
        parseReturn(r_cmd,sr)
    exit(0)

elif(results.slc_ptstatus):
    r_cmd='PT'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    #lcgi['channel']=99;
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","PTREAD",lcgi)
    print sr
    exit(0)
elif(results.slc_setperiod):
    r_cmd='setReadoutPeriod'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    if (results.period!=None):
        lcgi['period']=results.period
    else:
        print 'Please specify the period --period=second(s)'
        exit(0)
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","SETPERIOD",lcgi)
    print sr
    exit(0)
elif(results.slc_setvoltage):
    r_cmd='setVoltage'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    if (results.first!=None):
        lcgi['first']=results.first
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    if (results.last!=None):
        lcgi['last']=results.last
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    if (results.voltage!=None):
        lcgi['voltage']=results.voltage
    else:
        print 'Please specify the voltage --voltage=V'
        exit(0)
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","SETVOLTAGE",lcgi)
    print sr
    exit(0)

elif(results.slc_setcurrent):
    r_cmd='setCurrentLimit'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    if (results.first!=None):
        lcgi['first']=results.first
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    if (results.last!=None):
        lcgi['last']=results.last
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    if (results.current!=None):
        lcgi['current']=results.current
    else:
        print 'Please specify the current limit --current=V'
        exit(0)
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","SETCURRENTLIMIT",lcgi)
    print sr
    exit(0)
    
elif(results.slc_hvon):
    r_cmd='HVON'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    if (results.first!=None):
        lcgi['first']=results.first
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    if (results.last!=None):
        lcgi['last']=results.last
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","HVON",lcgi)
    print sr
    exit(0)

elif(results.slc_hvoff):
    r_cmd='HVOFF'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    lcgi.clear()
    if (results.first!=None):
        lcgi['first']=results.first
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    if (results.last!=None):
        lcgi['last']=results.last
    else:
        print 'Please specify the channels --first=# --last=#'
        exit(0)
    sr=executeCMD(fdc.slowhost,fdc.slowport,"WSLOW","HVOFF",lcgi)
    print sr
    exit(0)
elif(results.slc_store):
    r_cmd='startStorage'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    if (results.period!=None):
        lcgi['period']=results.period
    else:
        print 'Please specify the period --period=second(s)'
        exit(0)
    sr=executeFSM(fdc.slowhost,fdc.slowport,"WSLOW","STARTMONITOR",lcgi)
    print sr
    exit(0)
elif(results.slc_check):
    r_cmd='startCheck'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    if (results.period!=None):
        lcgi['period']=results.period
    else:
        print 'Please specify the period --period=second(s)'
        exit(0)
    sr=executeFSM(fdc.slowhost,fdc.slowport,"WSLOW","STARTCHECK",lcgi)
    print sr
    exit(0)

elif(results.slc_store_stop):
    r_cmd='stopStorage'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    sr=executeFSM(fdc.slowhost,fdc.slowport,"WSLOW","STOPMONITOR",lcgi)
    print sr
    exit(0)

elif(results.slc_check_stop):
    r_cmd='stopCheck'
    if (fdc.slowhost==None or fdc.slowport==None):
      print "No WSLOW application exiting"
      exit(0)
    sr=executeFSM(fdc.slowhost,fdc.slowport,"WSLOW","STOPCHECK",lcgi)
    print sr
    exit(0)

#print r_cmd
#print lcgi


def sendcommand2(command,host=fdc.daqhost,port=fdc.daqport,lq=None):
   global results 
   if (lq!=None):
       if (len(lq)!=0):
           myurl = "http://"+host+ ":%d" % (port)
           #conn = httplib.HTTPConnection(myurl)
           #if (name!=None):
           #    lq['name']=name
           #if (value!=None):
           #    lq['value']=value
           lqs=urllib.urlencode(lq)
           saction = '/%s?%s' % (command,lqs)
           myurl=myurl+saction
           #print myurl
           req=urllib2.Request(myurl)
           r1=urllib2.urlopen(req)

           return r1.read()
   else:
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       saction = '/%s' % command
       myurl=myurl+saction
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       if (command=="status" and not results.verbose):
           s=r1.read()
           sj=json.loads(s)
           ssj=json.loads(sj["statusResponse"]["statusResult"][0])
#           for x in ssj:
#             for d in x["difs"]:
#                print '#%4d %5x %6d %12d %12d %s %s ' % (d["id"],d["slc"],d["gtc"],d["bcid"],d["bytes"],d["state"],x["name"])
           print "\033[1m %4s %5s %6s %12s %12s %15s  %s \033[0m" % ('DIF','SLC','EVENT','BCID','BYTES','SERVER','STATUS')

           for d in ssj:
               #print d
               #for d in x["difs"]:
               print '#%4d %5x %6d %12d %12d %15s %s ' % (d["id"],d["slc"],d["gtc"],d["bcid"],d["bytes"],d["host"],d["state"])
       elif (command=="jobStatus" and not results.verbose ):
           s=r1.read()
           sj=json.loads(s)
           ssj=json.loads(sj["jobStatusResponse"]["jobStatusResult"][0])
           print "\033[1m %6s %15s %25s %20s \033[0m" % ('PID','NAME','HOST','STATUS')
           for x in ssj:
               if (x['DAQ']=='Y'):
                   print "%6d %15s %25s %20s" % (x['PID'],x['NAME'],x['HOST'],x['STATUS'])
       elif (command=="hvStatus" and not results.verbose):
           s=r1.read()
           sj=json.loads(s)
           ssj=json.loads(sj["hvStatusResponse"]["hvStatusResult"][0])
           print "\033[1m %5s %10s %10s %10s %10s \033[0m" % ('Chan','VSET','ISET','VOUT','IOUT')
           for x in ssj:
               print "#%.4d %10.2f %10.2f %10.2f %10.2f" % (x['channel'],x['vset'],x['iset'],x['vout'],x['iout'])

       elif (command=="LVStatus" and not results.verbose ):
           s=r1.read()
           sj=json.loads(s)
           ssj=json.loads(sj["LVStatusResponse"]["LVStatusResult"][0])
           print "\033[1m %10s %10s %10s \033[0m" % ('VSET','VOUT','IOUT')
           print " %10.2f %10.2f %10.2f" % (ssj['vset'],ssj['vout'],ssj['iout'])
           #for x in ssj:
           #    print "#%.4d %10.2f %10.2f %10.2f %10.2f" % (x['channel'],x['vset'],x['iset'],x['vout'],x['iout'])
       elif (command=="shmStatus" and not results.verbose):
           s=r1.read()
           sj=json.loads(s)
           ssj=json.loads(sj["shmStatusResponse"]["shmStatusResult"][0])
           print "\033[1m %10s %10s \033[0m" % ('Run','Event')
           print " %10d %10d " % (ssj['run'],ssj['event'])
       elif (command=="triggerStatus" and not results.verbose):
           s=r1.read()
           sj=json.loads(s)
           ssj=json.loads(sj["triggerStatusResponse"]["triggerStatusResult"][0])
           print "\033[1m %10s %10s %10s %10s %12s %12s %10s %10s %10s \033[0m" % ('Spill','Busy1','Busy2','Busy3','SpillOn','SpillOff','Beam','Mask','EcalMask')
           print " %10d %10d %10d %10d  %12d %12d %12d %10d %10d " % (ssj['spill'],ssj['busy1'],ssj['busy2'],ssj['busy3'],ssj['spillon'],ssj['spilloff'],ssj['beam'],ssj['mask'],ssj['ecalmask'])

       else:
          print r1.read()
          return r1.read()

       
       #print r1.status, r1.reason

if (r_cmd==None):
    print "No command given"
    exit(0)
if (len(lcgi)!=0):
    sendcommand2(r_cmd,lq=lcgi)
else:
    sendcommand2(r_cmd)
exit(0)