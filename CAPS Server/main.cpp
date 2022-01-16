#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include "TCPServer.h"
#include "TCPClient.h"


#define DEFAULT_PORT 12345

using namespace std;

bool terminateServer = false;

void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data, map<string, vector<string>>* messageboard);

int main()
{
	TCPServer server(DEFAULT_PORT);

	ReceivedSocketData receivedData;

	vector<thread> serverThreads;

	cout << "Starting server. Send \"exit\" (without quotes) to terminate." << endl;

	//map where key value is string and value is a vector of strings
	map<string, vector<string>>* messageboard = new map<string, vector<string>>();

	while (!terminateServer)
	{
		receivedData = server.accept();

		if (!terminateServer)
		{
			serverThreads.emplace_back(serverThreadFunction, &server, receivedData, messageboard);
		}
	}

	

	for (auto& th : serverThreads)
		th.join();

	cout << "Server terminated." << endl;

	return 0;
}

void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data, map<string, vector<string>>* messageboard)
{
	unsigned int socketIndex = (unsigned int)data.ClientSocket;

	

	do {
		server->receiveData(data, 0);

		if (data.request != "" && data.request != "exit" && data.request != "EXIT")
		{
			cout << "[" << socketIndex << "] Bytes received: " << data.request.size() << endl;
			cout << "[" << socketIndex << "] Data received: " << data.request << endl;

			data.reply = data.request;
			reverse(data.reply.begin(), data.reply.end());

			server->sendReply(data);
		}
		else if (data.request == "exit" || data.request == "EXIT")
		{
			cout << "[" << socketIndex << "] Data received: " << data.request << endl;
			cout << "[" << socketIndex << "] Exiting... Bye bye!" << endl;

			data.reply = data.request;
			server->sendReply(data);
		}
	} while (data.request != "exit" && data.request != "EXIT" && !terminateServer);

	if (!terminateServer && (data.request == "exit" || data.request == "EXIT"))
	{
		terminateServer = true;
		server->sendReply(data);
		TCPClient tempClient(string("127.0.0.1"), DEFAULT_PORT);
		tempClient.OpenConnection();
		tempClient.CloseConnection();
	}

	server->closeClientSocket(data);
}

