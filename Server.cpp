#include "Server.h"

pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER;
initData *listenParams; //= new initData;
void* process_request(void*);
std::queue<int> portQ;
std::queue<pthread_t*> threads;

Server::Server() 
{
   // Build queue with the port numbers. i starts at 30,000, and gets
   // even numbers up to 30,009
   for (int i = CLIENT_PORT_MIN; i < CLIENT_PORT_MAX; i+=2)
      portQ.push(i);

   try
   {
      std::cout << "Waiting for clients..." << std::endl;

      /*
	So, if we look in the S1 directory, you will see that this is
	how the server sockets work. You have to create a new socket
	which is bound to a port, and then you have to create a new
	"virgin socket" so that you can accept connections from the
	"slutty socket". We can then use the "virgin sockets" since
	they have been "sluttified".
      */
      
      client = new Socket(SERVER_DISPATCH_PORT);
      server = new Socket(CLIENT_DISPATCH_PORT);
      dispatchIn = new Socket();
      dispatchOut = new Socket();
   }
   catch (SocketException e)
   {
      std::cout << "ERROR:Server.cpp:Socket(): "
		<< "Failed to init Dispatch ports."
		<< std::endl;
   }
   
   currFrame = "";
}

Server::~Server() 
{
   delete dispatchIn;
   delete dispatchOut;
   
   for (int i = 0; i<threads.size(); i++)
   {
      delete threads.front();
   }
}
// Server constantly checks the dispatch socket. When not null,
// create a frame out of the received string. Check the frame
// type. If the frame type = CLIENT_HELLO, get the filename from
// frame.getData(), aswell as the pid. Create a thread of the
// listen function, passing it the filename requested, along with
// the next port in the sequence. Construct a frame with the pid
// of the client which we are messaging, with the type SERVER_HELLO,
// aswell as the port to which have just spawned listen on. Send to
// the Dispatch socket the frame we just made. Rinse N Repeat.
void Server::begin_server()
{
   while (true)
   {

      //
      // Accept on our dispatch ports so that Clients can connect to the Server
      //
      
      if (!server->accept(*dispatchIn))
      {
	 std::cout << "ERROR:Server.cpp:begin_server(): "
		   << "dispatchIn->accept() returned false;"
		   << std::endl;
      }
      
      std::cout << "Receiving frame from Client..." << std::endl;
      if(dispatchIn->recv(currFrame) == -1)
      {
	 std::cout << "Failed:Server.cpp:begin_server(): "
		   << "dispatchIn.recv returned -1"
		   << std::endl;
	 exit(-1);
      }

      //
      // Process the Client response, once we get one. Put it into a Frame.
      //

      std::cout << "Creating a frame from Client response..." << std::endl;
      Frame *dispatchFrame = new Frame(currFrame);
      switch (dispatchFrame->get_frame_type())
      {
	 output_done();
	 case CLIENT_HELLO:
	 {
	    listenParams = new initData;
	    // Delegate an available transfer port to a Client once we have connected
	    pthread_mutex_lock(&mutexLock);
	    int clientPort = portQ.front();
	    portQ.pop();
	    pthread_t *tmp = new pthread_t;
	    threads.push(tmp);
	    pthread_mutex_unlock(&mutexLock);

	    std::string clientPid = dispatchFrame->get_procId();

	    // We need to do this to remove the spaces at the end of the filename...
	    std::string fName = dispatchFrame->get_data();
	    fName = fName.substr(0, fName.find(" "));
    
	    listenParams->filename = fName;
	    listenParams->port = clientPort;

	    // Formulate a SERVER_HELLO message with the port number that we delegated
	    // and send it to the client so they can connect.
	    delete dispatchFrame;
	    dispatchFrame = new Frame(SERVER_HELLO,
				      to_string<int>(clientPort),
				      clientPid);
	    client->accept(*dispatchOut);
	    dispatchOut->send(dispatchFrame->get_frame_as_string());
 
	    // Create the thread AFTER we have sent SERVER_HELLO. If we create it before,
	    // bad things happen... The Client will wait for us to receive the message. (2 seconds)
	    pthread_create(tmp, NULL, process_request, (void*)listenParams);
	    break;	
	 }	       
	 default:
	 {
	    std::cout << "Failed: Server.cpp::dispatchIn.recv returned -1"
		      << std::endl;
	    exit(-1);
	 }
      }
   }
}

