#include <iostream>
#include "Frame.h"

int main() {
   Frame t(SYNC, "64");
   Frame s("2064");
   std::cout << "T str: " << t.get_frame_as_string() << std::endl
	     << "S str: " << s.get_frame_as_string() << std::endl;
   Frame nak(NAK, "NAK Frame");
   std::cout << nak.get_frame_as_string() << std::endl
	     << "Ftype is: " << nak.get_frame_type_as_string() << std::endl
	     << "is NAK? " << (NAK == nak.get_frame_type()) << std::endl;
   Frame newNak("61NAK Frame");
   std::cout << newNak.get_frame_as_string() << std::endl
	     << "Ftype is: " << newNak.get_frame_type_as_string() << std::endl
	     << "is NAK? " << (NAK == newNak.get_frame_type()) << std::endl;

   return 0;
}
