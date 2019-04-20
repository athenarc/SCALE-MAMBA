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

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace sedp {
  class ProtocolEntity {
  private:
    mutex cmtx;
    char delimiter = ','; // TODO: reconsider 
  public:
    ProtocolEntity() {};

    void send_msg(SSL *ssl, const void *msg, int len);
    void receive_msg(SSL *ssl, uint8_t *msg, int len);
    void send_to(SSL *ssl, const string &o);
    void receive_from(SSL *ssl, string &o); 
    void send_int_to(SSL *, unsigned int x);
    int receive_int_from(SSL *);
    void safe_print(const string& s);
    void pack(const vector<gfp>& v, string& s);
    void unpack(const string& s, vector<gfp>& v);
    gfp str_to_gfp(const string& s);
    string gfp_to_str(const gfp& y);
  };
}

#endif
