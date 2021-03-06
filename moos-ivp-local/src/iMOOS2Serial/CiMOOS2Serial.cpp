// $Header: /home/cvs/repository/project-plusnet/src/iMOOS2Serial/CiMOOS2Serial.cpp,v 1.2 2006/08/22 23:55:06 cvs Exp $
// (c) 2005

// CiMOOS2Serial.cpp: implementation of the CiMOOS2Serial class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiMOOS2Serial.h"
#include "dtime.h"
#include <signal.h>

bool sigflag = false;

void sigh(int)
{
	sigflag = true;
}

CiMOOS2Serial::CiMOOS2Serial()
{
	// constructor
	signal(SIGINT, sigh);
	signal(SIGTERM, sigh);
}

CiMOOS2Serial::~CiMOOS2Serial()
{
  // destructor
}

bool CiMOOS2Serial::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
     	string pt; int bd;
	
	m_MissionReader.GetConfigurationParam("Port", pt);
	m_MissionReader.GetConfigurationParam("Speed", bd);

        bbb = new CSerMOOS(pt, bd);
	bbb->SetCB(tramp, this);

	//m_MissionReader.GetConfigurationParam("InputVariable", invar);

	//m_Comms.Register(invar, 0);
	m_Comms.Register("DESIRED_THRUST", 0);
	m_Comms.Register("DESIRED_RUDDER", 0);

	return true;
}

bool CiMOOS2Serial::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  

  for(p = NewMail.begin(); p != NewMail.end(); p++) {
  
      CMOOSMsg &msg = *p;

    // This is roughly the RECON Transmit function. 
    // It should be broken out into its onw call eventually.
    int rc, length = 0, i, chksum = 0, maxlen;
    char buf[MAXBUFLEN];
    for (int i = 0; i < MAXBUFLEN; i++) {
      buf[i] = '\0';
    }

    printf("New Mail (getkey): %s\n", msg.GetKey().c_str());
//no	   printf("New Mail (m_skey.cstr): %s\n", msg.m_sKey().c_str());
//no	   printf("New Mail (m_skey): %s\n", msg.m_sKey());
    printf("New Mail (getdouble): %f\n", msg.GetDouble());
	   printf("New Mail (m_dfVal): %f\n", msg.m_dfVal);
    
    if (strcmp(msg.GetKey().c_str(), "DESIRED_THRUST") == 0) {
      length = sprintf(buf,"#C,Thrust,%f,*",msg.GetDouble());
    }
    else if (strcmp(msg.GetKey().c_str(), "DESIRED_RUDDER") == 0) {
      length = sprintf(buf,"#C,Rudder,%f,*",msg.GetDouble());
    }
    // Check for buffer overflow
    maxlen = MAXBUFLEN - 4;

    if ((length == -1) ||  (length >= maxlen))
      {
	// Not enough space left                                                                   
	return false;
      }

    // Make sure there's a recognized message before we send.
    if (length > 0) {
      // Do checksum                                                                               
      for (i = 0; i < length; i++)
	{
	  chksum += buf[i];
	}
      
      // Finish message                                                                            
      rc = snprintf(&(buf[length]), 4, "%02X\n", 0xff & chksum);
      if (rc != 3)
	{
	  MOOSTrace("WARNING - Appending checksum failed\n");
	  return false;
	}
      length += rc;
      
      printf("Writing to serial: %s",buf);
      bbb->AppendWriteQueue(buf,length);
      bbb->NonBlockingWrite();
    }
  }

  NewMail.clear();
  
  return true;
}



bool CiMOOS2Serial::handle(string s)
{
  printf("Incoming message: %s\n", s.c_str());

  // 22Feb2016: Parse out Nav position messages, with expected format from rcProto_v3.8
  // #N,T12345,LAT12.34567,LON12.34567,NAVE12.34,NAVN12.34,V1.23,H123.4,HDOP1.23*99

  // !!!
  // Still need to verify checksum.
  // !!!

  string msgType = s.substr(0,2);
  string msg = s.substr(3); //Eliminates "#N," 
  msg = msg.substr(0, msg.length() - 3);   // Now strip the cheksum

  int token;
  string pair;
  string param;
  string value;

  int nparam = 0;

  if (strcmp(msgType.c_str(),"#M") == 0)
    {
      printf("  MODE message: %s\n", msg.c_str());	
      nparam = 2;
    }
  else if (strcmp(msgType.c_str(),"#N") == 0) 
    {
      printf("  NAV message: %s\n", msg.c_str());	
      nparam = 7;
      
    }
  else
    {      // Unknown Message type.
      printf("  Unknown type.\n");
      return false;
    }
  for (int n = 0; n < nparam; n++) {
    // Loop through the string (assuming its all there...) 
    // and pull out the parameter/value pairs.
    token = msg.find(",");
    pair = msg.substr(0, token);
    msg = msg.substr(token + 1);
    
    param = pair.substr(0,3);
    value = pair.substr(3);
    
    printf("param: %s\n",param.c_str());
    printf("value: %s\n",value.c_str());
    if (strcmp(param.c_str(),"TMS") == 0) {
      // TIME, in millisec
      printf("param: %s\n",param.c_str());
      printf("value: %s\n",value.c_str());
      m_Comms.Notify("AVR_CLOCK", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"MOD") == 0) {
      m_Comms.Notify("NAV_MODE", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"LAT") == 0) {
      m_Comms.Notify("NAV_LAT", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"LON") == 0) {
      m_Comms.Notify("NAV_LON", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"NAE") == 0) {
      m_Comms.Notify("NAV_X", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"NAN") == 0) {
      m_Comms.Notify("NAV_Y", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"VEL") == 0) {
      m_Comms.Notify("NAV_SPEED", atof(value.c_str()));
    }
    if (strcmp(param.c_str(),"HDG") == 0) {
      m_Comms.Notify("NAV_HEADING", atof(value.c_str()));
    }
  } 
  return true;
}



bool CiMOOS2Serial::Iterate()
{
	// happens AppTick times per second
	/* commented Apr.29,18
	if(sigflag == true) { // time to exit
	  	//	delete port;
		exit(1);
	}
	*/
	return true;
}

bool CiMOOS2Serial::OnStartUp()
{
	// happens after connection is completely usabled 
	return true;
}

