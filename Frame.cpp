#include "Frame.h"

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


Frame::Frame(const FrameType& frameType, const std::string& payload, bool goodFrame) {
   fType = frameType;
   data = pad_data(payload, FRAME_DATA_LENGTH);
   set_parity(data);
   if(!goodFrame) {
      parity = !parity;
   }
}

Frame::Frame(const FrameType& frameType, const std::string& payload, const std::string& pid) {
   fType = frameType;
   data = pad_data(payload, FRAME_DATA_PID_LENGTH);
   procId = pad_data(pid, FRAME_PID_LENGTH);
   set_parity(data);
}

Frame::Frame(const std::string& rawData) {
   fType = char_to_frametype(rawData[0]);
   char pChar = rawData[1];
   if(pChar == '0') {
      parity = 0;
   } else {
      parity = 1;
   }

   // If we have a CLIENT_HELLO or a SERVER_HELLO, then we need to
   // associate a PID with our Frame (for use in the Server).
   if(fType == CLIENT_HELLO || fType == SERVER_HELLO) {
      procId = rawData.substr(2, FRAME_PID_LENGTH);
      data = rawData.substr(7, FRAME_DATA_PID_LENGTH);
   } else {
      data = rawData.substr(2, FRAME_DATA_LENGTH);
   }
}

std::string Frame::pad_data(const std::string& rawData, int size) {
   std::string appendStr = "\0";
   std::string returnStr = rawData;
   
   for(int i = rawData.length(); i < size-1; i++) {
      appendStr += " ";
   }
   
   returnStr += appendStr;
   return returnStr;
}

bool Frame::check_parity() const {
   int checksum = 0;
   for(int i = 0; i < data.length(); i++) {
      checksum = (checksum + data[i]) % 2;
   }

   return checksum;
}

void Frame::set_parity(std::string pStr) {
   int checksum = 0;
   for(int i = 0; i < pStr.length(); i++) {
      checksum = (checksum + pStr[i]) % 2;
   }

   parity = checksum;
}

std::string Frame::get_frame_as_string() const {
   std::string frameStr = frametype_to_string(fType);
   frameStr += to_string<bool>(parity);
   
   if(fType == CLIENT_HELLO || fType == SERVER_HELLO) {
      frameStr += procId;
   }
   
   frameStr += data;
   return frameStr;
}

FrameType Frame::char_to_frametype(char frameTypeChar) const {
   switch(frameTypeChar) {
      case '0':
	 return CLIENT_HELLO;
      case '1':
	 return SERVER_HELLO;
      case '2':
	 return SYNC;
      case '3':
	 return GOODBYE;
      case '4':
	 return DATA;
      case '5':
	 return ACK;
      case '6':
	 return NAK;
      default:
	 return NAK;
   }

   return NAK; // this should never happen!!!
}

std::string Frame::frametype_to_string(const FrameType& f) const {
   switch(f) {
      case CLIENT_HELLO:
	 return "0";
      case SERVER_HELLO:
	 return "1";
      case SYNC:
	 return "2";
      case GOODBYE:
	 return "3";
      case DATA:
	 return "4";
      case ACK:
	 return "5";
      case NAK:
	 return "6";
      default:
	 return "4";
   }

   return "4"; // this should never happen!!!
}

