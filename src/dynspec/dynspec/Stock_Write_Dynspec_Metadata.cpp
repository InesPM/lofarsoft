#include <iostream>
#include <string>

#include "Stock_Write_Dynspec_Metadata.h"

#include <dal/lofar/BF_File.h>

using namespace DAL;

using namespace std;


  Stock_Write_Dynspec_Metadata::Stock_Write_Dynspec_Metadata(){}
  Stock_Write_Dynspec_Metadata::~Stock_Write_Dynspec_Metadata(){}


  void Stock_Write_Dynspec_Metadata::stockDynspecMetadata(string attr_69,string attr_70,string attr_71,string attr_72,string attr_73,string attr_74,string attr_75,double attr_76,double attr_77,string attr_78,string attr_79,
							  string attr_80,double attr_81,double attr_82,double attr_83,double attr_84,double attr_85,double attr_86,double attr_87,double attr_88,double attr_89,
							  string attr_90,double attr_91,vector<string> attr_92,string attr_93,double attr_94,string attr_95,bool attr_96,vector<string> attr_97,bool attr_98,string attr_99,
							  string attr_100,vector<double> attr_101,vector<string> attr_102,string attr_103,double attr_104,string attr_105,string attr_106,double attr_107,double attr_108,
							  vector<string> attr_109,string attr_110,string attr_111,vector<string> attr_112,int attr_113,vector<string> attr_114,vector<string> attr_115,vector<double> attr_116,
							  vector<double> attr_117,vector<double> attr_118,vector<double> attr_119,vector<double> attr_121,vector<double> attr_122, string attr_123,string attr_124,
							  vector<string> attr_125,int attr_126,vector<string> attr_127,vector<string> attr_128,vector<double> attr_129,vector<double> attr_130,vector<double> attr_131,
							  vector<double> attr_132,vector<double> attr_133,vector<double> attr_135,string attr_137,string attr_138,vector<string> attr_139,int attr_140,
 							  vector<string> attr_141,vector<string> attr_142,int Ntime,int Nspectral,string attr_147,string attr_148,int attr_149,string attr_150,string attr_151,
							  int attr_152,vector<string> attr_153,vector<string> attr_154,vector<string> attr_155,vector<string> attr_156,vector<string> attr_157,vector<string> attr_158,
							  string attr_159,bool attr_160,bool attr_161,bool attr_162,bool attr_163) 
 {
   // Stock DYNSPEC metadata in an object
   
    // DYNSPEC group meta-data
    
    m_attr_69	= attr_69;
    m_attr_70	= attr_70;
    m_attr_71	= attr_71;
    m_attr_72	= attr_72;
    m_attr_73	= attr_73;
    m_attr_74	= attr_74;
    m_attr_75	= attr_75;
    m_attr_76	= attr_76;
    m_attr_77	= attr_77;
    m_attr_78	= attr_78;
    m_attr_79 	= attr_79;    
    m_attr_80	= attr_80;
    m_attr_81	= attr_81;
    m_attr_82	= attr_82;
    m_attr_83	= attr_83;
    m_attr_84	= attr_84;
    m_attr_85	= attr_85;
    m_attr_86	= attr_86;
    m_attr_87	= attr_87;
    m_attr_88	= attr_88;
    m_attr_89	= attr_89;
    m_attr_90	= attr_90;
    m_attr_91	= attr_91;
 
    m_size_attr_92= attr_92.size();
    m_attr_92.resize(m_size_attr_92);
    for (int i=0;i< attr_92.size();i++)
    {m_attr_92[i] = attr_92[i];}
 
    m_attr_93	= attr_93;
    m_attr_94	= attr_94;
    m_attr_95	= attr_95;
    m_attr_95	= attr_96;
    
    m_size_attr_97= attr_97.size();
    m_attr_97.resize(m_size_attr_97);
    for (int i=0;i< attr_97.size();i++)
    {m_attr_97[i] = attr_97[i];}
 
    m_attr_98	= attr_98;
    m_attr_99	= attr_99; 
    m_attr_100	= attr_100;
    
    m_size_attr_101= attr_101.size();
    m_attr_101.resize(m_size_attr_101);
    for (int i=0;i< attr_101.size();i++)
    {m_attr_101[i] = attr_101[i];}
    
    m_size_attr_102= attr_102.size();
    m_attr_102.resize(m_size_attr_102);
    for (int i=0;i< attr_102.size();i++)
    {m_attr_102[i] = attr_102[i];}

    m_attr_103	= attr_103;
    m_attr_104	= attr_104;
    m_attr_105	= attr_105;
    m_attr_106	= attr_106;
    m_attr_107	= attr_107;
    m_attr_108	= attr_108;
      
    m_size_attr_109= attr_109.size();
    m_attr_109.resize(m_size_attr_109);
    for (int i=0;i< attr_109.size();i++)
    {m_attr_109[i] = attr_109[i];}
    
    
    m_attr_110	= attr_110;
    m_attr_111	= attr_111;
    
    m_size_attr_112= attr_112.size();
    m_attr_112.resize(m_size_attr_112);
    for (int i=0;i< attr_112.size();i++)
    {m_attr_112[i] = attr_112[i];}
    
    m_attr_113	= attr_113;
    
    m_size_attr_114= attr_114.size();
    m_attr_114.resize(m_size_attr_114);
    for (int i=0;i< attr_114.size();i++)
    {m_attr_114[i] = attr_114[i];}
    
    m_size_attr_115= attr_115.size();
    m_attr_115.resize(m_size_attr_115);
    for (int i=0;i< attr_115.size();i++)
    {m_attr_115[i] = attr_115[i];}
    
    m_size_attr_116= attr_116.size();
    m_attr_116.resize(m_size_attr_116);
    for (int i=0;i< attr_116.size();i++)
    {m_attr_116[i] = attr_116[i];}
    
    m_size_attr_117= attr_117.size();
    m_attr_117.resize(m_size_attr_117);
    for (int i=0;i< attr_117.size();i++)
    {m_attr_117[i] = attr_117[i];}
    
    m_size_attr_118= attr_118.size();
    m_attr_118.resize(m_size_attr_118);
    for (int i=0;i< attr_118.size();i++)
    {m_attr_118[i] = attr_118[i];}
    
    m_size_attr_119= attr_119.size();
    m_attr_119.resize(m_size_attr_119);
    for (int i=0;i< attr_119.size();i++)
    {m_attr_119[i] = attr_119[i];}
			        
    m_size_attr_121= attr_121.size();
    m_attr_121.resize(m_size_attr_121);
    for (int i=0;i< attr_121.size();i++)
    {m_attr_121[i] = attr_121[i];}     

    m_size_attr_122= attr_122.size();
    m_attr_122.resize(m_size_attr_122);
    for (int i=0;i< attr_122.size();i++)
    {m_attr_122[i] = attr_122[i];}

    m_attr_123	= attr_123;
    m_attr_124	= attr_124;
    
    m_size_attr_125= attr_125.size();
    m_attr_125.resize(m_size_attr_125);
    for (int i=0;i< attr_125.size();i++)
    {m_attr_125[i] = attr_125[i];}
    
    m_attr_126	= attr_126;
    
    m_size_attr_127= attr_127.size();
    m_attr_127.resize(m_size_attr_127);
    for (int i=0;i< attr_127.size();i++)
    {m_attr_127[i] = attr_127[i];}
    
    m_size_attr_128= attr_128.size();
    m_attr_128.resize(m_size_attr_128);
    for (int i=0;i< attr_128.size();i++)
    {m_attr_128[i] = attr_128[i];}
    
    m_size_attr_129= attr_129.size();
    m_attr_129.resize(m_size_attr_129);
    for (int i=0;i< attr_129.size();i++)
    {m_attr_129[i] = attr_129[i];}
    
    m_size_attr_130= attr_130.size();
    m_attr_130.resize(m_size_attr_130);
    for (int i=0;i< attr_130.size();i++)
    {m_attr_130[i] = attr_130[i];}
    
    m_size_attr_131= attr_131.size();
    m_attr_131.resize(m_size_attr_131);
    for (int i=0;i< attr_131.size();i++)
    {m_attr_131[i] = attr_131[i];}
    
    m_size_attr_132= attr_132.size();
    m_attr_132.resize(m_size_attr_132);
    for (int i=0;i< attr_132.size();i++)
    {m_attr_132[i] = attr_132[i];}
    
    m_size_attr_133= attr_133.size();
    m_attr_133.resize(m_size_attr_133);
    for (int i=0;i< attr_133.size();i++)
    {m_attr_133[i] = attr_133[i];}
    
    m_size_attr_135= attr_135.size();
    m_attr_135.resize(m_size_attr_135);
    for (int i=0;i< attr_135.size();i++)
    {m_attr_135[i] = attr_135[i];}

    m_attr_137	= attr_137;
    m_attr_138	= attr_138;
    
    m_size_attr_139= attr_139.size();
    m_attr_139.resize(m_size_attr_139);
    for (int i=0;i< attr_139.size();i++)
    {m_attr_139[i] = attr_139[i];}
    
    m_attr_140	= attr_140;
    
    m_size_attr_141= attr_141.size();
    m_attr_141.resize(m_size_attr_141);
    for (int i=0;i< attr_141.size();i++)
    {m_attr_141[i] = attr_141[i];}
    
    m_size_attr_142= attr_142.size();
    m_attr_142.resize(m_size_attr_142);
    for (int i=0;i< attr_142.size();i++)
    {m_attr_142[i] = attr_142[i];}
    
    m_Ntime	= Ntime;
    m_Nspectral	= Nspectral;
    m_attr_147	= attr_147;
    m_attr_148	= attr_148;
    m_attr_149	= attr_149;
    m_attr_150	= attr_150;
    m_attr_151	= attr_151;
    m_attr_152	= attr_152;
    
    m_size_attr_153= attr_153.size();
    m_attr_153.resize(m_size_attr_153);
    for (int i=0;i< attr_153.size();i++)
    {m_attr_153[i] = attr_153[i];}   
        
    m_size_attr_154= attr_154.size();
    m_attr_154.resize(m_size_attr_154);
    for (int i=0;i< attr_154.size();i++)
    {m_attr_154[i] = attr_154[i];}       
    
    m_size_attr_155= attr_155.size();
    m_attr_155.resize(m_size_attr_155);
    for (int i=0;i< attr_155.size();i++)
    {m_attr_155[i] = attr_155[i];}    
    
    m_size_attr_156= attr_156.size();
    m_attr_156.resize(m_size_attr_156);
    for (int i=0;i< attr_156.size();i++)
    {m_attr_156[i] = attr_156[i];}    
    
    m_size_attr_157= attr_157.size();
    m_attr_157.resize(m_size_attr_157);
    for (int i=0;i< attr_157.size();i++)
    {m_attr_157[i] = attr_157[i];}    
    
    m_size_attr_158= attr_158.size();
    m_attr_158.resize(m_size_attr_158);
    for (int i=0;i< attr_158.size();i++)
    {m_attr_158[i] = attr_158[i];}    
    
    m_attr_159	= attr_159;
    m_attr_160	= attr_160;
    m_attr_161	= attr_161;
    m_attr_162	= attr_162;
    m_attr_163	= attr_163;
  }


