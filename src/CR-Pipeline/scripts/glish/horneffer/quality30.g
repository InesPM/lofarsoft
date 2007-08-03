#This file contains information about the quality of reduced CR events from LOPES.
#The first number is the ID numer in the event table selection file and the second
#Number is a quality value between 0 (bad) and 1 (good) - see crresults.g

print '#-----------------------------------------------------------------------'
print '#Loading default values for the quality - they have been set by "eye".'
print '#Make sure this is what you want ...'
print '#-----------------------------------------------------------------------'
print ' '

quality := array(0.,664);

# -------------------  1    2    3    4    5    6    7    8    9   10
quality[001:010] := [ 0.2, 0. , 0.1, 0.2, 1. , 1. , 0.1, 0.5, 1. , 5. ]; 
quality[011:020] := [ 0.7, 0.7, 0. , 1. , 0. , 0.2, 0. , 0.7, 1. , 1. ]; 
quality[021:030] := [ 0. , 1. , 0.9, 0. , 0.5, 0. , 0.7, 0.1, 1. , 7. ]; 
quality[031:040] := [ 1. , 1. , 1. , 0. , 0.5, 0.3, 0.5, 0. , 0. , 1. ]; 
quality[041:050] := [ 0.5, 1. , 1. , 0.3, 0. , 0. , 0.3, 0.9, 0.2, 1. ]; 
quality[051:060] := [ 1. , 0.3, 0. , 0.5, 0. , 0. , 0.1, 0.7, 0.7, 0. ]; 
quality[061:070] := [ 0. , 0. , 0.7, 0.2, 0.4, 0.5, 0.5, 0.2, 0.7, 0. ]; 
quality[071:080] := [ 0.9, 0. , 0.7, 0.3, 0. ,-0.1,-1. , 0.5, 0.3, 0.9]; 
quality[081:090] := [-1. , 0.3, 0. , 0. , 0.1, 0.4, 0.8, 0.8, 0.9, 0.5]; 
quality[091:100] := [ 0.5, 0.4, 0.3, 0.5, 0.7, 0.1, 0. , 1. , 0.5, 0.5]; 
# ------------------- 1    2    3    4    5    6    7    8    9   10
quality[101:110] := [ 0.6, 0.6, 0.2, 0. , 0.4,-0.1, 0.6, 0.2,-0.1,-1. ]; 
quality[111:120] := [ 1. , 0.2, 0. , 0.1, 0.3, 0.5, 0. , 0. , 0.1, 0.1]; 
quality[121:130] := [ 1. , 0.3, 0. , 0. , 0. , 0.5, 0.1, 0.3, 0. , 0. ]; 
quality[131:140] := [ 0.2, 0.5, 0.9, 0.2, 0. , 0.1, 0.5, 0.1, 0. , 0.7]; 
quality[141:150] := [ 0. , 0. , 0.3, 0.1, 0.1, 0.9,-1. , 1. , 0. , 0.1]; 
quality[151:160] := [ 1. , 0.7, 1. , 7. ,-1. , 0. , 0. , 0. , 0.5, 0.5]; 
quality[161:170] := [ 0. , 0.1, 0.5, 0.4, 0.7, 0.5, 0.3, 0.4, 0. , 0.5]; 
quality[171:180] := [ 0. , 1. , 1. , 0. , 0.6, 0. , 0.5, 0.3, 0.3, 1. ]; 
quality[181:190] := [ 1. , 0.4, 0.6, 0.5, 0.5, 0. , 0. , 0.6, 0.5, 0. ]; 
quality[191:200] := [ 0. , 0.6, 0.6, 0.7, 0.5, 1. , 0. , 1. , 0.4, 0.4]; 
# ------------------- 1    2    3    4    5    6    7    8    9   10
quality[201:210] := [ 0.3,-0.5, 0.1, 0.7, 0.3, 1. , 1. , 0. , 0.3, 0. ]; 
quality[211:220] := [ 0.6, 0. , 0.7, 0.4, 0.4, 1. , 0. , 1. , 0. ,-1.9]; 
quality[221:230] := [ 0.3, 0.4, 0.1, 0. , 0.5, 0.1, 0.1, 0.3, 0.1, 0.4]; 
quality[231:240] := [ 0. , 0.7, 0.7, 1. , 0.3, 0. , 0. , 0.3, 0.6, 0.5]; 
quality[241:250] := [ 1. , 1. , 0.4, 0. , 0.6, 0. , 0. , 0.3, 0. , 0. ]; 
quality[251:260] := [ 0.2, 0. , 0.9, 0.5, 1. , 0. , 0.5, 0. , 0.5, 1. ]; 
quality[261:270] := [ 0.5, 0.6, 0.6, 0.4, 1. , 0.6, 1. , 0. , 0.1, 0.6]; 
quality[271:280] := [ 0. , 0. , 0.1, 0.4, 0. , 0. , 0. , 0. , 0. , 0.5]; 
quality[281:290] := [ 0.5, 0.7, 0.7, 0. , 0.8, 0. , 0. , 0.4, 0.9, 0. ]; 
quality[291:300] := [ 0.7, 0.5, 0.5, 0.6, 0.7, 0.7, 0. , 0.6, 0. , 0. ]; 
# ------------------- 1    2    3    4    5    6    7    8    9   10
quality[301:310] := [ 0.5, 1. , 1. , 0. , 0.6, 0. , 0.4, 0.3, 0.9, 0. ]; 
quality[311:320] := [ 0.8, 0. , 0. , 0. , 0. , 0.5, 0.6, 0.2, 0.6, 0.5]; 
quality[321:330] := [ 0.6, 1. , 0.4, 0.7, 0.1, 0.3, 0.7, 0.1, 0.4, 0.5]; 
quality[331:340] := [ 0.7, 0.1, 0.2, 1. , 1. , 0. , 0.4, 0.5, 0. , 0. ]; 
quality[341:350] := [ 1. , 0.2, 0.3, 0. , 0.8, 0. , 0.3, 0.8, 0.3, 0.8]; 
quality[351:360] := [ 0.6, 0. , 0.1, 0.3, 0.4, 0. , 0.6, 0. , 0.5, 0.8]; 
quality[361:370] := [ 0.3, 0.8, 0. , 0. , 0. , 0. , 0. , 1. , 0.6, 0.4]; 
quality[371:380] := [ 0.4, 1. , 0. , 0.4, 0.7, 0.5, 0.4, 0. , 0. , 0.7]; 
quality[381:390] := [ 0.5, 0. ,-0.5, 0.3, 0.8, 0.3, 0.2, 0.7, 1. , 0. ]; 
quality[391:400] := [ 1. , 0.6, 0.6, 0.5, 0.5, 0.4, 0. , 0.3, 0.4, 0. ]; 
# ------------------- 1    2    3    4    5    6    7    8    9   10
quality[401:410] := [ 0. , 0.4, 0.4, 0. , 0. , 0.3, 0.8,-1. , 0.7, 0. ]; 
quality[411:420] := [ 0.6, 0. , 0. , 0.6, 0.5, 0. , 0. , 0.6, 0.6, 0. ]; 
quality[421:430] := [ 0.4, 0.4, 0.7, 0. , 1. , 0.4, 0.8, 0.4, 0.4, 0.4]; 
quality[431:440] := [ 1. , 0.6, 0.4, 0.4, 0.8, 0.6, 0. , 0.5, 0.2, 0.2]; 
quality[441:450] := [ 0.3, 0.3, 0. , 0.4, 0.6, 0. , 0. , 0. , 0.2, 0.7]; 
quality[451:460] := [ 0. , 0. , 0.4, 0.6, 0.4, 0.6, 0. , 0. , 0.3, 1. ]; 
quality[461:470] := [ 0.5, 0. , 0.1, 0.7, 0.8, 0. , 0.2, 0.2, 0.4, 0.4]; 
quality[471:480] := [ 0.5, 0.2, 0. , 0.3, 0.5, 0.3, 0.3, 0. , 0. , 0.1]; 
quality[481:490] := [ 0.6, 0.7, 0.6, 0. , 0.1,-1. , 0.6, 0. , 0.4, 0. ]; 
quality[491:500] := [ 0.8, 0. , 0. , 0. , 0. , 0. , 0. , 0.9, 1. , 0. ]; 
# ------------------- 1    2    3    4    5    6    7    8    9   10
quality[501:510] := [ 0.5, 0. , 0. , 0.1, 0.4, 0. , 0.8, 0.6, 0.4, 0. ]; 
quality[511:520] := [ 0.3, 0.4, 0.9, 0.9, 0.4, 0.8, 0. , 0. , 0. , 0.6]; 
quality[521:530] := [ 0. , 0.3, 0.6, 0.4, 0.4, 0.3, 0.6, 0. , 0.3, 0.4]; 
quality[531:540] := [ 0.4, 0. , 0.4, 1. , 0.1, 1. , 0. , 0. , 1. , 0. ]; 
quality[541:550] := [ 0.3, 0. , 0. , 0.6, 0. , 0. , 0.8, 0. , 0.3, 0.4]; 
quality[551:560] := [ 0.4, 0.3, 0.6, 0. , 1. , 0. , 0.4, 0. , 0. , 0. ]; 
quality[561:570] := [ 0. , 0.9, 0.2, 0.9, 0.3, 0. , 0.3, 0. , 0.4, 0.8]; 
quality[571:580] := [ 0. , 1. , 0.1,-1. , 0.3, 0.1,-1. ,-1. ,-1. ,-1. ]; 
quality[581:590] := [-1. ,-1. ,-1. ,-1. , 0.6, 0.4, 1. , 0.3, 0. , 0. ]; 
quality[591:600] := [ 0.6, 0.7, 0.8, 0.2, 0. , 0.5, 0. , 0.3, 0. , 0.5]; 
# ------------------- 1    2    3    4    5    6    7    8    9   10
quality[601:610] := [ 0.5, 0.4, 0.1, 0.8, 0.3, 0.1,-1. , 1. , 0. , 0.9]; 
quality[611:620] := [ 0.4, 0.5, 0.6, 0.4, 0.9, 0.3, 0. , 0. , 0.4, 0.7]; 
quality[621:630] := [ 0. , 0.5, 0. , 0.8, 1. ,-1. , 0.7, 0. , 0.1, 0.4]; 
quality[631:640] := [ 0. , 0.4, 0.3, 0. , 0. , 0.8, 0. , 0.6, 0.5, 0. ]; 
quality[641:650] := [ 0. , 0.8, 0.3, 0. , 0. , 0. , 1. , 0.4, 0.5, 0.6]; 
quality[651:660] := [ 0.8, 0. , 0. , 0. , 0.3, 0.3, 0.4, 0.9, 0.8, 0. ]; 
quality[661:664] := [ 0. , 0. , 0.6, 0.7];
# ------------------- 1    2    3    4    5    6    7    8    9   10

