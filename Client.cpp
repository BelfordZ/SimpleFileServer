#include "Client.h"

Client::Client(int pid) {
   clientPID = pid;
   framesRemaining = 0;
   lineBuffer = "";
   previousLineLength = 0;
}

void Client::connect(const std::string& fileName) {

   //
   // Create sockets to connect to the Server on the dispatch ports
   //
   
   sktOut = new Socket();
   sktOut->create();
   if(!sktOut->connect(SERVER_NAME, CLIENT_DISPATCH_PORT)) {
      std::cout << "ERROR: Could not connect to output dispatch port " << CLIENT_DISPATCH_PORT << std::endl;
      exit(-1);
   }
  
   sktIn = new Socket();
   sktIn->create();

   if(!sktIn->connect(SERVER_NAME, SERVER_DISPATCH_PORT)) {
      std::cout << "ERROR: Could not connect to input dispatch port " << SERVER_DISPATCH_PORT << std::endl;
      exit(-1);
   }

   std::string outFileName = "client_" + to_string<__pid_t>(getpid()) + "_" + fileName;
   outFile = new std::ofstream(outFileName.c_str());
   
   //
   // Send CLIENT_HELLO on dispatch port
   //
   
   Frame clientHello(CLIENT_HELLO, fileName, to_string<__pid_t>(getpid()));
   std::cout << "Sending CLIENT_HELLO... ";
   send_frame(clientHello);
   std::cout << "Done!" << std::endl;

   //
   // Process the SERVER_HELLO to determine the transfer port to use
   //
   
   std::cout << "Waiting for SERVER_HELLO... ";
   Frame serverHello = receive_frame();
   int serverPortIn = std::atoi(serverHello.get_data().c_str()); // Get the transfer input port
   int serverPortOut = serverPortIn + 1;
   std::cout << "Done!" << std::endl;
   std::cout << "Handshake complete, dispatched to port " << serverPortIn << std::endl;
   sleep(1);

   //
   // Delete dispatch sockets, we don't need them anymore
   //
   
   delete sktIn;
   delete sktOut;

   //
   // Create new sockets in order to connect to the transfer ports
   //
   
   sktIn = new Socket();
   sktIn->create();

   if(!sktIn->connect(SERVER_NAME, serverPortIn)) {
      std::cout << "ERROR: Could not connect to input port " << serverPortIn << " specified by server"
		<< std::endl;
      exit(-1);
   }

   sktOut = new Socket();
   sktOut->create();
   if(!sktOut->connect(SERVER_NAME, serverPortOut)) {
      std::cout << "ERROR: Could not connect to output port " << serverPortOut << " specified by server"
		<< std::endl;
      exit(-1);
   }

   //
   // Assuming everything went correctly, we should be connected to the server!
   // Send ACK and start transferring our file!
   //
   
   Frame ackFrame(ACK, "FILE PLZ");
   send_frame(ackFrame);
   std::cout << "--------------------[ BEGIN TRANSMISSION ]--------------------" << std::endl;
	
}

void Client::disconnect() {
   std::cout << "Goodbye!" << std::endl;
   Frame goodbye(GOODBYE, "BAI SERVER");
   send_frame(goodbye);
}

void Client::send_frame(const Frame& f) {
   sktOut->send(f.get_frame_as_string());
}

Frame Client::receive_frame() {
   std::string receivedString = "";

   // Wait for the server to send us data: if the socket is empty, we need to wait
   sktIn->recv(receivedString);
   Frame receivedFrame(receivedString); // Create a frame from our raw data    
   return receivedFrame;
}

void Client::listen() {
   // Need to know if it's our first SYNC frame, because we don't want to
   // output an empty buffer to our file...
   bool isFirstSYNC = true;

   // Transfer Stats
   int totalFrames = 0;
   int badFrames = 0;

   // Main listening loop
   while(true) {
      Frame f = receive_frame();
      totalFrames++;
      
      if(f.is_good()) {

	 switch(f.get_frame_type()) {
	    case SYNC:
	    {
	       // Display line, append to file, and erase line buffer
	       if(!isFirstSYNC) {
		  std::cout << lineBuffer << std::endl;
		  lineBuffer = lineBuffer.substr(0, previousLineLength);
		  lineBuffer.append("\n");
		  *outFile << lineBuffer;
		  lineBuffer = "";
	       }
	       
	       isFirstSYNC = false;

	       // ACK our good SYNC frame, next will be DATA frames
	       Frame ackFrame(ACK, "MOAR FRAMEZ PLZ");
	       send_frame(ackFrame);
	       previousLineLength = atoi(f.get_data().c_str());
	       break;
	    }
	    case DATA:
	    {
	       // Data frame, so just append it to the current line buffer
	       lineBuffer.append(f.get_data());

	       Frame ackFrame(ACK, "KAYPE");
	       send_frame(ackFrame);
	       break;
	    }
	    case GOODBYE:
	    {
	       transfer_stats(totalFrames, badFrames);
	       return; // We're done here
	    }
	    default:
	    {
	       // WE SHOULD NEVER GET HERE!!!
	       std::cout << "Invalid frame type received!" << std::endl;
	       return;
	    }
	 }
	 
      } else {

	 if(f.get_frame_type() == GOODBYE) {
	    transfer_stats(totalFrames, badFrames);
	    return; // We can't ACK GOODBYE frames... duh.
	 }

	 badFrames++;
	 
	 // Send NAK and wait for the server to retransmit the frame
	 // Next one is guaranteed to be okay, since we just send a NAK
	 std::cout << "\t\t[Bad frame! Requesting retransmission... Type: " << f.get_frame_type() << "]"
		   << std::endl;
	 Frame nakFrame(NAK, "LOLWUT SEND AGAIN");
	 send_frame(nakFrame);
      }
   }
}

void Client::transfer_stats(int totalFrames, int badFrames) const {
   std::cout << "--------------------[  END TRANSMISSION  ]--------------------" << std::endl
	     << " Transfer statistics: " << std::endl
	     << "\tTotal Frames Received:\t\t" << totalFrames << std::endl
	     << "\tGood Frames Received:\t\t" << totalFrames-badFrames << std::endl
	     << "\tDamaged Frames Received:\t" << badFrames << std::endl
	     << "\tGood-Damaged Frame Ratio:\t" << (totalFrames-badFrames)/badFrames << ":1" << std::endl;
}


void Client::write_file() {
   outFile->close();
}
