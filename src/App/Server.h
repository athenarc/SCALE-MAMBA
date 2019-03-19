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
#include <thread>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>

#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "Protocol.h"
#include "ProtocolEntity.h"
#include "Concurrent_Queue.h"

using namespace std;

namespace sedp {
  class Server: public ProtocolEntity {
  private:
    int socket_id;
    State protocol_state = State::INITIAL;
    unsigned int player_id;
    unsigned int port_number;
    unsigned int max_clients;
    unsigned int accepted_clients;
    unsigned int handled_clients;
    unsigned int total_data;
    thread accept_thread;
    thread handler_thread;
    mutex mtx;
    mutex mtx_data;
    mutex mtx_protocol;
    Concurrent_Queue<int> pending_clients;
    vector<int> data;
    vector<int> random_triples;
    map<int, vector<int>> clients;
    condition_variable protocol_cond;

    void handle_clients ();

  public:
    ifstream inpf;
    ofstream outf;

    Server(unsigned int id, unsigned int port, unsigned int max_clients);

    ~Server();
    void init();
    void accept_clients();
    int accept_single_client();
    bool should_accept_clients();
    bool should_handle_clients();
    void handshake(int client_sd);
    int get_data_size();
    void put_random_triple(const int& s);
    void send_random_triples(int client_sd, int start, int end);
    void get_private_inputs(int client_sd, int dataset_size, int start, vector<int>& vc);
    vector<int> &get_data();
  };
}

#endif
