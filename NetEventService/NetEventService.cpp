#include "NetEventService.h"

#include "NetEventServer.h"
#include "NetEventClient.h"


NETEVENTSERVICE_API NetEvtServer* CreateNetEvtServer()
{
	NetEventServer* pNetEvServer = new NetEventServer();
	return pNetEvServer;
}

NETEVENTSERVICE_API NetEvtClient* CreateNetEvtClient()
{
	NetEventClient* pNetEvClient = new NetEventClient();
	return pNetEvClient;
}


