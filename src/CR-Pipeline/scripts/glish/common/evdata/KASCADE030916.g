# lopes event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: 2003/09/16/16:31:01

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'Karlsruhe at KASCADE'
evmeta.allhead.Observatory := 'LOPES';
evmeta.allhead.AntennaCalFile := 'empty.tab';

# channel-specific headers

evmeta.ch['10101'].AntPos := '13.573 ,-50.195 ,124.729'; #C9Q1  #pos3
evmeta.ch['10101'].ClusterPos := 'Cluster 9 Quadrant 1  ';
evmeta.ch['10101'].AntennaCalFile := 'lopes10-alteich.tab';
evmeta.ch['10102'].AntPos := '31.937 ,-18.275 ,124.667'; #C9Q3  #pos4
evmeta.ch['10102'].ClusterPos := 'Cluster 9 Quadrant 3  ';
evmeta.ch['10102'].AntennaCalFile := 'lopes10-alteich.tab';
evmeta.ch['10201'].AntPos := '0.000  ,0.000   ,124.597'; #C10Q1 #pos5
evmeta.ch['10201'].ClusterPos := 'Cluster 10 Quadrant 1  ';
evmeta.ch['10201'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['10202'].AntPos := '18.161 ,31.969  ,124.726'; #C10Q3 #pos6
evmeta.ch['10202'].ClusterPos := 'Cluster 10 Quadrant 3  ';
evmeta.ch['10202'].AntennaCalFile := 'lopes10-alteich.tab';
evmeta.ch['20101'].AntPos := '-20.561,75.420  ,124.652'; #C11Q2 #pos7
evmeta.ch['20101'].ClusterPos := 'Cluster 11 Quadrant 2  ';
evmeta.ch['20101'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['20102'].AntPos := '11.632 ,57.116  ,124.674'; #C11Q4 #pos8
evmeta.ch['20102'].ClusterPos := 'Cluster 11 Quadrant 4  ';
evmeta.ch['20102'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['20201'].AntPos := '50.007 ,13.485  ,124.574'; #C14Q1 #pos9
evmeta.ch['20201'].ClusterPos := 'Cluster 14 Quadrant 1  ';
evmeta.ch['20201'].AntennaCalFile := 'lopes10-alteich.tab';
evmeta.ch['20202'].AntPos := '68.283 ,45.522  ,124.507'; #C14Q3 #pos10
evmeta.ch['20202'].ClusterPos := 'Cluster 14 Quadrant 3  ';
evmeta.ch['20202'].AntennaCalFile := 'lopes10-alteich.tab';

