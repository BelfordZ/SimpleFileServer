#include "FrameSet.h"

FrameSet::FrameSet(const std::string& line, bool ravageFrames) {
   //make SYNC frame
   Frame syncFrm(SYNC, to_string(line.length()) );
   frames.push_back(syncFrm);
	
   // create vector of frames
   bool goodFrame;
   for (int i=0; i< line.length(); i+=62){
      //create a data frame class for data
      Frame* f = NULL;

      int badFrame = rand() % 5;

      if((badFrame == 3 || badFrame == 4) && ravageFrames) {
	 goodFrame = false;
	 std::cout << "Flipping bit!" << std::endl;
      } else {
	 goodFrame = true;
      }
      
      if(line.length() < 62) {
	 f = new Frame(DATA, line, goodFrame);
      } else {
	 f = new Frame (DATA, line.substr(i, 62), goodFrame);
      }	

      frames.push_back(*f);
      //int x;std::cin>>x;

   }

   
}

int FrameSet::get_number_of_frames(){ return frames.size();}

std::string FrameSet::to_string(int toStr)
{
   std::ostringstream convert;
   convert << toStr;
   return convert.str();
}

void FrameSet::ravage_frameset()
{ 
   int index,damagedFrameNumber;
   float er = 0.2;

   // Determining How many frames am i going to damage?
   damagedFrameNumber = floor(get_number_of_frames() * er);
   std::cout << "Number of frames to ravage: " << damagedFrameNumber << std::endl;

   // generate a random number between 0-MaxNumOfFrames in the vector array
   index = rand() % (get_number_of_frames()-1);	// this number is used as an idex.

   for (int i=0 ; i<damagedFrameNumber; i++)
   {
      frames[index].flip_parity();
      // generate next index
      std::cout << "Ravaged index: " << index << std::endl;
      index = rand() % (get_number_of_frames()-1);	
   }
}
