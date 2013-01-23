all: ClientApplication ServerApplication clean

test: FrameTester FrameSetTester
	echo -n	"TESTING FRAME CLASS"
	./FrameTester
	echo -n "TESTING FRAMESET CLASS"
	./FrameSetTester

ClientApplication: ClientApplication.o Client.o Frame.o FrameSet.o Socket.o
	g++ $^ -o ClientApplication

ServerApplication: ServerApplication.o Server.o Frame.o FrameSet.o Socket.o
	g++ $^ -o ServerApplication -lpthread

ServerApplication.o: ServerApplication.cpp
	g++ -c $^ -lpthread -Wall

ClientApplication.o: ClientApplication.cpp
	g++ -c $^ -Wall

Socket.o: Socket.cpp
	g++ -c $^ -Wall

Client.o: Client.cpp
	g++ -c $^

Server.o: Server.cpp
	g++ -c $^ -lpthread

Frame.o : Frame.cpp
	g++ -c $^

FrameSet.o: FrameSet.cpp
	g++ -c $^

FrameTester: FrameTester.o Frame.o
	g++  $^ -o FrameTester

FrameTester.o: FrameTester.cpp
	g++ -c $^

FrameSetTester: FrameSetTester.o FrameSet.o Frame.o
	g++ $^ -o FrameSetTester

FrameSetTester.o: FrameSetTester.cpp
	g++ -c $^

ClientTester: ClientTester.o Client.o Frame.o FrameSet.o
	g++  $^ -o ClientTester

ClientTester.o: ClientTester.cpp
	g++ -c $^

clean:
	rm -f *~
	rm -f *.o

clean-all: clean
	rm -f ClientTester 
	rm -f FrameTester
	rm -f FrameSetTester
	rm -f ClientApplication
	rm -f ServerApplication
	rm -f client_*_*
	rm -f \#*
# Makefile for the socket programming tutorial 
#
#server = ServerSocket.o Socket.o server.o ServerSocket.h Socket.h SocketException.h
#client = ClientSocket.o Socket.o client.o ServerSocket.h Socket.h SocketException.h


#all : server client

#server: $(server)
#	g++ -o server $(server)

#client: $(client)
#	g++ -o client $(client)


#Socket: Socket.cpp 
#ServerSocket: ServerSocket.cpp 
#ClientSocket: ClientSocket.cpp
#server: server.cpp
#client: client.cpp

#clean:
#	rm -f *.o simple_server simple_client
