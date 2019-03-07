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
#include <fstream>
#include <iterator> 
#include <map>

#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "Protocol.h"

using namespace std;


namespace sedp {
  class Server {
  private:
    int socket_id;
    State protocol_state = State::INITIAL;
    unsigned int player_id;
    unsigned int port_number;
    unsigned int max_clients;
    map<int, int> clients;

  public:
    ifstream inpf;

    Server(unsigned int id, unsigned int port, unsigned int max_clients);

    ~Server();
    State get_state();
    void start();
    void run_protocol(); 
    void accept_clients();
    void send_int_to(unsigned int client_id, unsigned int x);
    int receive_int_from(unsigned int client_id);
    void send_msg(int socket, uint8_t *msg, int len);
    void receive_msg(int socket, uint8_t *msg, int len);
  };
}

#endif
