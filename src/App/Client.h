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
    vector <State> protocol_states;
    unsigned int client_id;
    unsigned int max_players;
    vector<int> players;
    int dataset_size;

  public:
    ifstream inpf;

    Client(unsigned int id, unsigned int max_clients);
    
    ~Client();
    State get_state();
    int get_id();
    void run_protocol(int player_id); 
    int connect_to_player(struct sockaddr_in addr);
    void connect_to_players(vector <pair <string,int>> p_addresses);
    void send_dataset_size(int player_id);
    void send_private_inputs(int player_id);
    void get_random_triples(int player_id);
  };
}


#endif
