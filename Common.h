// Common header file with shared global variables
// between the client and server implementation.
#ifndef __COMMON_H__
#define __COMMON_H__

#include <sstream>

//use these as client ports.
//   client 1 uses 3000 & 3001
//   client 2 uses 3002 & 3003
//   etc...
const int CLIENT_PORT_MIN = 30000;
const int CLIENT_PORT_MAX = 30009;


// Client sends request via CLIENT_DISPATCH_PORT
// Server sends request via SERVER_DISPATCH_PORT
const int SERVER_DISPATCH_PORT = 40000;
const int CLIENT_DISPATCH_PORT = 40001;

// characters in a frame
const int FRAME_SIZE = 64; 

// maximum number of data chars we can send/receive in a frame
const int FRAME_DATA_LENGTH = 62;
const int FRAME_DATA_PID_LENGTH = 57;
const int FRAME_PID_LENGTH = 5;

// how many chars we reserve for the frame header (frame type and parity)
const int FRAME_HEADER_LENGTH = 2; 

// servername
const std::string SERVER_NAME = "localhost";

// convert to to a string

// Converting an int, for example, you would do:
//      std::string str = to_string<int>(314);

// Converting a double: std::string d = to_string<double>(3.14159);

// This allows you to convert from any type to a string 
// (at least any type that supports the << operator to a stringstream)
template <class T>
inline std::string to_string(const T& t)
{
   std::stringstream ss;
   ss << t;
   return ss.str();
}

#endif
