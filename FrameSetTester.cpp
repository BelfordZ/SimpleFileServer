#include <iostream>
#include <string>

#include "FrameSet.h"

int main() {
   std::string line = "Hello, this is a test of the FrameSet class and this is a really long line to make sure that we can still read from it like the best programmers that we are ever, so we are now going to try and parse a really super long line so that we can see whether or not the FrameSet gets damaged (like it should), because if it doesn't, someone is going to have to fix this fucking thing, and the person who does that is probably going to be me because I have no fucking life which is really quite sad";
   FrameSet f(line);
   FrameSet g(line, false);
   for(int i = 0; i < f.frames.size(); i++) {
      std::cout << " Bad: " << f.frames[i].get_frame_as_string() << std::endl;
      std::cout << "Good: " << g.frames[i].get_frame_as_string() << std::endl;
   }

   
   return 0;
}
