#ifndef __FRAME_H__
#define __FRAME_H__

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Common.h"

// Frame type identifiers
typedef enum { CLIENT_HELLO, SERVER_HELLO, SYNC, GOODBYE, DATA, ACK, NAK } FrameType;


/*
  yo bro, frames of type CLIENT_HELLO or SERVER_HELLO need to also contain a process ID field.
  the max length of a processID number is 5. It will be casted to a string
  before you have to deal with it, so dont worry about any typecasting
  things. The thrid implementation of Frame should deal with this scenario

  a frame looks like:
  constructer 1: [frame_type(1 char) | check bit(1 char) | data(62)]
  constructer 2: [frame_type(1 char) | check bit(1 char) | procId(5 chars) | data(57)]
  constructer 3: [frame_type(1 char) | check bit(1 char) | data(62)]

  Server Sends client a frameType of GOODBYE when the line passed to the
  server contains nothing but EOF
*/

class Frame {
  public:
   Frame(const FrameType& frameType, const std::string& data, bool goodFrame = true); //SYNC, DATA, GOODBYE , ACK & NAK 
   Frame(const FrameType& frameType, const std::string& data, const std::string& pid); //sever hello, client hello
   Frame(const std::string& rawData);
   
   // Accessors
   bool get_parity() const { return parity; } //## added
   bool check_parity() const;
   bool is_good() const { return (parity == check_parity()); }
   std::string get_data() const { return data; }
   std::string get_procId() const { return procId; }
   FrameType get_frame_type() const { return fType; }
   std::string get_frame_type_as_string() const { return frametype_to_string(fType); }
   std::string get_frame_as_string() const;
   std::string pad_data(const std::string&, int);
   void set_parity(std::string pStr); //## added
   void flip_parity() { parity = !parity; }
  private:
   bool parity;
   FrameType fType;
   std::string procId;
   std::string data; // the data of the fram without the control bits 
	
   FrameType char_to_frametype(char) const;
   std::string frametype_to_string(const FrameType& f) const;
};

#endif
