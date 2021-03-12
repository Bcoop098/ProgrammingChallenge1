/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net-RakNet-Server.cpp
	Source for RakNet server management.
*/

#include "gpro-net/gpro-net-server/gpro-net-RakNet-Server.hpp"
#include <map>
#include <iostream>
namespace gproNet
{
	std::map<std::string, std::string> nameToServer;
	std::map<std::string, bool> userToIsInServer;

	cRakNetServer::cRakNetServer()
	{
		RakNet::SocketDescriptor sd(SET_GPRO_SERVER_PORT, 0);
		unsigned short MAX_CLIENTS = 10;

		peer->Startup(MAX_CLIENTS, &sd, 1);
		peer->SetMaximumIncomingConnections(MAX_CLIENTS);
	}

	cRakNetServer::~cRakNetServer()
	{
		peer->Shutdown(0);
	}

	bool cRakNetServer::ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID)
	{
		if (cRakNetManager::ProcessMessage(bitstream, sender, dtSendToReceive, msgID))
			return true;

		// server-specific messages
		switch (msgID)
		{
		case ID_NEW_INCOMING_CONNECTION:
			//printf("A connection is incoming.\n");
			return true;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			//printf("The server is full.\n");
			return true;
		case ID_DISCONNECTION_NOTIFICATION:
			//printf("A client has disconnected.\n");
			return true;
		case ID_CONNECTION_LOST:
			//printf("A client lost the connection.\n");
			return true;

			// test message
		case ID_GPRO_MESSAGE_COMMON_BEGIN:
		{
			// server receives greeting, print it and send one back
			RakNet::BitStream bitstream_w;
			//in here, we get a username and add that to a map of users
			//userToIsInServer[username] = false; // the user is not in a game server, but the main one
			ReadTest(bitstream);
			WriteTest(bitstream_w, "Hello client from server");
			peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
		}	return true;
		case ID_JOINSERVER:
		{
			//check to see if the user is in the main server, if they are let them join a new server, and set the bool to true. Requires a name to be passed in
			/*
			* std::map<std::string, server>::iterator it;
			* it = nameToServer.find(serverName.ToString());
			* if(!userToIsInServer[username] && it != nameToServer.end())
			* {
			*	let the user join the server
			*	userToIsInServer[username] = true;
			*   send connection information to that server
			*	write the server name, and send that to that server
			*	peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false); need to set the server being joined
			* }
			*/
		}
		return true;
		case ID_LEAVESERVER:
		{
			//check to see if the user is not in the main server, if they are let them leave the server, and set the bool to false.
			/*
			* std::map<std::string, server>::iterator it;
			* it = nameToServer.find(serverName.ToString());
			* if(userToIsInServer[username] && it != nameToServer.end())
			* {
			*	let the user join the server
			*	userToIsInServer[username] = false;
			*   send disconnect infomation to that server
			*   write the server name, and send that to that server
			*   peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
			* }
			*/
		}
		return true;
		case ID_SERVERLIST:
		{
			RakNet::BitStream bitstream_w;
			//iterate through the map with all the server names.
			std::map<std::string, std::string>::iterator it;
			for (it = nameToServer.begin(); it != nameToServer.end(); it++)
			{
				bitstream_w.Write(it->first); //I think it needs to be c string
				bitstream_w.Write(" ");
			}
			peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
		}
		return true;
		}
		return false;
	}
}