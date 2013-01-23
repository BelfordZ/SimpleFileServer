#ifndef __SERVER_H__
#define __SERVER_H__

#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "Common.h"
#include "Frame.h"
#include "FrameSet.h"
#include "Socket.h"

// Server uses sockets 3000-3009 for client comms, and uses
// 
struct initData
{
   std::string filename;
   int port;
};

class Server
{
  public:
   Server();
   ~Server();
   void begin_server();
  
  private: 
   Socket *dispatchIn;
   Socket *dispatchOut;
   Socket *client;
   Socket *server;
   std::string currFrame;
   
};
#endif
