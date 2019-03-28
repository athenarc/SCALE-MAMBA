#ifndef _ProtocolEntity
#define _ProtocolEntity

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <mutex>

#include "Math/bigint.h" 
#include "Math/gfp.h"
#include "Math/gf2n.h"

namespace sedp {
  class ProtocolEntity {
  private:
    mutex cmtx;
    char delimiter = ','; // TODO: reconsider 
  public:
    ProtocolEntity() {};

    void send_msg(int socket, const void *msg, int len);
    void receive_msg(int socket, uint8_t *msg, int len);
    void send_to(int socket, const string &o);
    void receive_from(int socket, string &o); 
    void send_int_to(int socket, unsigned int x);
    int receive_int_from(int socket);
    void safe_print(const string& s);
    void pack(const vector<gfp>& v, string& s);
    void unpack(const string& s, vector<gfp>& v);
    gfp str_to_gfp(const string& s);
    string gfp_to_str(const gfp& y);
  };
}

#endif
