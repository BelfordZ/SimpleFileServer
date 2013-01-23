#ifndef __FRAMESET_H__
#define __FRAMESET_H__

#include <vector>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <math.h>

#include "Frame.h"

class FrameSet {
  public:
   FrameSet(const std::string&, bool ravageFrames = true);

   // returns the number of frames total (including sync frame)
   int get_number_of_frames();

   //Im tired of work arounds because this fucker is private.
   //Im making it public because I need to check frame types,
   //resend frames... etc.
   std::vector<Frame> frames;
   
   // Because every one in 5 frames will be "bad", we need to get an array of
   // all the frame parity chars, and then flip every 1/5 of them. Do this
   // in a "statistical" way, don't just flip every 5th char.
   void ravage_frameset(); // this function should implement above description
  private:
   //void last_frame_requested() { return lastFrameRequested; }
   //std::vector<Frame> frames;
   //Frame *lastFrameRequested;
	std::string to_string(int);
};
#endif
