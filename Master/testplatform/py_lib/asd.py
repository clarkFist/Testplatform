from xml.dom.minidom import parse
import xml.dom.minidom
cfg = parse('testcase/cfg1/CSZ_MVCU1344-FSFB2-0722.xml')
collection = cfg.documentElement
PHY_STRUCT = collection.getElementsByTagName("PHY_STRUCT")
RACK = PHY_STRUCT[0].getElementsByTagName('RACK')
LRU_1_2 = RACK[0].getElementsByTagName('LRU')[0]
CELL = LRU_1_2.getElementsByTagName('CELL')
CELL_PARAM = CELL[0].getElementsByTagName('CELL_PARAM')
CELL_COM = CELL_PARAM[0].getElementsByTagName('CELL_COM')
IP = CELL_COM[0].getElementsByTagName('IP')
NORMAL_IP = IP[0].getElementsByTagName("NORMAL_IP")
REDUNDANT_IP = IP[0].getElementsByTagName('REDUNDANT_IP')
USIG =  CELL_COM[0].getElementsByTagName('USIG')
SESSION_IDENT = USIG[0].getElementsByTagName('SESSION_IDENT')
nIP_dict ={}
rIP_dict ={}
sIP_dict ={}
for item in NORMAL_IP:
    t =int(item._attrs['IP_TYPE'].nodeValue)
    nIP_dict[t] = item._attrs['IP_ADDRESS'].nodeValue
    print(nIP_dict[t])
for item in REDUNDANT_IP:
    t =int(item._attrs['IP_TYPE'].nodeValue)
    rIP_dict[t] = item._attrs['IP_ADDRESS'].nodeValue
    print(rIP_dict[t])
for item in SESSION_IDENT:
    t =int(item._attrs['SESSION_ID'].nodeValue)
    if item._attrs['OPERATION'].nodeValue == 'AUTONOMOUS':
        REMOTE_IP = item.getElementsByTagName('REMOTE_IP')[0]
        sIP_dict[t] = (REMOTE_IP._attrs['REM_IP_N'].nodeValue,REMOTE_IP._attrs['REM_IP_R'].nodeValue)
    elif item._attrs['OPERATION'].nodeValue == 'TRIGGERED':
        print("TRIGGERED")
    else:
        raise Exception
    print(sIP_dict.get(t,None))
