`-- File Name : ICMTC
-- Date      : Thu Mar 22 11:18:33 CST 2012
-- Author    : WebNMS Agent Toolkit C Edition - MibEditor 6
-- *****************************************************************
-- ICMTC-MIB:  MIB-II MIB file
-- 22 Mar 2012, Lin Yi - ICMTC TEAM
-- CASCO reserved
-- *****************************************************************
--
 "Description"                                                          "This is the name of the CASCO product (i.e. hardware+software package) without version or index indication. The string is intended for presentation to a human"                       "This is the description of the CASCO product (i.e. the main function) in one sentence. The string is intended for presentation to a human"                           C"The number of independents software present inside the equipment."                       n"This is a list of software entries. The number of entries 
              is given by the value of SwNumber."                       "description of a software"                       x"A unique value for each software of the equipment. 
               Its value ranges between 1 and the value SwNumber."                       u"This is a text string containing information about the software. The string is intended for presentation to a human"                       "This is a text string containing information about the location of the software inside the equipment. The string is intended for presentation to a human"                       |"This is a text string containing information about the software editor. The string is intended for presentation to a human"                       "This is a text string containing information about the version software. 
              The string is intended for presentation to a human"                       ~"This is a text string containing information about the checksum software. The string is intended for presentation to a human"                           J"This is the number of LRU of the subsystem present inside the equipment."                       j"This is a list of Lru entries. The number of 
              entries is given by the value of LruNumber."                       1"A LRU entry is containing multiple information."                       {"This is a unique value for each LRU of the subsystem. Its value ranges between 1 and the value lruNumber.
              "                       "This is a text string containing information about the LRU of the subsystem. The string is intended for presentation to a human"                        "Indicate the state of the LRU."                      "This field makes it possible to particularize the problem for the LRU at fault. 
               The code of defect will be used by the team of maintenance 
               in particular for the checking of the disappearance of the 
               defect after intervention."                      "Indicate only if the LRU is not available, in this case this field indicates the date of appearance of the problem. 
               The time fields are coded in hour GMT (Greenwich Mean Time) on 64 bits:
               The first 32 bits represent the number of seconds passed since midnight (0 hour, or 0h) on January 1, 1900.   
               The 32 following bits store the one second fraction."                      ["The data present in the field context are identical for the LRU, 
				OtherEquipment and RMonGauge. With each detection of problem 
				(State passes from available to unavailable) the data of the context 
				are photographed and places in this field. The field context allows 
				an immediate invertigation of the problem by operator MSS."                       «"The data shall represent the table name from the system database where this LRU is described.
				If this LRU is not described in the dabase, lruRefTable shall be empty"                       ^"This is the number of hardware of all the LRU of the subsystem present inside the equipment."                       "This is a list of Hardware entries of the specified LRU. 
              The number of entries is given by the value of LruNumberOfHw."                       	"en test"                       ~"This is a unique value for each hardware of the LRU. 
              Its value ranges between 1 and the value LruNumberOfHw."                       ­"This is a unique value for each LRU of the subsystem. Its value ranges between 1 and the value lruNumber.
               This index indicates what hardware refers to lru."                       "This is a text string containing information of the hardware component, with type and name. The string is intended for presentation to a human"                       "This is a text string containing version information of the hardware component.
              The string is intended for presentation to a human"                       "This is a  text string containing serial number of the hardware component. 
              The string is intended for presentation to a human"                           l"This is the number of passive equipment or no-computer 
              equipments connect at the subsystem"                       i"This is a list of Eqt entries. The number of entries 
              is given by the value of EqtNumber"                       <"A external equipment entry containing multiple information"                       k"This is a unique value for equipment. 
              Its value ranges between 1 and the value EqtNumber."                       Υ"This is a text string containing information about the type of the equipment. The string is intended for presentation to a human. The type of equipment must be able to present the reference of the OtherEquipment"                      »"A text string must define this OtherEquipment in the subsystem who describe this MIB. The string is intended for presentation to a human
For each OtherEquipment entry, if the OtherEquipment is described in the database system (like a Point from CBI subsystem) the eqtDescr shall represent the objects name from the table where the OtherEquipment is described.
If the OtherEquipment is not described in the dabase, eqtRefTable shall be empty"                       &"Indicate the state of the equipment."                      "This field makes it possible to particularize the problem for the LRU at fault. 
               The code of defect will be used by the team of maintenance 
               in particular for the checking of the disappearance of the 
               defect after intervention."                      §"Indicate only if the equipment is not available, in this case this field indicates the date of appearance of the problem. 
              The time fields are coded in hour GMT (Greenwich Mean Time) on 64 bits:
              The first 32 bits represent the number of seconds passed since midnight (0 hour, or 0h) on January 1, 1900.   
              The 32 following bits store the one second fraction.  
              "                      ["The data present in the field context are identical for the LRU, 
				OtherEquipment and RMonGauge. With each detection of problem 
				(State passes from available to unavailable) the data of the context 
				are photographed and places in this field. The field context allows 
				an immediate invertigation of the problem by operator MSS."                       Α"The data shall represent the table name from the system database where this OtherEquipment is described.
				If this OtherEquipment is not described in the dabase, lruRefTable shall be empty"                           ?"This is the number of remote monitoring data in the subsystem"                       l"This is a list of remote monitoring data entries. The number of entries is given by the value of RmgNumber"                       K"This is a remote monitoring gauge data entry contain multiple information"                       "This is a unique value for each remote monitoring gauge data of the subsystem. 
              Its value ranges between 1 and the value RmgNumber. "                       "Type of Gauge managing "                       a"This is  the total description of the gauge. The string is intended for presentation to a human"                      R"This value must make it possible to know the reference of the equipment that applies this threshold.
              Additional requirements:
              0 if the equipment is the equipment supporting the MIB
              EqtIndex else . EqtIndex is an other equipment who is 
              described in the sub tree OtherEquipment"                       \"This is the unit of coding of the data. The string is intended for presentation to a human"                       "Value of the threshold before sending a alarm. 
              REMARK : The threshold can be configured by the SNMP manager (MSS)"                       @"This is the current value of the remote monitoring gauge data."                      F"When Trap is send (Threshold is reached)
               The time fields are coded in hour GMT (Greenwich Mean Time) on 64 bits:
               The first 32 bits represent the number of seconds passed since midnight (0 hour, or 0h) on January 1, 1900.   
               The 32 following bits store the one second fraction."                      ["The data present in the field context are identical for the LRU, 
				OtherEquipment and RMonGauge. With each detection of problem 
				(State passes from available to unavailable) the data of the context 
				are photographed and places in this field. The field context allows 
				an immediate invertigation of the problem by operator MSS."                           H"This is the number of remote monitoring counter data in the subsystem."                       N"This is a remote monitoring counter data entry contain multiple information."                       N"This is a remote monitoring counter data entry contain multiple information."                       "This is a unique value for each remote monitoring counter data 
              of the subsystem. Its value ranges between 1 and the value RmcNumber."                       g"This is the total description of the counter data. The string is intended for presentation to a human"                      N"This value must make it possible to know the reference of the equipment that applies this counter.
              Additional requirements:
              0 if the equipment is the equipment supporting the MIB
              EqtIndex else . EqtIndex is other equipment who is described 
              in the sub tree OtherEquipment "                       d"This is the unit of coding of the counter data. The string is intended for presentation to a human"                      R"This is the current value of the remote monitoring counter data. 
              REMARK : The counter can be positioned by the SNMP manager (MSS), 
              generally when the counter reached a threshold inside MSS, MSS create a alarm. 
              When the alarm will acknowledged by an operator,  MSS will reset the RmcValue."                           z"Unique Id in the system of the receiving Sub-system 
              (get from RedundancyHeader), the current sub-system."                       8"(get from RedundancyHeader) of the current sub-system."                       >"Up to MaxConnectedSSy; (Nb of connected Sender Sub-systems)."                       "If the MaxRsLinkNumber has been reached and that a new Sub-system 
              has to be linked with (either as sender or receiver)."                       "nb of messages rejected due to data inconsistency in the redundancy 
              header fields (length, version, Transmitter/ReceiverID)."                       2"Sequence of RsLkEntry repeat RsLinkNumber times."                       
"nothing."                       "This is a unique value for each link entry of redundancy layer for the subsystem.
              Its value ranges between 1 and the value RsLinkNumber."                       B"Unique Id of the sending Sub-system (get from RedundancyHeader)."                      M"In NTP format to record the communication start
               The time fields are coded in hour GMT (Greenwich Mean Time) on 64 bits:
               The first 32 bits represent the number of seconds passed since midnight (0 hour, or 0h) on January 1, 1900.   
               The 32 following bits store the one second fraction."                       A"Nb of msgs rejected due to SN already in the ReceivingSN_Table."                       S"Nb of valid messages sent to the Application layer (after redundancy processing)."                           S"the counter is incremented each time a LRU changes its state (OK->NOK or OK->NOK)"                      f"It is relative to the last increment of stateLRUCounter 
               The time fields are coded in hour GMT (Greenwich Mean Time) on 64 bits:
               The first 32 bits represent the number of seconds passed since midnight (0 hour, or 0h) on January 1, 1900.   
               The 32 following bits store the one second fraction.
              "                       _"the counter is incremented each time an OtherEquipment changes its state (OK->NOK or OK->NOK)"                      `"It is relative to the last increment of stateTimeStampOtherEquipment 
              The time fields are coded in hour GMT (Greenwich Mean Time) on 64 bits:
              The first 32 bits represent the number of seconds passed since midnight (0 hour, or 0h) on January 1, 1900.   
              The 32 following bits store the one second fraction."                      ­"This is the description of the result of the self-test of starting. The string is intended for presentation to a human.
              Additional requirements:
              The string must begin by a number on four characters, followed by a space. 
              This number must consist of four characters between Γ½0Γ½ and Γ½9Γ½.
              This number must be unique in each subsystem for this string.
              "                       J"The number of external service needed for the subsystem.
              "                       "structure of service."                       5"The serviceΓ½ state contains multiples information."                       "A unique value for each external service needed for the subsystem. Its value ranges between 1 and the value StateServiceNumber"                       |"The total description of the external service needed for the subsystem. The string is intended for presentation to a human"                       $"The current state of this service."                           D"Red SIG address where the MSS is reachable to receive TRAP message"                       E"Blue SIG address where the MSS is reachable to receive TRAP message"                          