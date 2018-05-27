/************************************************************/
/*    NAME: Min Dai                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE: 03/03/2018                                               */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"
#include <cmath>
using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  //ADDED
 m_first_reading = 0;
 m_current_x = 0;
 m_current_y = 0;
 m_previous_x = 0;
 m_previous_y = 0;
 m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    double dval  = msg.GetDouble();
    if (key == "NAV_X"){
    m_previous_x = m_current_x;
    m_current_x = dval;
    }
    if (key == "NAV_Y"){
    m_previous_y = m_current_y;
    m_current_y = dval;
    }
#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);

   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{
  m_total_distance = m_total_distance + sqrt(pow((m_current_x-m_previous_x),2)+pow((m_current_y-m_previous_y),2));

  Notify("ODOMETRY_DIST",m_total_distance);
  //sqrt((m_current_x-m_previous_x)^2+(m_current_y-m_previous_y)^2);
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      if(param == "FOO") {
        //handled
      }
      else if(param == "BAR") {
        //handled
      }
    }
  }

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry::RegisterVariables()
{
  // Register("FOOBAR", 0);
  //ADDED
  Register("NAV_X",0);
  Register("NAV_Y",0);
  Register("ODOMETRY_DIST",0.0);
}
