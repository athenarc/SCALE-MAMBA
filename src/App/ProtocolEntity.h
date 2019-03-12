#ifndef _ProtocolEntity
#define _ProtocolEntity

#include <cstdint>
#include <sys/socket.h>

#include "Math/bigint.h" 

namespace sedp {
  class ProtocolEntity {
  public:
    ProtocolEntity() {};

    void send_msg(int socket, const void *msg, int len);
    void receive_msg(int socket, uint8_t *msg, int len);
    void send_to(int socket, const string &o);
    void receive_from(int socket, string &o); 
    void send_int_to(int socket, unsigned int x);
    int receive_int_from(int socket); 
  };
}

#endif
