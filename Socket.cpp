#include "Socket.h"

void output_status(std::string s)
{
   std::cout << s << std::endl;;
}
void output_done()
{
   std::cout << "  -------->Done!" << std::endl;
}

Socket::Socket() : m_sock ( -1 )
{
   std::memset(&m_addr, 0, sizeof(m_addr));
}

Socket::Socket(const int port) : m_sock ( -1 )
{
   if (!create())
   {
      throw SocketException("Error creating server socket!");
   }
   if (!bind(port))
   {
      throw SocketException("Error binding to port!");
   }
   if (!listen())
   {
      throw SocketException("Error listening to socket!");
   }
}

Socket::~Socket()
{
   if (is_valid())
      ::close(m_sock);
}

bool Socket::create()
{
   try
   {
      m_sock = socket(AF_INET, SOCK_STREAM, 0);
   } catch(SocketException &e)
   {
      std::cout << "ERROR:Socket.cpp:create(): " << e.description() << std::endl;
   }  
   if (!is_valid())
   {
      std::cout << "ERROR:Socket.cpp:create(): !is_valid() returned false" << std::endl;
      return false;
   }
   
   // TIME_WAIT - argh
   int on = 1;
   if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof (on)) == -1)
   {
      std::cout << "ERROR:Socket.cpp:create(): !setsockopt returned -1" << std::endl;
      return false;
   }
   return true;
}

bool Socket::bind (const int port)
{
   if (!is_valid()){
      return false;
   }

   m_addr.sin_family = AF_INET;
   m_addr.sin_addr.s_addr = INADDR_ANY;
   m_addr.sin_port = htons (port);

   int bind_return = ::bind (m_sock,
			     (struct sockaddr *)&m_addr,
			     sizeof(m_addr));
   if (bind_return == -1){
      return false;
   }
   
   return true;
}


bool Socket::listen() const
{
   if (!is_valid()){
      return false;
   }

   int listen_return = ::listen (m_sock, MAXCONNECTIONS);

   if (listen_return == -1){
      return false;
   }
   
   return true;
}

bool Socket::accept (Socket& new_socket) const
{

   int addr_length = sizeof (m_addr);
   new_socket.m_sock = ::accept (m_sock, (sockaddr *)&m_addr, (socklen_t *)&addr_length);

   if (new_socket.m_sock <= 0)
      return false;
   else
      return true;
}

bool Socket::send(const std::string s) const
{
   //std::cout << "Sending a frame on port " << m_sock << std::endl;
   //std::cout << s << std::endl;
   int status = ::send(m_sock, s.c_str(), s.size(), MSG_NOSIGNAL);
   if (status == -1)
      return false;
   
   return true;
}

int Socket::recv(std::string& s) const
{
   //std::cout << "Receiving a frame on port " << m_sock << std::endl;
   char buf[MAXRECV + 1];
   s = "";

   memset(buf, 0, MAXRECV + 1);

   int status = ::recv(m_sock, buf, MAXRECV, 0);
   
   if (status == -1){
      std::cout << buf << std::endl;
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
   }
   else if (status == 0){
      return 0;
   }
   else{
      s = buf;
      return status;
   }
}

bool Socket::connect (const std::string host, const int port)
{
   if (!is_valid())
   {
      std::cout << "ERROR: Socket.cpp::connect: is_valid() returned false;" << std::endl;
      return false;
   }

   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons(port);

   int status = inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);

   if (errno == EAFNOSUPPORT)
   {
      std::cout << "ERROR: Socket.cpp::connect: " << errno << " ;" << std::endl;
      return false;
   }

   status = ::connect(m_sock, (sockaddr *)&m_addr, sizeof (m_addr));

   if (status == 0)
   {
      return true;
   }
   else
   {									\
      std::cout << "ERROR: Socket.cpp::connect: " << errno << " ;" << std::endl;
      return false;
   }
}
