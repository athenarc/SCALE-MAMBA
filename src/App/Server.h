#ifndef _Server
#define _Server

#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "Protocol.h"

using namespace std;

class Server {
private:
  int socket_id;
  State protocol_state = State::INITIAL;
  unsigned int player_id;
  unsigned int port_number;
  unsigned int max_clients;
  vector<int> clients;

public:
  Server(unsigned int id, unsigned int port, unsigned int max_clients);

  ~Server();
  
  void accept_clients();
  void send_int_to(unsigned int client_id, unsigned int x);
  int receive_int_from(unsigned int client_id);
  void broadcast_int(unsigned int x);
  void send_msg(int socket, uint8_t *msg, int len);
  void receive_msg(int socket, uint8_t *msg, int len);

};

#endif
