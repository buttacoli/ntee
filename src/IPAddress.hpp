#ifndef INCLUDED_IPADDRESS_HPP
#define INCLUDED_IPADDRESS_HPP

#include "Address.hpp"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class IPAddress : public Address {
public:

   IPAddress(const char* host, const char* port);
   IPAddress(const char* host, int port );
   ~IPAddress();
   
   sockaddr* getAddr();
   int getLen();
   int getPort();
   
   IPAddress* clone() const;
   
private:

   void compute();
   void freemem();
   
   addrinfo* pAI_;
   std::string hostname_;
   std::string port_;
};

#endif
