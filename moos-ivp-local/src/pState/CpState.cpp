//
// CpState.cpp: implementation of the CpState class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CpState.h"
#include "dtime.h"
#include <signal.h>

using namespace std;

CpState::CpState()
{
  // constructor
  // NAV state variables
  navHeading = 0.0;
  navSpeed = 0.0;
  navX = 0.0;
  navY = 0.0;
  
  // RAW incoming vars
  imuHeading = 0.0;
  imuSpeed = 0.0;
  gpsLat = 0.0;
  gpsLon = 0.0;
  gpsValid = false;
}

CpState::~CpState()
{
  // destructor
}

bool CpState::OnConnectToServer()
{
	// register for variables here
	// note, you cannot ask the server for anything in this function yet

	m_Comms.Register("IMU_HEADING", 0);
	m_Comms.Register("IMU_SPEED", 0);
	m_Comms.Register("GPS_LATITUDE_DEG", 0);
	m_Comms.Register("GPS_LONGITUDE_DEG", 0);

	return true;
}

bool CpState::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    if (strcmp(msg.GetKey().c_str(), "IMU_HEADING") == 0) {
      imuHeading = msg.GetDouble(); 
    }
    else if (strcmp(msg.GetKey().c_str(), "IMU_SPEED") == 0) {
      imuSpeed = msg.GetDouble();
    }
    else if (strcmp(msg.GetKey().c_str(), "GPS_LATITUDE_DEG") == 0) {
      if (msg.GetDouble() > 0) {
	gpsLat = msg.GetDouble();
	printf("Good GPS");
	gpsValid = true;
      } else {
	gpsLat = 0.0;
	printf("Bad GPS");
	gpsValid = false;
      }
    }
    else if (strcmp(msg.GetKey().c_str(), "GPS_LONGITUDE_DEG") == 0) {
      if (gpsValid) {
	gpsLon = msg.GetDouble();
      } else {
	gpsLon = 0.0;
      }
    }
  }
  NewMail.clear();
  
  return true;
}



bool CpState::Iterate()
{
	// happens AppTick times per second
  printf("Lat/Long: %f N, %f E\n",gpsLat, gpsLon);

	
	// DO THE MATH and then notify
	//        m_Comms.Notify("NAV_SPEED", atof(value.c_str()));
	//        m_Comms.Notify("NAV_HEADING", atof(value.c_str()));
	//        m_Comms.Notify("NAV_X", atof(value.c_str()));
	//        m_Comms.Notify("NAV_Y", atof(value.c_str()));
	
	return true;
}

bool CpState::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

