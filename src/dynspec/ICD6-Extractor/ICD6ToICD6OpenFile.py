#!/usr/bin/python
# -*- coding: utf-8 -*-


#############################
# IMPORT COMMANDS
#############################

import sys, os, glob, commands
import time
import tkFileDialog

from pylab import *
import matplotlib

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure

from numpy import *
import numpy as np
#import h5py

from Tkinter import *
from dal import *
from tkFileDialog import *

#############################
# OPEN READ AND WRITE METADATA
#############################


class ICD6toICD6penFile():

    def __init__(self, parent, h5FilePath):
      
        # Attributes of ICD6OpenFile Class
        self.grandparent 	= parent
        self.PATH0 		= h5FilePath
        

	self.tStart			= ''
	self.tStop			= ''
	self.tIncrement			= ''	
	self.fMin			= ''
	self.fMax			= ''
	self.nofChannelsPerSubband	= ''        
	self.nofSubbands		= '' 	
	self.nofTimeSample		= ''	
	self.AXIS_VALUE_WORLD		= ''
	
	self.SAPRawFileSize		= ''	
	
        self.outputDir			= ''
	self.RAM 			= ''
	
	self.NEW_NOF_SUBBANDS		= ''
	self.NEW_CHANNELPERSUBBANDS	= ''
	self.NEW_FMIN			= ''
	self.NEW_FMAX			= ''
	self.NEWTIMEREBIN		= ''
	self.NEWTIMESTART		= ''
	self.NEWTIMESTOP		= ''        
        
        self.GOindicator 		= 'yes'

	self.RAMOptimumResumeAll	= ''  
	self.TimeEstimatedResumeAll	= ''        
        self.SizeEstimated		= ''
	
	self.StartTimeProcess		= ''
	self.observationID		= ''
	
	self.NEW_CHANNELPERSUBBANDS2COMPUTE	= ''
	self.NEW_FMIN2COMPUTE			= ''
	self.NEW_FMAX2COMPUTE			= ''
	self.NEWTIMEREBIN2COMPUTE		= ''
	self.NEWTIMESTART2COMPUTE		= ''
	self.NEWTIMESTOP2COMPUTE		= ''	
	
	
	
	
        ### Define Left and Right Parents for organising windows 		
	###############################################
        self.leftparent = Frame(self.grandparent, width=672)
        self.leftparent.pack(side=LEFT, fill=Y)
        

        
        ### Frames for Left Parent
        
	# FRAME N°0 (Frame for loading Dynspec => Listbox)
        self.lframe0 = LabelFrame(self.leftparent, text="Dynamic Spectra",width=672, height=15)
        self.lframe0.pack(anchor=NW)
	### Listbox for dynspec
	yscrollbar0 = Scrollbar(self.lframe0,orient=VERTICAL )
	yscrollbar0.pack(side=RIGHT, fill= Y)		
	self.dynspecTable = Listbox(self.lframe0,width=75, height=15,yscrollcommand=yscrollbar0.set) 
	### Load data file
	h5File = File(h5FilePath, File.READ)
	nofDynspec = AttributeInt(h5File, "NOF_DYNSPEC")
	k=range(nofDynspec.value)
	for i in k:
	    if i < 10:
		self.dynspecTable.insert(i,"DYNSPEC_00%s"%(i))
	    if i > 9 and k<100:
		self.dynspecTable.insert(i,"DYNSPEC_0%s"%(i))
	    if i > 99 and k<1000:
		self.dynspecTable.insert(i,"DYNSPEC_%s"%(i))
	### Listbox generation
	self.dynspecTable.pack()
	yscrollbar0.config(command=self.dynspecTable.yview)		
	
	
	
	### Frame N°1 (Frame for loading Metadata => TextBox)
	self.lframe1 = LabelFrame(self.leftparent, text="Metadata",width=672, height=15)
        self.lframe1.pack(anchor=W)
	### Message box for showing selected metadata
	xscrollbar1 = Scrollbar(self.lframe1, orient=HORIZONTAL)
	xscrollbar1.pack(side=BOTTOM, fill=X)
	yscrollbar1 = Scrollbar(self.lframe1, orient=VERTICAL)
	yscrollbar1.pack(side=RIGHT, fill=Y)
	self.dynspecMetaData = Text(self.lframe1,width=83, height=15, wrap=NONE,xscrollcommand=xscrollbar1.set,yscrollcommand=yscrollbar1.set)
	self.dynspecMetaData.pack()
	xscrollbar1.config(command=self.dynspecMetaData.xview)
	yscrollbar1.config(command=self.dynspecMetaData.yview)	
	
	
	
	###FRAME N°3 (Action button)
	self.lframe3 = LabelFrame(self.leftparent, text="Actions", width=672, height=2)
        self.lframe3.pack(anchor=SW)      
	### Plot Button for plotting selected dynspec
	self.dplotBbutton = Button(self.lframe3, text="Extract & Rebin DynSpec",width=73, height=2, padx=10, pady=3)
	self.dplotBbutton.config(state=DISABLED)
	self.dplotBbutton.pack()
	
	
	### Erase button for erase all and restart with another file
	self.eraseBbutton = Button(self.lframe3, text="Clear",width=73, height=2, padx=10, pady=3)
	self.eraseBbutton.pack()	
	
	
	
        
        
     
        
        
	### Actions for All Buttons and Listbox etc ... 
	##############################################
        self.dynspecTable.bind("<Double-Button-1>", lambda event, dynspecTable = self.dynspecTable, dynspecMetaData=self.dynspecMetaData,  : self.selectionMetaData(self,dynspecTable,dynspecMetaData))
	
	
        ### Active buttons for Frame N°3

        self.dplotBbutton.bind("<Double-Button-1>", lambda event : self.extractAllSAPRebin(self))
        self.eraseBbutton.bind("<Double-Button-1>", lambda event, lframe0 = self.lframe0, lframe1 = self.lframe1, lframe3 = self.lframe3, leftparent = self.leftparent  : self.selectionErase(self,lframe0,lframe1,lframe3,leftparent))
                
        
        
    ###############################################"    
    # Erase Function     
    ###############################################"    
    def selectionErase(self,event,lframe0,lframe1,lframe3,leftparent):	
	lframe0.destroy()
	lframe1.destroy()
	lframe3.destroy()
	leftparent.destroy()
	
	
    def eraseObject(self):	
	self.lframe0.destroy()
	self.lframe1.destroy()	
	self.lframe3.destroy()
	self.leftparent.destroy()
	
	
	
    ###############################################"    
    # Selection Dynspec => plot metadata  Function     
    ###############################################" 	
    def selectionMetaData(self,event,dynspecTable,dynspecMetaData):
	
	dynspecMetaData.config(state=NORMAL)
	dynspecMetaData.delete(1.0, END)

	
	nofSelectedDynspec = int(dynspecTable.curselection()[0])
	
	if nofSelectedDynspec < 10:
	    pathDynspec = "DYNSPEC_00%s"%(nofSelectedDynspec)
        if nofSelectedDynspec > 9 and nofSelectedDynspec<100:
	    pathDynspec = "DYNSPEC_0%s"%(nofSelectedDynspec)
        if nofSelectedDynspec > 99 and nofSelectedDynspec<1000:   
            pathDynspec = "DYNSPEC_%s"%(nofSelectedDynspec)
        
        self.dynspec = pathDynspec
        
        h5File = File(self.PATH0, File.READ)
        
        selectedDynspec= Group(h5File, pathDynspec)
        selectCoordinates = Group(selectedDynspec,"COORDINATES")
        selectTime = Group(selectCoordinates,"TIME")
        selectSpectral = Group(selectCoordinates,"SPECTRAL")
        selectPolarization=Group(selectCoordinates,"POLARIZATION")        
        
        selectedDynspec= Group(h5File, pathDynspec)

        
        
        obsID = AttributeString(h5File, "OBSERVATION_ID")
        obsTarget=AttributeString(h5File, "TARGET")
        obsRA = AttributeFloat(selectedDynspec, "POINT_RA")
        obsDEC = AttributeFloat(selectedDynspec, "POINT_DEC")
        obsStationList = AttributeVString(selectedDynspec, "BEAM_STATIONS_LIST")
        obsNuMin = AttributeFloat(selectedDynspec,"BEAM_FREQUENCY_MIN")
        obsNuMax = AttributeFloat(selectedDynspec, "BEAM_FREQUENCY_MAX") 
        obsNuCenter= AttributeFloat(selectedDynspec, "BEAM_FREQUENCY_CENTER")
        obsNuStep = AttributeFloat(selectedDynspec, "DYNSPEC_BANDWIDTH")
        obsTimeIncrement = AttributeFloat(selectTime, "INCREMENT")
        obsDuration = AttributeFloat(h5File, "TOTAL_INTEGRATION_TIME")
        obsTstart = AttributeString(h5File, "OBSERVATION_START_UTC")
        obsTstop = AttributeString(h5File, "OBSERVATION_END_UTC")
        obsStokes = AttributeVString(selectPolarization,"AXIS_NAMES") 

	obsNofFrequencyChannel		= AttributeVFloat(selectSpectral,"AXIS_VALUE_WORLD")
	obsNofChannelPerSubband		= AttributeFloat(h5File,"CHANNELS_PER_SUBANDS")        
        obsChannelWidth			= AttributeFloat(h5File,"CHANNEL_WIDTH")
        obsNofTimeSample		= AttributeFloat(h5File,"NOF_SAMPLES")
        
	dynspecMetaData.update()

 	dynspecMetaData.insert(1.0, """Observation ID: %s\n"""%(obsID.value))
 	dynspecMetaData.insert(2.0, """Target: %s\n"""%(obsTarget.value))
 	dynspecMetaData.insert(3.0, """Ra: %s\n"""%(obsRA.value)) 	
 	dynspecMetaData.insert(4.0, """Dec: %s\n"""%(obsDEC.value))
	dynspecMetaData.insert(5.0, """Station List: %s\n"""%(obsStationList.value))
	
	dynspecMetaData.insert(6.0, "\n")	

	dynspecMetaData.insert(7.0,  """Minimum Frequency : %s\n"""%(obsNuMin.value))
	dynspecMetaData.insert(8.0,  """Maximum Frequency : %s\n"""%(obsNuMax.value))
	dynspecMetaData.insert(9.0,  """Center Frequency : %s\n"""%(obsNuCenter.value))
	dynspecMetaData.insert(10.0,  """Frequency Band Width : %s\n"""%(obsNuStep.value))
	dynspecMetaData.insert(11.0, """Frequency Unit : MHz\n""")
	dynspecMetaData.insert(12.0, """Stokes Parameter : %s\n"""%(obsStokes.value))
	
	dynspecMetaData.insert(13.0, "\n")	
	
	dynspecMetaData.insert(14.0, """Time Increment : %s\n"""%(obsTimeIncrement.value))
	dynspecMetaData.insert(15.0, """Total Duration : %s\n"""%(obsDuration.value))
	dynspecMetaData.insert(16.0, """UTC Start Time : %s\n"""%(obsTstart.value))
	dynspecMetaData.insert(17.0, """UTC Stop Time : %s\n"""%(obsTstop.value))

	
	dynspecMetaData.update()
	
	self.stokesArray = obsStokes.value
	
	self.observationID		= obsID.value
	
	self.tStart			= 0
	self.tStop			= obsDuration.value
	self.tIncrement			= obsTimeIncrement.value
	self.fMin			= obsNuMin.value
	self.fMax			= obsNuMax.value
	self.nofChannelsPerSubband	= int(obsNofChannelPerSubband.value)
	
	
	AXIS_VALUE_WORLD_TEMP		= obsNofFrequencyChannel.value
	k				= range(len(AXIS_VALUE_WORLD_TEMP))
	self.AXIS_VALUE_WORLD		= range(len(AXIS_VALUE_WORLD_TEMP))
	for i in k:
	    self.AXIS_VALUE_WORLD[i]	= AXIS_VALUE_WORLD_TEMP[i]/1E6	

	
	self.nofSubbands		= int(len(self.AXIS_VALUE_WORLD)/self.nofChannelsPerSubband)
	self.nofTimeSample		= obsNofTimeSample.value	
		
	
	### Fix time and frequency bins 
	########################

	self.minFrequency	= obsNuMin.value
	self.maxFrequency	= obsNuMax.value	
	self.totalTime		= obsDuration.value

	self.NofChannelPerSubband	= obsNofChannelPerSubband.value
	self.NofSubband			= len(obsNofFrequencyChannel.value)/self.NofChannelPerSubband
	self.ChannelWidth		= obsChannelWidth.value
	
	
	self.FrequencyTotalBanWidth	= obsNuStep.value
	
	

	
        RawFileSize 	= os.path.getsize(self.PATH0)/1E9
        oneSAPRawFileSize = RawFileSize
        allSAPRawFileSize = oneSAPRawFileSize
	

	self.SAPRawFileSize = oneSAPRawFileSize
	
	self.dplotBbutton.config(state=NORMAL)
	
      
    ################################################"    
    ## EXTRACT ALL SAP WITH Rebin ... 
    ################################################" 	    
      
    def extractAllSAPRebin(self,event):

    
	self.SelectionRebin = Tk()
	self.SelectionRebin.geometry("915x340+0+0")
	self.SelectionRebin.title("Time and Frequency selection and rebinning")
	
	
	self.parent1 = Frame(self.SelectionRebin)
        self.parent1.pack() 
	
	self.parent2 = Frame(self.SelectionRebin)
        self.parent2.pack()
        
	self.parent3 = Frame(self.SelectionRebin)
        self.parent3.pack()       
        
	self.parent4 = Frame(self.SelectionRebin)
        self.parent4.pack()  
        
	self.parent5 = Frame(self.SelectionRebin)
        self.parent5.pack()      
 
	self.parent6 = Frame(self.SelectionRebin)
        self.parent6.pack()
        
    
	self.TminSelection = Label(self.parent1,             text="""Time Start (in s, must be greater or equal to %s)                                                                                           """%(self.tStart))
	self.TmaxSelection = Label(self.parent2,             text="""Time Stop (in s, must be lower or equal to %s)                                                                              """%(self.tStop))
	self.FminSelection = Label(self.parent3,             text="""Frequency Start (in MHz, must be greater or equal to %s)                                                                 """%(self.fMin))
	self.FmaxSelection = Label(self.parent4,             text="""Frequency Stop (in MHz, must be lower or equal to %s)                                                                    """%(self.fMax))
	self.TincSelection		= Label(self.parent5, text="""New Time increment (in s, must be greater or equal to %s)                                                             """%(self.tIncrement))
	self.FNofChaSubSelection	= Label(self.parent6, text="""New Number of Channels per subbands (must =< to %s,  and =1 or multiple of 2^(n) => 2,4,8,16,32 ... 2^(13))"""%(self.nofChannelsPerSubband))
      	
	self.TminValue 		= Entry(self.parent1, bd =15)
	self.TmaxValue 		= Entry(self.parent2, bd =15)
	self.FminValue 		= Entry(self.parent3, bd =15)
	self.FmaxValue 		= Entry(self.parent4, bd =15)
	self.TincValue 		= Entry(self.parent5, bd =15)
	self.FNofChaSubValue 	= Entry(self.parent6, bd =15)

 	self.TminSelection.pack( side = LEFT)
	self.TminValue.pack( side =  RIGHT)
	
	self.TmaxSelection.pack( side = LEFT)
	self.TmaxValue.pack( side =  RIGHT)
	
	self.FminSelection.pack( side = LEFT)
	self.FminValue.pack( side =  RIGHT)
	
	self.FmaxSelection.pack( side = LEFT)
	self.FmaxValue .pack( side =  RIGHT)
	
	self.TincSelection.pack( side = LEFT)
	self.TincValue.pack( side =  RIGHT)
	
	self.FNofChaSubSelection.pack( side = LEFT)	
	self.FNofChaSubValue.pack( side =  RIGHT)
   
	self.SelectionRebinButton= Button(self.SelectionRebin,text="GO",width=9, height=1)
        self.SelectionRebinButton.pack(side = BOTTOM) 
   
   
	self.SelectionRebinButton.bind("<Button-1>", lambda event, TminValue = self.TminValue, TmaxValue = self.TmaxValue, FminValue = self.FminValue, FmaxValue= self.FmaxValue, TincValue= self.TincValue, FNofChaSubValue = self.FNofChaSubValue,SelectionRebin = self.SelectionRebin : self.extractAllSAPRebinGO(self,TminValue, TmaxValue, FminValue, FmaxValue, TincValue, FNofChaSubValue,SelectionRebin))      


    def badParameterButtonOK(self,event,badParameterBox,SelectionRebin):
	
	badParameterBox.destroy()
	self.SelectionRebinButton.config(state=NORMAL)	
	
	
	
    def extractAllSAPRebinGO(self,event,TminValue, TmaxValue, FminValue, FmaxValue, TincValue, FNofChaSubValue,SelectionRebin):   
            
      
      
	self.GOindicator 		= 'yes'
	
	if TminValue.get() == '' or float(TminValue.get()) < self.tStart:	  
	    self.SelectionRebinButton.config(state=DISABLED)
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Start (in s) is <%s or empty"""%(self.tStart))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	    
	if TmaxValue.get() == ''or float(TmaxValue.get()) > self.tStop:  
	    self.SelectionRebinButton.config(state=DISABLED)	  
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Stop (in s) is >%s or empty"""%(self.tStop))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if float(TmaxValue.get()) < float(TminValue.get()):	
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Stop (in s) is <Time Start (in s)""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if FminValue.get() == '' or float(FminValue.get()) > self.fMax:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Minimum Frequency is empty or > Fmax (%s MHz)"""%(self.fMax))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	if FmaxValue.get() == '' or float(FmaxValue.get()) < self.fMin:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Maximum Frequency is empty or < Fmin (%s MHz)"""%(self.fMin))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	if float(FmaxValue.get()) < float(FminValue.get()):
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Maximum Frequency < Minimum Frequency""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	if TincValue.get() == '' or float(TincValue.get()) < self.tIncrement:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New time Increment < %s"""%(self.tIncrement))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	
	FNofChaSubValue_temp	= int(round(float(FNofChaSubValue.get())))
	if FNofChaSubValue.get() == '': 
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is empty""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	if FNofChaSubValue_temp > self.nofChannelsPerSubband:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is > %s"""%(self.nofChannelsPerSubband))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	if FNofChaSubValue_temp !=1:
	    if FNofChaSubValue_temp&(FNofChaSubValue_temp-1) !=0 or FNofChaSubValue_temp<2:
		self.SelectionRebinButton.config(state=DISABLED)	
		self.GOindicator = 'no'
		self.badParameterBox = Tk()
		self.badParameterBox.geometry("500x40+0+0")
		self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
		self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is not a multiple of 2^(n) or <2 """)  
		self.badParameter.pack()	    
		self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
		self.badParameterButton.pack(side = BOTTOM) 	    
		self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
		    
		
	
	#################################################
	# C++ Computation
	
	if self.GOindicator == 'yes':
	
	
	
	    index_fmin	= 0
	    start_min		= self.AXIS_VALUE_WORLD[0]  	    
	    while start_min < float(FminValue.get()):
		    index_fmin 	= index_fmin+1
		    start_min 	= self.AXIS_VALUE_WORLD[index_fmin*self.nofChannelsPerSubband]
	
	
	    index_fmax	= self.nofSubbands-1 
	    start_max	= self.AXIS_VALUE_WORLD[len(self.AXIS_VALUE_WORLD)-1]
	    while start_max > float(FmaxValue.get()):
		  index_fmax 	= index_fmax-1
		  start_max 	= self.AXIS_VALUE_WORLD[index_fmax*self.nofChannelsPerSubband+self.nofChannelsPerSubband-1] 
		
		
	    timeIcrementReal 	= 1/(self.nofTimeSample/self.tStop)	
	    timeIndexIncrementRebin	= round(float(TincValue.get())/timeIcrementReal)
	    timeRebin 		= timeIndexIncrementRebin*(1/(self.nofTimeSample/self.tStop))

	    timeIncrement 		= timeRebin
	  
	    timeIndexStart   	= round(float(TminValue.get())/timeIcrementReal)
	    timeIndexStart 	= (timeIndexStart/timeIndexIncrementRebin)*timeIndexIncrementRebin
	    timeMinSelect  	= timeIndexStart*timeIcrementReal
	    
	    timeIndexStop	= round(float(TmaxValue.get())/timeIcrementReal)
	    timeIndexStop 	= timeIndexStop/timeIndexIncrementRebin*timeIndexIncrementRebin
	    timeMaxSelect 	= timeIndexStop*timeIcrementReal
	      
	    self.NEW_NOF_SUBBANDS		= index_fmax-index_fmin+1
	    self.NEW_CHANNELPERSUBBANDS	= int(round(float(FNofChaSubValue.get())))
	    self.NEW_FMIN			= start_min
	    self.NEW_FMAX			= start_max
	    self.NEWTIMEREBIN		= timeIncrement
	    self.NEWTIMESTART		= timeMinSelect
	    self.NEWTIMESTOP		= timeMaxSelect	
	    
	    
	    self.SizeEstimated 	=   (((self.NEWTIMESTOP-self.NEWTIMESTART)/self.NEWTIMEREBIN)*(float(FNofChaSubValue.get())*self.NEW_NOF_SUBBANDS))*(self.SAPRawFileSize)/(self.nofTimeSample*self.nofSubbands*self.nofChannelsPerSubband)
	    
	    RAMOptimumTemp 	= 1/(1.277E8/((self.NEWTIMESTOP-self.NEWTIMESTART)/self.NEWTIMEREBIN*self.NEW_NOF_SUBBANDS*self.NEW_CHANNELPERSUBBANDS))
	    self.RAMOptimum	= round(RAMOptimumTemp,3)+0.05 
	    
	    self.TimeEstimated	= self.SizeEstimated*1.59/0.2256
	    	 

	    ######################################
	    
	    
	    #self.SelectionRebin.destroy()
	    
	    self.SelectionRebinButton.config(state=DISABLED)	
	    
	    self.ResumeAll	= Tk()
	    self.ResumeAll.title("Overview Selection")	
	    self.ResumeAll.geometry("400x220+0+0")
	    
	    self.parent7 = Frame(self.ResumeAll)
	    self.parent7.pack() 
	    
	    self.parent8 = Frame(self.ResumeAll)
	    self.parent8.pack()
	    
	    self.parent9 = Frame(self.ResumeAll)
	    self.parent9.pack()       
	    
	    self.parent10 = Frame(self.ResumeAll)
	    self.parent10.pack()  
	    
	    self.parent11 = Frame(self.ResumeAll)
	    self.parent11.pack()      
    
	    self.parent12 = Frame(self.ResumeAll)
	    self.parent12.pack()
	    
	    self.parent13 = Frame(self.ResumeAll)
	    self.parent13.pack()           
	    
	    self.parent15 = Frame(self.ResumeAll)
	    self.parent15.pack()    	    

	    self.parent16 = Frame(self.ResumeAll)
	    self.parent16.pack() 	    
	    
	    self.TminResumeAll = Label(self.parent7,text="""Time Start (in s): %s) """%(self.NEWTIMESTART))
	    self.TmaxResumeAll = Label(self.parent8,text="""Time Stop (in s): %s)"""%(self.NEWTIMESTOP))
	    self.FminResumeAll = Label(self.parent9,text="""Frequency Start (in MHz): %s)"""%(self.NEW_FMIN))
	    self.FmaxResumeAll = Label(self.parent10,text="""Frequency Stop (in MHz):  %s) """%(self.NEW_FMAX))
	    self.TincResumeAll		= Label(self.parent11, text="""New Time increment %s s"""%(self.NEWTIMEREBIN))
	    self.FNofChaSubResumeAll	= Label(self.parent12, text="""New Number of Channels per subbands %s"""%(self.NEW_CHANNELPERSUBBANDS))
	    self.FILESIZE		= Label(self.parent13, text="""TOTAL of Files in Go %s"""%(self.SizeEstimated))
	    self.RAMOptimumResumeAll	= Label(self.parent15, text="""Optimal RAM Quantity (in Go) for processing: %s"""%(self.RAMOptimum))	    
	    self.TimeEstimatedResumeAll= Label(self.parent16, text="""Optimal Processing Time Estimation (in s): %s"""%(self.TimeEstimated)) 
	    
	    
	    self.TminResumeAll.pack( side = LEFT)
	    self.TmaxResumeAll.pack( side = LEFT)
	    self.FminResumeAll.pack( side = LEFT)
	    self.FmaxResumeAll.pack( side = LEFT)
	    self.TincResumeAll.pack( side = LEFT)
	    self.FNofChaSubResumeAll.pack( side = LEFT)
	    self.FILESIZE.pack( side = LEFT)
	    self.RAMOptimumResumeAll.pack( side = LEFT)
	    self.TimeEstimatedResumeAll.pack( side = LEFT)	    
	    
	    
	    self.ResumeAllButtonYes	= Button(self.ResumeAll,text="GO",width=9, height=1)
	    self.ResumeAllButtonYes.pack(side = LEFT) 
      
	    self.ResumeAllButtonNo	= Button(self.ResumeAll,text="STOP",width=9, height=1)
	    self.ResumeAllButtonNo.pack(side = RIGHT) 
	    
	    self.ResumeAllButtonYes.bind("<Button-1>", lambda event  : self.ResumeAllYes_GO(self))      
	    self.ResumeAllButtonNo.bind("<Button-1>", lambda event,  ResumeAll = self.ResumeAll, SelectionRebin = self.SelectionRebin: self.ResumeAllButtonNo_Go(self, ResumeAll,SelectionRebin))      

        
	
    def ResumeAllYes_GO(self,event):
	
	self.ResumeAllButtonYes.config(state=DISABLED)
	self.ResumeAllButtonNo.config(state=DISABLED)
	
	
	self.outputDir = tkFileDialog.askdirectory(title='Choose Output Directory')	
	self.RamSelect	= Tk()
	self.RamSelect.title("RAM Selection")	
	self.RAMText = Label(self.RamSelect, text="""How much RAM do you want to use ? [Default:1Go], OPTIMAL: %s Go """%(self.RAMOptimum))
	self.RAMText.pack( side = LEFT)	
	self.RAMValue = Entry(self.RamSelect, bd =5)
	self.RAMValue.pack(side = RIGHT)	
	self.RAMButton	= Button(self.RamSelect,text="GO",width=9, height=1)
        self.RAMButton.pack(side = BOTTOM)        
	self.RAMButton.bind("<Button-1>", lambda event, RAMValue = self.RAMValue, RamSelect = self.RamSelect : self.extractALLSAP_REBIN_GO(self,RAMValue,RamSelect))   	
	
	self.ProcessInfoAllSAP()
	
	
    def extractALLSAP_REBIN_GO(self,event,RAMValue,RamSelect):
	
      if RAMValue.get() == '':
	  self.RAM = 1
      else:
	  self.RAM = float(RAMValue.get())
	  if self.RAM <= 0:
	      self.RAM = 1
	    
      self.RamSelect.destroy()
      self.ResumeAll.destroy()
      self.SelectionRebin.destroy()  
      
      
      current_dir = os.getcwd()

      cmd = """(cd %s/ LD_LIBRARY_PATH=/usr/local/lib/; %s/../src/ICD6-ICD6-Rebin/Dyn2Dyn %s L%s  %s/L%s_rebinned.h5  %s %s %s %s %s %s yes %s 0)"""%(self.outputDir,current_dir,self.PATH0,self.observationID,self.outputDir,self.observationID,self.NEWTIMESTART,self.NEWTIMESTOP,self.NEWTIMEREBIN,self.NEW_FMIN,self.NEW_FMAX,self.NEW_CHANNELPERSUBBANDS,self.RAM)                          
      

      print commands.getoutput(cmd)
      self.grandparent.quit()  	
	
	
    def ResumeAllButtonNo_Go(self,event,ResumeAll,SelectionRebin):
	    ResumeAll.destroy()
	    self.SelectionRebinButton.config(state=NORMAL)	    
    
    
    
    

    
    
    
    
    ###########################################################################      
    #Process Info
    
    def ProcessInfoAllSAP(self):
      
	self.ProcessWindow      = Tk()
	self.ProcessWindow.title("Processing Information")      
	self.ProcessWindow.geometry("400x140+0+0")      
	self.parent17 = Frame(self.ProcessWindow)
	self.parent17.pack()       
	self.parent18 = Frame(self.ProcessWindow)
	self.parent18.pack()            
	self.parent20 = Frame(self.ProcessWindow)
	self.parent20.pack()        
	self.parent21 = Frame(self.ProcessWindow)
	self.parent21.pack()        
	self.parent22 = Frame(self.ProcessWindow)
	self.parent22.pack()  

	self.StartTimeProcess	= time.asctime()
	self.StartInfo       	= Label(self.parent17,text=""" Start Processing at: %s"""%(self.StartTimeProcess))
	self.DataInfo      	= Label(self.parent18,text="""Data Flow to Process (Go): %s"""%(self.SizeEstimated))
	self.RAMOpInfo       	= Label(self.parent20,text="""Optimal RAM to process (Go): %s)"""%(self.RAMOptimum))
	self.RAMInfo       	= Label(self.parent21,text="""Selected RAM Quantity (Go): %s)"""%(self.RAM))
	self.TimeInfo      	= Label(self.parent22,text="""Optimal Processing Time Estimation (in s):  %s) """%(self.TimeEstimated))
	
	self.StartInfo.pack( side = LEFT)
	self.DataInfo.pack( side = LEFT)
	self.RAMOpInfo.pack( side = LEFT)
	self.RAMInfo.pack( side = LEFT)
	self.TimeInfo.pack( side = LEFT)

	