////////////////////////////////////////////////////////////////////////////////////////////////////////


  void Stock_Write_Dynspec_Metadata::writeDynspecMetadata(Group &dynspec_grp,string pathDir,string obsName,string pathFile,string outputFile,File &root_grp,int i,int j,int k,int l,int q,int obsNofStockes,vector<string> stokesComponent,float memoryRAM)
  {
    
      // Write the DYNSPEC meta-data in the output file

      string index_i1,index_j1,index_k1,index_q1;
      std::ostringstream oss_i;oss_i << i;string index_i(oss_i.str());
      if (i<10){index_i1="_SAP00"+index_i;}
      if (i>=10 && i<100){index_i1="_SAP0"+index_i;}
      if (i>=100 && i<1000){index_i1="_SAP"+index_i;}
      
      std::ostringstream oss_j;oss_j << j;string index_j(oss_j.str()); 
      if (j<10){index_j1="_B00"+index_j;}
      if (j>=10 && i<100){index_j1="_B0"+index_j;}
      if (j>=100 && i<1000){index_j1="_B"+index_j;}
      
      std::ostringstream oss_q;oss_q << q;string index_q(oss_q.str()); 
      if (q<10){index_q1="_P00"+index_q;}
      if (q>=10 && i<100){index_q1="_P0"+index_q;}
      if (q>=100 && i<1000){index_q1="_P"+index_q;}      
      
      
      std::ostringstream oss_k;oss_k << k;string index_k(oss_k.str());
      if (k<10){index_k1="00"+index_k;}
      if (k>=10 && k<100){index_k1="0"+index_k;}
      if (k>=100 && k<1000){index_k1=index_k;}
      
      pathFile	= pathFile+index_i1+index_j1+"_S0"+index_q1+"_bf.h5"; 
      
      /////////////////////////////////////////////////
     
      // SUB GROUPS
      Group coords_grp(dynspec_grp, "COORDINATES");
      coords_grp.create();
      //SUB GROUP EVENT
      Group event_grp(dynspec_grp, "EVENT");
      event_grp.create();

       //SUB GROUP EVENT      
      Group process_histo_grp(dynspec_grp, "PROCESS_HISTORY");           
      process_histo_grp.create();   

      //SUBSUB GROUP 
      Group time_grp(coords_grp, "TIME");
      time_grp.create();
      
      Group spectral_grp(coords_grp, "SPECTRAL");
      spectral_grp.create();       

      Group polarization_grp(coords_grp, "POLARIZATION");
      polarization_grp.create(); 
      
      
      // META DATA
      
      
      // DYNSPEC GROUP

      Attribute<string> attr_69(dynspec_grp, "GROUPETYPE");
      Attribute<string> attr_70(dynspec_grp, "DYNSPEC_START_MJD");
      Attribute<string> attr_71(dynspec_grp, "DYNSPEC_STOP_MJD");
      Attribute<string> attr_72(dynspec_grp, "DYNSPEC_START_UTC");
      Attribute<string> attr_73(dynspec_grp, "DYNSPEC_STOP_UTC");
      Attribute<string> attr_74(dynspec_grp, "DYNSPEC_START_TAI");
      Attribute<string> attr_75(dynspec_grp, "DYNSPEC_STOP_TAI");
      Attribute<double> attr_76(dynspec_grp, "DYNSPEC_BANDWIDTH");
      Attribute<double> attr_77(dynspec_grp, "BEAM_DIAMETER");     
      Attribute<string> attr_78(dynspec_grp, "TRACKING");
      Attribute<string> attr_79(dynspec_grp, "TARGET");
      Attribute<string> attr_80(dynspec_grp, "ONOFF");     
      Attribute<double> attr_81(dynspec_grp, "POINT_RA");
      Attribute<double> attr_82(dynspec_grp, "POINT_DEC");     
      Attribute<double> attr_83(dynspec_grp, "POSITION_OFFSET_RA");
      Attribute<double> attr_84(dynspec_grp, "POSITION_OFFSET_DEC");
      Attribute<double> attr_85(dynspec_grp, "BEAM_DIAMETER_RA");
      Attribute<double> attr_86(dynspec_grp, "BEAM_DIAMETER_DEC");     
      Attribute<double> attr_87(dynspec_grp, "BEAM_FREQUENCY_MAX");
      Attribute<double> attr_88(dynspec_grp, "BEAM_FREQUENCY_MIN");
      Attribute<double> attr_89(dynspec_grp, "BEAM_FREQUENCY_CENTER");
      Attribute<string> attr_90(dynspec_grp, "BEAM_FREQUENCY_UNIT");      
      Attribute<double> attr_91(dynspec_grp, "BEAM_NOF_STATIONS");
      Attribute< vector<string> > attr_92(dynspec_grp, "BEAM_STATIONS_LIST");     
      Attribute<string> attr_93(dynspec_grp, "DEDISPERSION");
      Attribute<double> attr_94(dynspec_grp, "DISPERSION_MEASURE");
      Attribute<string> attr_95(dynspec_grp, "DISPERSION_MEASURE_UNIT");
      Attribute<bool> attr_96(dynspec_grp, "BARYCENTER");
      Attribute< vector<string> > attr_97(dynspec_grp, "STOCKES_COMPONENT");
      Attribute<bool> attr_98(dynspec_grp, "COMPLEX_VOLTAGE");
      Attribute<string> attr_99(dynspec_grp, "SIGNAL_SUM");
      
      
      attr_69.value = m_attr_69;
      attr_70.value = m_attr_70;  
      attr_71.value = m_attr_71;
      attr_72.value = m_attr_72;
      attr_73.value = m_attr_73;
      attr_74.value = m_attr_74;
      attr_75.value = m_attr_75;
      attr_76.value = m_attr_76;
      attr_77.value = m_attr_77;
      attr_78.value = m_attr_78;
      attr_79.value = m_attr_79;   
      attr_80.value = m_attr_80;
      attr_81.value = m_attr_81;      
      attr_82.value = m_attr_82;
      attr_83.value = m_attr_83;
      attr_84.value = m_attr_84;
      attr_85.value = m_attr_85;
      attr_86.value = m_attr_86;
      attr_87.value = m_attr_87;
      attr_88.value = m_attr_88;
      attr_89.value = m_attr_89;      
      attr_90.value = m_attr_90;
      attr_91.value = m_attr_91;      
      attr_92.create().set(m_attr_92);    
      attr_93.value = m_attr_93;
      attr_94.value = m_attr_94;
      attr_95.value = m_attr_95;
      attr_96.value = m_attr_96;      
      attr_97.create().set(m_attr_97);     
      attr_98.value = m_attr_98;
      attr_99.value = m_attr_99;
            
      
      // SUB-GROUP COORDINATES
      
      Attribute<string> attr_100(coords_grp, "GROUPE_TYPE");
      Attribute< vector<double> > attr_101(coords_grp, "REF_LOCATION_VALUE");
      Attribute< vector<string> > attr_102(coords_grp, "REF_LOCATION_UNIT");
      Attribute<string> attr_103(coords_grp, "REF_LOCATION_FRAME");
      Attribute<double> attr_104(coords_grp, "REF_TIME_VALUE");
      Attribute<string> attr_105(coords_grp, "REF_TIME_UNIT");
      Attribute<string> attr_106(coords_grp, "REF_TIME_FRAME");
      Attribute<double> attr_107(coords_grp, "NOF_COORDINATES");
      Attribute<double> attr_108(coords_grp, "NOF_AXIS");
      Attribute< vector<string> > attr_109(coords_grp, "COORDINATE_TYPES");      
      
      attr_100.value = m_attr_100;      
      attr_101.create().set(m_attr_101);
      attr_102.create().set(m_attr_102);
      attr_103.value = m_attr_103;
      attr_104.value = m_attr_104;
      attr_105.value = m_attr_105;
      attr_106.value = m_attr_106;
      attr_107.value = m_attr_107;
      attr_108.value = m_attr_108;
      attr_109.create().set(m_attr_109);      
        
      
      //SUBGROUP EVENT

      Attribute<string> attr_147(event_grp,"GROUPE_TYPE");
      Attribute<string> attr_148(event_grp,"DATASET");
      Attribute<int> attr_149(event_grp,"N_AXIS");
      Attribute<string> attr_150(event_grp,"N_AXIS_1");
      Attribute<string> attr_151(event_grp,"N_AXIS_2");
      Attribute<int> attr_152(event_grp,"N_EVENT");
      Attribute< vector<string> >attr_153(event_grp,"FIELD_1");
      Attribute< vector<string> >attr_154(event_grp,"FIELD_2");
      Attribute< vector<string> >attr_155(event_grp,"FIELD_3");
      Attribute< vector<string> >attr_156(event_grp,"FIELD_4");
      Attribute< vector<string> >attr_157(event_grp,"FIELD_5");
      Attribute< vector<string> >attr_158(event_grp,"FIELD_6");
      
      attr_147.value	= m_attr_147;
      attr_148.value	= m_attr_148;
      attr_149.value	= m_attr_149;
      attr_150.value	= m_attr_150;
      attr_151.value	= m_attr_151;
      attr_152.value	= m_attr_152;
      attr_153.create().set(m_attr_153); 
      attr_154.create().set(m_attr_154);      
      attr_155.create().set(m_attr_155);   
      attr_156.create().set(m_attr_156);   
      attr_157.create().set(m_attr_157);   
      attr_158.create().set(m_attr_158);   

      
      // SUB GROUP PROCESS HISTO

      Attribute<string> attr_159(process_histo_grp,"GROUPE_TYPE");      
      Attribute<bool> attr_160(process_histo_grp,"OBSERVATION_PARSET");        
      Attribute<bool> attr_161(process_histo_grp,"OBSERVATION_LOG");
      Attribute<bool> attr_162(process_histo_grp,"DYNSPEC_PARSET");
      Attribute<bool> attr_163(process_histo_grp,"DYNSPEC_LOG");
      
      attr_159.value	= m_attr_159;
      attr_160.value	= m_attr_160;
      attr_161.value	= m_attr_161;
      attr_162.value	= m_attr_162;
      attr_163.value	= m_attr_163;
      
      // SUB-SUB-SUBGROUPS
      
      //TIME

      Attribute<string> attr_110(time_grp, "GROUPE_TYPE");
      Attribute<string> attr_111(time_grp, "COORDINATE_TYPE");
      Attribute< vector<string> > attr_112(time_grp, "STORAGE_TYPE");
      Attribute<int> attr_113(time_grp, "NOF_AXES");
      Attribute< vector<string> > attr_114(time_grp, "AXIS_NAMES");
      Attribute< vector<string> > attr_115(time_grp, "AXIS_UNIT");      
      Attribute< vector<double> > attr_116(time_grp, "REFERENCE_VALUE");
      Attribute< vector<double> > attr_117(time_grp, "REFERENCE_PIXEL");
      Attribute< vector<double> > attr_118(time_grp, "INCREMENT");
      Attribute< vector<double> > attr_119(time_grp, "PC");          
      Attribute< vector<double> > attr_121(time_grp, "AXIS_VALUE_PIXEL");
      Attribute< vector<double> > attr_122(time_grp, "AXIS_VALUE_WORLD");
      
      attr_110.value = m_attr_110;
      attr_111.value = m_attr_111;
      attr_112.create().set(m_attr_112);      
      attr_113.value = m_attr_113;
      attr_114.create().set(m_attr_114); 
      attr_115.create().set(m_attr_115); 
      attr_116.create().set(m_attr_116);
      attr_117.create().set(m_attr_117);
      attr_118.create().set(m_attr_118);
      attr_119.create().set(m_attr_119);      
      attr_121.create().set(m_attr_121);
      attr_122.create().set(m_attr_122);        
      
      // SPECTRAL

      Attribute<string> attr_123(spectral_grp, "GROUPE_TYPE");
      Attribute<string> attr_124(spectral_grp, "COORDINATE_TYPE");
      Attribute< vector<string> > attr_125(spectral_grp, "STORAGE_TYPE");
      Attribute<int> attr_126(spectral_grp, "NOF_AXES");
      Attribute< vector<string> > attr_127(spectral_grp, "AXIS_NAMES");
      Attribute< vector<string> > attr_128(spectral_grp, "AXIS_UNIT");
      Attribute< vector<double> > attr_129(spectral_grp, "REFERENCE_VALUE");
      Attribute< vector<double> > attr_130(spectral_grp, "REFERENCE_PIXEL");
      Attribute< vector<double> > attr_131(spectral_grp, "INCREMENT");
      Attribute< vector<double> > attr_132(spectral_grp, "PC");
      Attribute< vector<double> > attr_133(spectral_grp, "AXIS_VALUE_WORLD");
      Attribute< vector<double> > attr_135(spectral_grp, "AXIS_VALUE_PIXEL");
      
      attr_123.value = m_attr_123;
      attr_124.value = m_attr_124;
      attr_125.create().set(m_attr_125);
      attr_126.value = m_attr_126;
      attr_127.create().set(m_attr_127);
      attr_128.create().set(m_attr_128);
      attr_129.create().set(m_attr_129);
      attr_130.create().set(m_attr_130);
      attr_131.create().set(m_attr_131);
      attr_132.create().set(m_attr_132);
      attr_133.create().set(m_attr_133); 
      attr_135.create().set(m_attr_135);    
      
      // POLARIZATION         

      Attribute<string> attr_137(polarization_grp, "GROUPE_TYPE");
      Attribute<string> attr_138 (polarization_grp, "COORDINATE_TYPE");
      Attribute< vector<string> > attr_139(polarization_grp, "STORAGE_TYPE");
      Attribute<int> attr_140(polarization_grp, "NOF_AXES");
      Attribute< vector<string> > attr_141(polarization_grp, "AXIS_NAMES");
      Attribute< vector<string> > attr_142(polarization_grp, "AXIS_UNIT");
      
      attr_137.value = m_attr_137;
      attr_138.value = m_attr_138;
      attr_139.create().set(m_attr_139);
      attr_140.value = m_attr_140;
      attr_141.create().set(m_attr_141);
      attr_142.create().set(m_attr_142);
	
  }	

	   
 
  
  
  
  