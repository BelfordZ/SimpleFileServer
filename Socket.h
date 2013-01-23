// Definition of the Socket class

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "Common.h"
#include "SocketException.h"

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
//const int MAXRECV = 500;
// I believe we need to use this as our Max length to get from socket.
const int MAXRECV = (sizeof(char)*64);

void output_status(std::string s);
void output_done();

class Socket
{
  public:
   Socket();
   Socket(int);
   virtual ~Socket();
   
   // Server initialization
   bool create();
   bool bind (const int port);
   bool listen() const;
   bool accept (Socket&) const;
   
   // Client initialization
   bool connect (const std::string host, const int port);
   
   // Data Transimission
   bool send (const std::string) const;
   int recv (std::string&) const;
   
   bool is_valid() const {
      return m_sock != -1; }
  private:
   int m_sock;
   sockaddr_in m_addr;
};
#endif