// open the requested file. getline, constructing a FrameSet from
// the results. Get the frameSet size. For each frame in the FrameSet,
// If this is the first iteration, or we have received an ACK, send the
// send frame. However, if our FrameSet is empty, repeat. If the type of any
// of our Frames is EOF, wait for one more ack. Once this ack has been
// received, push the port in use back onto the stack, delete our socket
// reference, close the file and end the thread by calling return.
void* process_request(void* ptr)
{
   initData* message = (initData*) ptr;

   /*
     Ditto what I said above about our sockets basically
     being a bunch of highschoolers...
   */

   std::cout << "Asking client to connect on " << message->port << std::endl;
   Socket newServer = Socket(message->port+1);
   Socket newClient = Socket(message->port);
   Socket sktOut = Socket();
   Socket sktIn = Socket();

   newServer.accept(sktIn);
   newClient.accept(sktOut);

   std::ifstream inFile;
   //std::cout << "Opening " << message->filename << "... ";
   inFile.open(message->filename.c_str());
   //std::cout << "Done!" << std::endl;
   int index = 0;
   std::string myTemp;
   sktIn.recv(myTemp);
   std::cout << "Client has ACK'd our port number response" << std::endl;
   std::string tmp;
   while(!inFile.fail())
   {
      getline(inFile, tmp);
      index += 1;
      
      //std::cout << "Server::Line " << index << " of the frame set:" << tmp << std::endl;

      //
      // Generate two FrameSets from the same line: one with all of the "Good" frames, one with
      // some "Bad" frames. Try to send the ones from the bad set first, and if we get a NAK on
      // one of them, retransmit from the known good FrameSet. Simple as that.
      //
      std::cout << "Sending line " << index << std::endl;
      FrameSet goodLine = FrameSet(tmp, false);
      FrameSet line = FrameSet(tmp);
      int numOfFrames = line.get_number_of_frames();

      //send sync frame
      sktOut.send(line.frames[0].get_frame_as_string());

      std::string syncResponse = "";
      sktIn.recv(syncResponse);
      Frame syncResponseFrame(syncResponse);
      
      if(syncResponseFrame.get_frame_type() == NAK) {
	 sktOut.send(goodLine.frames[0].get_frame_as_string());
	 std::string clientAck = "";
	 sktIn.recv(clientAck); // Discard client ACK for good SYNC frame, start sending data.
      }
      	 
      //for all the rest of the frames
      for (int i = 1; i<numOfFrames; i++)
      {
	 sktOut.send(line.frames[i].get_frame_as_string()); // try to send a frame that may be bad first
	 
	 std::string inFrameString;
	 sktIn.recv(inFrameString);
	 Frame *inFrame = new Frame(inFrameString);
	 
	 switch (inFrame->get_frame_type())
	 {
	    case ACK:
	    {
	       std::cout << "Got ACK, OK!" << std::endl;
	       break;
	    }				 
	    case NAK:
	    {
	       std::cout << "Got NAK, retransmitting!" << std::endl;
	       sktOut.send(goodLine.frames[i].get_frame_as_string()); // send a good frame since we got NAK
	       sktIn.recv(inFrameString); // discard ACK, we know the frame we sent was good
	       break;
	    }			 
	    default:
	       break;
	 }
      }
   }
   Frame goodbyeFrame(GOODBYE, " GOODBYE Client");
   sktOut.send(goodbyeFrame.get_frame_as_string());
}
