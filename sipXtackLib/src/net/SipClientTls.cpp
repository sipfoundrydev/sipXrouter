//
// Copyright (C) 2007 Pingtel Corp., certain elements licensed under a Contributor Agreement.  
// Contributors retain copyright to elements licensed under a Contributor Agreement.
// Licensed to the User under the LGPL license.
// 
//
// $$
////////////////////////////////////////////////////////////////////////
//////


// SYSTEM INCLUDES
#include <stdio.h>

// APPLICATION INCLUDES
#include <net/SipMessage.h>
#include <net/SipClientTls.h>
#include <net/SipMessageEvent.h>
#include <net/SipUserAgentBase.h>

#include <os/OsDateTime.h>
#include <os/OsDatagramSocket.h>
#include <os/OsStatus.h>
#include <os/OsSysLog.h>
#include <os/OsEvent.h>

#include <utl/XmlContent.h>

#define SIP_DEFAULT_RTT 500

#define LOG_TIME

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS

// All requests must contain at least 72 characters:
/*
X Y SIP/2.0 \n\r
i: A\n\r
f: B\n\r
t: C\n\r
c: 1\n\r
v: SIP/2.0/UDP D\n\r
l: 0 \n\r
\n\r

*/
// However to be tolerant of malformed messages we allow smaller:
#define MINIMUM_SIP_MESSAGE_SIZE 30
#define MAX_UDP_PACKET_SIZE (1024 * 64)

// STATIC VARIABLE INITIALIZATIONS

const UtlContainableType SipClientTls::TYPE = "SipClientTls";
const int sDefaultPort = SIP_TLS_PORT;

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Constructor
SipClientTls::SipClientTls(OsSocket* socket,
                           SipProtocolServerBase* pSipServer,
                           SipUserAgentBase* sipUA) :
   SipClient(socket, pSipServer, sipUA, "SipClientTls-%d")
{
}

/* ============================ MANIPULATORS ============================== */

// Send a message.  Executed by the thread.
void SipClientTls::sendMessage(const SipMessage& message,
                               const char* address,
                               int port)
{
   UtlBoolean sendOk = FALSE;

   if (!clientSocket->isOk())
   {
      clientSocket->reconnect();
#ifdef TEST_SOCKET
      if (clientSocket)
      {
         OsSysLog::add(FAC_SIP, PRI_DEBUG,
                       "SipClientTcp[%s]::send reconnected with socket descriptor %d",
                       mName.data(),
                       clientSocket->getSocketDescriptor());
      }
#endif
   }
   if (clientSocket->isOk())
   {
#ifdef LOG_TIME
      OsTimeLog eventTimes;
      eventTimes.addEvent("writing");
#endif
      // *** beware of blocking
      sendOk = message.write(clientSocket);
#ifdef LOG_TIME
      eventTimes.addEvent("released");
      UtlString timeString;
      eventTimes.getLogString(timeString);
      OsSysLog::add(FAC_SIP, PRI_DEBUG, "SipClientTcp[%s]::send time log: %s",
                    this->mName.data(),
                    timeString.data());
#endif

      if (sendOk)
      {
         touch();
      }
   }

   // *** if !sendOK, synthesize 408 message.

   return;
}

/* ============================ ACCESSORS ================================= */

/* ============================ INQUIRY =================================== */

// Return the default port for the protocol of this SipClientTls.
int SipClientTls::defaultPort(void) const
{
   return sDefaultPort;
}

/* //////////////////////////// PROTECTED ///////////////////////////////// */

/* //////////////////////////// PRIVATE /////////////////////////////////// */

/* ============================ FUNCTIONS ================================= */
