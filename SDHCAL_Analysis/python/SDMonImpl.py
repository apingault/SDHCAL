#!/usr/bin/python

from PyQt4 import QtGui, QtCore
import sys
import mc
#import postelog
import SDMon
from ROOT import *
class ImageViewer(QtGui.QMainWindow, SDMon.Ui_MainWindow): 
    def __init__(self, parent=None):
        super(ImageViewer, self).__init__(parent)
        self.setupUi(self)
        self.daq_=None
        self.isLVOn_=False
        self.canvas =None
        self.connectActions()
    
    def LVOn(self):
        if self.daq_!=None:
            self.daq_.LVOn()
            self.PBLVOff.setEnabled(True)
            self.PBLVOn.setEnabled(False)
            self.isLVOn_=True
    def LVOff(self):
        if self.daq_!=None:
            self.daq_.LVOff()
            self.PBLVOff.setEnabled(False)
            self.PBLVOn.setEnabled(True)
            self.isLVOn_=False
    def SetVoltage(self):
        if self.daq_!=None:
            self.daq_.HVSetVoltage(self.SBVoltage.value(),self.SBFirstHvChannel.value(),self.SBLastHvChannel.value())

    def SwitchHVOn(self):
        if self.daq_!=None:
            self.daq_.HVSwitchOn(self.SBFirstHvChannel.value(),self.SBLastHvChannel.value())
    def SwitchHVOff(self):
        if self.daq_!=None:
            self.daq_.HVSwitchOff(self.SBFirstHvChannel.value(),self.SBLastHvChannel.value())
    def HVDump(self):
        if self.daq_!=None:
            iv=self.daq_.HVGet(self.SBFirstHvChannel.value(),self.SBLastHvChannel.value())
            self.vm_=iv[0]
            self.im_=iv[1]
            print self.vm_
            print self.im_
            self.HSChannel.setEnabled(True)
            self.HSChannel.setMinimum(self.SBFirstHvChannel.value())
            self.HSChannel.setMaximum(self.SBLastHvChannel.value())
    def HVDisplay(self):
        self.LCDChannel.display(self.HSChannel.value())
        self.LCDVoltage.display(self.vm_[self.HSChannel.value()-self.HSChannel.minimum()])
        self.LCDCurrent.display(self.im_[self.HSChannel.value()-self.HSChannel.minimum()])
    
    def Initialise(self):
        if self.daq_!=None and self.isLVOn_:
            self.daq_.Initialise()
            self.PBInitialise.setEnabled(False)
            self.PBConfigure.setEnabled(True)
            self.PBDestroy.setEnabled(True)
            self.PBUpdate.setEnabled(True)
        elif not self.isLVOn_:
            QtGui.QMessageBox.about(self,"LV is OFF","Please put LV On")
            return
        
    
    def Configure(self):
        if self.daq_!=None:
            self.daq_.Configure()
            self.PBConfigure.setEnabled(True)
            self.PBStart.setEnabled(True)
            self.PBDestroy.setEnabled(True)

    def Start(self):
        if self.daq_!=None:
            self.daq_.Start()
            postelog.post(self.daq_.daq_.getCurrentRun(),self.daq_.daq_.getCurrentState())
            self.LCDRun.display(self.daq_.daq_.getCurrentRun())
            self.PBConfigure.setEnabled(False)
            self.PBStart.setEnabled(False)
            self.PBStop.setEnabled(True)
            self.PBDestroy.setEnabled(True)

    def Stop(self):
        if self.daq_!=None:
            self.daq_.Stop()
            self.PBStop.setEnabled(False)
            self.PBStart.setEnabled(True)
            self.PBConfigure.setEnabled(True)
            self.PBDestroy.setEnabled(True)
            
    def Destroy(self):
        if self.daq_!=None:
            self.daq_.Destroy()
            self.PBDestroy.setEnabled(False)
            self.PBConfigure.setEnabled(False)
            self.PBStart.setEnabled(False)
            self.PBStop.setEnabled(False)
            self.PBUpdate.setEnabled(False)
            self.PBInitialise.setEnabled(True)
    def Update(self):
       if self.daq_!=None:
            self.LCDRun.display(self.daq_.daq_.getCurrentRun())
            self.LCDEvent.display(self.daq_.daq_.getCurrentEvent())
            self.daq_.Print()
            self.daq_.daq_.getDifInfo()
            for i in range(0,self.daq_.daq_.seenNumberOfDif()):
                it_id = QtGui.QTableWidgetItem('%d' % self.daq_.daq_.seenId(i))
                self.TWDIF.setItem(i+1, 0,it_id)
                it_slc = QtGui.QTableWidgetItem('%x' % self.daq_.daq_.seenSlc(i))
                self.TWDIF.setItem(i+1, 1,it_slc)
                it_gtc = QtGui.QTableWidgetItem('%d' % self.daq_.daq_.seenGtc(i))
                self.TWDIF.setItem(i+1, 2,it_gtc)
                it_bcid = QtGui.QTableWidgetItem('%ld' % self.daq_.daq_.seenBcid(i))
                self.TWDIF.setItem(i+1, 3,it_bcid)
                it_bytes = QtGui.QTableWidgetItem('%ld' % self.daq_.daq_.seenBytes(i))
                self.TWDIF.setItem(i+1, 4,it_bytes)

                it_host = QtGui.QTableWidgetItem(self.daq_.daq_.seenHost(i))
                self.TWDIF.setItem(i+1, 5,it_host)

                it_state = QtGui.QTableWidgetItem(self.daq_.daq_.seenState(i))
               
                s=self.daq_.daq_.seenState(i)
                if (s.upper().find('FAIL')>=0):
                    it_state.setBackground(QtGui.QColor('red'))
                    QtGui.QMessageBox.about(self,"One DIF  failed" ,"Debug la %d Mon gros !" % self.daq_.daq_.seenId(i))

                self.TWDIF.setItem(i+1, 6,it_state)
            
    def accept(self):
        print "titi"
    def reject(self):
        print "tita"
    def main(self):
        self.show()
    def Quit(self):
        exit(0)
    def startMonitoring(self):
        host=str(self.leHost.text().toAscii())
        shmdir=str(self.leShmDirectory.text().toAscii())

        mc.startMonitoring(host,self.sbPort.value(),shmdir,self.sbNbDIF.value(),1001)
    def stopMonitoring(self):
        host=str(self.leHost.text().toAscii())
        mc.stopMonitoring(host,self.sbPort.value())
    def startReadFile(self):
        host=str(self.leHost.text().toAscii())
        filename=str(self.leFileName.text().toAscii())

        mc.startFile(host,self.sbPort.value(),filename)
    def Refresh(self):
        host=str(self.leHost.text().toAscii())
        self.histoList=mc.getHistoList(host,self.sbPort.value())
        self.fillTvhistos()
    def fillTvhistos(self):
        hl=self.histoList
        self.tvHistos.clear()
        # Found largest level
        maxlevel=0
        for hn in sorted(hl):
            fdirs= hn.split("/")
            if (len(fdirs)>(maxlevel+1)): 
                maxlevel=len(fdirs)-1
        print maxlevel
        
        # Loop on level
        topLevelItem=None
        for hn in sorted(hl):
            #print hn,"----------------------------------------------"
            fdirs= hn.split("/")

            
            splitFileName = QtCore.QString(hn).split("/");
            #print len(splitFileName)

            # add root folder as top level item if treeWidget doesn't already have it
            #print "TEST ",self.tvHistos.findItems(splitFileName[0], QtCore.Qt.MatchFixedString),len(self.tvHistos.findItems(splitFileName[0], QtCore.Qt.MatchFixedString)),splitFileName[0]
            if (len(self.tvHistos.findItems(splitFileName[0], QtCore.Qt.MatchFixedString))==0):
            
                topLevelItem = QtGui.QTreeWidgetItem();
                topLevelItem.setText(0, splitFileName[0]);
                self.tvHistos.addTopLevelItem(topLevelItem);
            

            parentItem = topLevelItem;

            # iterate through non-root directories (file name comes after)
            for i in range(1,len(splitFileName)):
            
                # iterate through children of parentItem to see if this directory exists
                thisDirectoryExists = False;
                for j in range(0,parentItem.childCount()):
                    if (splitFileName[i] == parentItem.child(j).text(0)):
                        thisDirectoryExists = True;
                        parentItem = parentItem.child(j);
                        break;
                    
                

                if (not thisDirectoryExists):
                    parentItem = QtGui.QTreeWidgetItem(parentItem);
                    parentItem.setText(0, splitFileName[i]);
                
            

            #childItem = QtGui.QTreeWidgetItem(parentItem);
            #childItem.setText(0, splitFileName.last());
        

        #setCentralWidget(self.tvHistos);










    def GetEff(self):
        host=str(self.leHost.text().toAscii())
        l=mc.GetEff(host,self.sbPort.value(),self.sbPlane.value())
        print l
        if (self.canvas==None):
            self.canvas=TCanvas("HistoGrams","Histograms")
        self.heff=TH2F(l[2])
        self.canvas.cd()
        
        self.heff.Draw("COLZ")
        self.canvas.Update()
    def histoClick(self):
        host=str(self.leHost.text().toAscii())
        print "Clicked"
        sel=self.tvHistos.currentItem()
        ldir=[]
        c=sel
        subd=False
        while c!=None:
            ldir.append(c.text(0).toAscii())
            c=c.parent()
        if (len(ldir)==1):
            hn=ldir[0]
        else:
            subd=True
            hn=""
            for i in reversed(ldir):
                hn=hn+i+"/"
        if (subd):
            hn=hn[0:len(hn)-1]
        print hn
        h=mc.getHisto(host,self.sbPort.value(),hn)
        if (self.canvas==None):
            self.canvas=TCanvas("HistoGrams","Histograms")
        
        if (h==None): 
            return
        self.canvas.cd()
        if (h.IsA().GetName() == 'TH2F'):
            if (self.cb2DOptions.currentText().toAscii() !='NONE'):
                dopt=self.cb2DOptions.currentText().toAscii()
                print str(dopt)
                h.Draw(str(dopt))
            else:
                h.Draw()
        else:
            h.Draw()
        self.canvas.Update()
    def connectActions(self):
        print "On connecte",self.leHost.text().toAscii()
        #QtCore.QObject.connect( self.PBCreateDaq, QtCore.SIGNAL('clicked()'), self.CreateDaq)
        #self.PBCreateDaq.setEnabled(False)
        self.pbQuit.clicked.connect(self.Quit)
        self.pbStartMonitoring.clicked.connect(self.startMonitoring)
        self.pbStopMonitoring.clicked.connect(self.stopMonitoring)
        self.pbStartReadFile.clicked.connect(self.startReadFile)
        self.pbRefresh.clicked.connect(self.Refresh)
        self.pbGetEff.clicked.connect(self.GetEff)
        self.tvHistos.doubleClicked.connect(self.histoClick)
        #self.PBStopHost.clicked.connect(self.StopHost)  
        #self.PBStartRPI.clicked.connect(self.StartRPI)
        #self.PBStopRPI.clicked.connect(self.StopRPI)
        #self.PBSynchronizeRPI.clicked.connect(self.SynchronizeRPI)
        #self.PBDiscover.clicked.connect(self.Discover)
        #self.PBDiscoverDNS.clicked.connect(self.DiscoverDNS)
        #self.PBDownloadDB.clicked.connect(self.DownloadDB)
        #self.PBInitialiseWriter.clicked.connect(self.InitialiseWriter)
        #self.PBLVOn.clicked.connect(self.LVOn)
        #self.PBLVOff.clicked.connect(self.LVOff)
        #self.PBHVOn.clicked.connect(self.SwitchHVOn)
        #self.PBHVOff.clicked.connect(self.SwitchHVOff)
        #self.PBSetVoltage.clicked.connect(self.SetVoltage)
        #self.PBHVDump.clicked.connect(self.HVDump)
        #self.HSChannel.valueChanged.connect(self.HVDisplay)
        #self.PBInitialise.clicked.connect(self.Initialise)
        #self.PBConfigure.clicked.connect(self.Configure)
        #self.PBStart.clicked.connect(self.Start)
        #self.PBStop.clicked.connect(self.Stop)
        #self.PBDestroy.clicked.connect(self.Destroy)
        #self.PBUpdate.clicked.connect(self.Update)
    def main(self):
        self.show()
        
if __name__=='__main__':
    app = QtGui.QApplication(sys.argv)
    imageViewer = ImageViewer()
    imageViewer.main()
    app.exec_()