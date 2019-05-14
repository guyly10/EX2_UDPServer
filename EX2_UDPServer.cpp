#include <iostream>
#include <dirent.h> 
using namespace std;
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <ctime>



int main(int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Error: no port provided\n");
		exit(-1);

	}

	int TIME_PORT = atoi(argv[1]);
	int msg_counter = 1;
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return(-1);
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
												// IP Port. The htons (host to network - short) function converts an
												// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)& serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return(-1);
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255] = "";
	char recvBuff[255];
	char command[255];

	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests. at PORT: " << TIME_PORT << " \n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return(-1);
		}

		recvBuff[bytesRecv] = '\0';
		memcpy(command, recvBuff, strlen(recvBuff));

		if (strcmp(recvBuff, "GETAll") == 0) {

			DIR* dir;
			struct dirent* ent;
			if ((dir = opendir("./Files")) != NULL) {
				/* print all the files and directories within directory */
				while ((ent = readdir(dir)) != NULL) {
					printf("%s\n", ent->d_name);
					strcpy_s(sendBuff, ent->d_name);
					sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)& client_addr, client_addr_len);
					cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
					cout << "Time Server: Wait for NEW clients' requests.\n";
				}
				closedir(dir);
			}
			else {
				/* could not open directory */
				perror("");
				return EXIT_FAILURE;
			}			
		}



		//recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		//cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		//// Answer client's request by the current time.

		//// Get the current time.
		//time_t timer;
		//time(&timer);
		//// Parse the current time to printable string.
		////strcpy(sendBuff, ctime(&timer));
		//char integer_string[32];
		//sprintf_s(integer_string, "%dd", msg_counter);
		//msg_counter++;
		//char s2[100] = "Server message Number: ";
		//strcat_s(s2, integer_string);

		//strcpy_s(sendBuff, _countof(sendBuff), s2);

		//sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
		//// Sends the answer to the client, using the client address gathered
		//// by recvfrom. 
		//bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)& client_addr, client_addr_len);
		//if (SOCKET_ERROR == bytesSent)
		//{
		//	cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
		//	closesocket(m_socket);
		//	WSACleanup();
		//	return(-1);
		//}

		//cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
		//cout << "Time Server: Wait for NEW clients' requests.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}
