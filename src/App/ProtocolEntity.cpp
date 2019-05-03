#include "ProtocolEntity.h"

void sedp::ProtocolEntity::send_msg(SSL *ssl, const void *msg, int len) {
  if (SSL_write(ssl, msg, len) != len)
    {
      throw sending_error();
    }
}

void sedp::ProtocolEntity::receive_msg(SSL *ssl, uint8_t *msg, int len) {
  int i = 0, j;
  while (len - i > 0)
    {
      j = SSL_read(ssl, msg + i, len - i);
      if (j <= 0)
        {
          throw receiving_error();
        }
      i = i + j;
    }

  if (len - i != 0)
  {
    throw receiving_error();
  }
}

void sedp::ProtocolEntity::send_to(SSL * ssl, const string &o) {
  uint8_t buff[4];
  encode_length(buff, o.length());
  send_msg(ssl, buff, 4);
  send_msg(ssl, o.c_str(), o.length());
}


void sedp::ProtocolEntity::receive_from(SSL * ssl, string &o) {
  uint8_t buff[4];
  receive_msg(ssl, buff, 4);

  int nlen = decode_length(buff);
  uint8_t *sbuff= new uint8_t[nlen];

  receive_msg(ssl, sbuff, nlen);

  o.assign((char *) sbuff, nlen);
  delete[] sbuff;
}

void sedp::ProtocolEntity::send_int_to(SSL *ssl, unsigned int x)
{
  uint8_t buff[4];
  INT_TO_BYTES(buff, x);
  send_msg(ssl, buff, 4);
}


int sedp::ProtocolEntity::receive_int_from(SSL * ssl)
{
  uint8_t buff[4];
  receive_msg(ssl, buff, 4);
  return BYTES_TO_INT(buff);
}

void sedp::ProtocolEntity::safe_print(const string& s) {
  lock_guard<mutex> g{cmtx};
  cout << s << endl;
}

void sedp::ProtocolEntity::pack(const vector<gfp>& v, string& s) {
  ostringstream ss;
  string del(1, delimiter);

  for (auto &g: v){
    ss << g;
    ss << del;
  }

  s = ss.str();
}

void sedp::ProtocolEntity::unpack(const string& s, vector<gfp>& v){
  string token;
  istringstream tokenStream(s);

  while (getline(tokenStream, token, delimiter))
  {
    gfp x;
    stringstream ss;
    ss << token;
    ss >> x;
    v.push_back(x);
  }
}

gfp sedp::ProtocolEntity::str_to_gfp(const string& s) {
  stringstream ss(s);
  gfp y;
  ss >> y;

  return y;
}

string sedp::ProtocolEntity::gfp_to_str(const gfp& y) {
  ostringstream ss;
  ss << y;
  return ss.str();
}
