#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <fstream>
#include <iostream>
#include <string>

#include "Common.h"
#include "Frame.h"
#include "FrameSet.h"
#include "Socket.h"

class Client {
  public:
   Client(int);
   void connect(const std::string&);
   void disconnect();
   void send_frame(const Frame&);
   Frame receive_frame();
   void request_file(const std::string&);
   void write_file();
   void listen();
   void transfer_stats(int, int) const;
  private:
   int framesRemaining;
   int previousLineLength;
   int clientPID;
   std::string lineBuffer;
   std::ofstream* outFile;
   Socket* sktIn;
   Socket* sktOut;
};

#endif
