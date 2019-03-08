#ifndef _Client
#define _Client

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
#include <App/App.h>
#include <fstream>

#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "Protocol.h"
#include "ProtocolEntity.h"

using namespace std;

namespace sedp {
  class Client: public ProtocolEntity {
  private:
    int socket_id;
    State protocol_state = State::INITIAL;
    unsigned int client_id;
    unsigned int max_players;
    vector<int> players;

  public:
    ifstream inpf;

    Client(unsigned int id, unsigned int max_clients);
    
    ~Client();
    State get_state();
    int get_id();
    void run_protocol(); 
    int connect_to_player(const string& ip_address, int port);
  };
}


#endif
