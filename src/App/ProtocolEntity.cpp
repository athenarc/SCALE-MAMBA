#include "ProtocolEntity.h"

void sedp::ProtocolEntity::send_msg(int socket, const void *msg, int len) {
  if (send(socket, msg, len, 0) != len)
    {
      throw sending_error();
    }
}

void sedp::ProtocolEntity::receive_msg(int socket, uint8_t *msg, int len) {
  int i = 0, j;
  while (len - i > 0)
    {
      j = recv(socket, msg + i, len - i, 0);
      if (j < 0)
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

void sedp::ProtocolEntity::send_to(int socket, const string &o) {
  uint8_t buff[4];
  encode_length(buff, o.length());
  send_msg(socket, buff, 4);
  send_msg(socket, o.c_str(), o.length());
}


void sedp::ProtocolEntity::receive_from(int socket, string &o) {
  uint8_t buff[4];
  receive_msg(socket, buff, 4);

  int nlen = decode_length(buff);
  uint8_t *sbuff= new uint8_t[nlen];

  receive_msg(socket, sbuff, nlen);

  o.assign((char *) sbuff, nlen);
  delete[] sbuff;
}

void sedp::ProtocolEntity::send_int_to(int socket, unsigned int x)
{
  uint8_t buff[4];
  INT_TO_BYTES(buff, x);
  send_msg(socket, buff, 4);
}


int sedp::ProtocolEntity::receive_int_from(int socket)
{ 
  uint8_t buff[4];
  receive_msg(socket, buff, 4);
  return BYTES_TO_INT(buff);
}
