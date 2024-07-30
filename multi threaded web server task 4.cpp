#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void handle_client(SOCKET client_socket);

int main() {
	WSADATA wsaData;
	int iResult;

	SOCKET server_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cerr << "WSAStartup failed with error: " << iResult << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, to_string(PORT).c_str(), &hints, &result);
	if (iResult != 0) {
		cerr << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for the server to listen for client connections
	server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (server_socket == INVALID_SOCKET) {
		cerr << "socket failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Forcefully attach the socket to port 8080
	int reuse_addr = 1;
	iResult = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse_addr, sizeof(reuse_addr));
	if (iResult == SOCKET_ERROR) {
		cerr << "setsockopt failed with error: " << WSAGetLastError() << endl;
		closesocket(server_socket);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Bind the socket to the network address and port
	iResult = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cerr << "bind failed with error: " << WSAGetLastError() << endl;

		// Additional error diagnostics
		switch (WSAGetLastError()) {
		case WSAEACCES:
			cerr << "Error: Permission denied. Try running the program as an administrator." << endl;
			break;
		case WSAEADDRINUSE:
			cerr << "Error: Address already in use. Ensure no other process is using port " << PORT << "." << endl;
			break;
		case WSAEINVAL:
			cerr << "Error: Invalid parameter. Ensure the socket is not already bound to an address." << endl;
			break;
		default:
			cerr << "Unknown error occurred." << endl;
			break;
		}

		freeaddrinfo(result);
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	// Listen for incoming connections
	iResult = listen(server_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		cerr << "listen failed with error: " << WSAGetLastError() << endl;
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	cout << "Server is listening on port " << PORT << endl;

	vector<thread> threads;

	while (true) {
		client_socket = accept(server_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET) {
			cerr << "accept failed with error: " << WSAGetLastError() << endl;
			closesocket(server_socket);
			WSACleanup();
			return 1;
		}

		threads.emplace_back(handle_client, client_socket);
	}

	// Clean up threads
	for (auto &t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}

	closesocket(server_socket);
	WSACleanup();

	return 0;
}

void handle_client(SOCKET client_socket) {
	char buffer[BUFFER_SIZE];
	int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
	if (bytes_received > 0) {
		buffer[bytes_received] = '\0';

		istringstream request(buffer);
		string method, path, version;
		request >> method >> path >> version;

		if (path == "/") {
			path = "/index.html";
		}

		ifstream file("www" + path);
		if (file) {
			ostringstream response;
			response << "HTTP/1.1 200 OK\r\n";
			response << "Content-Type: text/html\r\n";
			response << "\r\n";
			response << file.rdbuf();

			string response_str = response.str();
			send(client_socket, response_str.c_str(), response_str.size(), 0);
		}
		else {
			const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
			send(client_socket, not_found_response, strlen(not_found_response), 0);
		}
	}

	closesocket(client_socket);
}


