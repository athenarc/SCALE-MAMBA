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
#include "ProtocolEntity.h"

using namespace std;


namespace sedp {
  class Server: public ProtocolEntity {
  private:
    int socket_id;
    State protocol_state = State::INITIAL;
    vector <int> Shares;
    unsigned int player_id;
    unsigned int port_number;
    unsigned int max_clients;
    map<int, int> clients;
    int dataset_size;

  public:
    ifstream inpf;

    Server(unsigned int id, unsigned int port, unsigned int max_clients);

    ~Server();
    State get_state();
    void start();
    void run_protocol(); 
    void accept_clients();
    void send_random_triples();
    void get_private_inputs();
    void get_dataset_size();
  };
}

#endif
