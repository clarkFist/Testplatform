title mib
cd ..
set PATH=.\classes;%PATH%
.\jre\bin\java -mx200M -Dswing.defaultlaf=com.sun.java.swing.plaf.windows.WindowsLookAndFeel -Dswing.metalTheme=steel -classpath ".\conf;.\MibBrowser;.\jars\ApiUtils.jar;.\jars\AdventNetSnmp.jar;.\jars\AdventNetLogging.jar;.\jars\AdventNetCCLXMLParser.jar;.\jars\AdventNetUtils.jar;.\jars\AdventNetCCLUtils.jar;.\jars\AdventNetSnmpUtilitiesSplash.jar;.\classes;.\jars\MibBrowser.jar;.;.\jars\JimiProClasses.zip;.\jars\AdventNetNPrevalent.jar;.\jars\AdventNetUI.jar;.\jars\Mail.jar;.\jars\activation.jar" MibBrowserApplication  -h 20.2.1.10 -p 161 -v v1 -c URBALIS -m ".\mibs\ICMTC" -t 5 -r 0
#".\mibs\ICMTC"