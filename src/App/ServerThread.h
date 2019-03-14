#ifndef _ServerThread
#define _ServerThread

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
  class ServerThread: public ProtocolEntity {
  private:
    int client_sd;
    int client_id;
    int player_id;
    int dataset_size;
    State protocol_state = State::INITIAL;
    vector<int> shares;
    vector<int> data;

  public:
    ifstream inpf;
    ofstream outf;

    ServerThread(int client_sd, int player_id);

    ~ServerThread();
    void run_protocol(); 
    void handshake(); 
    void send_random_triples();
    void get_private_inputs();
    void get_dataset_size();
  };
}

#endif
