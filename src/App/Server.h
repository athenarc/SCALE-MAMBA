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
#include <tuple>
#include <thread>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>

// No need to initialize gpf as it will be done by SCALE
#include "Math/gfp.h"
#include "Math/gf2n.h"
#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "System/Player.h"
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
    Concurrent_Queue<SSL*> pending_clients;
    vector<gfp> data;
    vector<vector<gfp>> random_triples;
    vector<tuple<int, gfp, gfp>> random_integers;
    map<vector<int>, SSL *> clients;
    condition_variable protocol_cond;

    SSL_METHOD *method;
    SSL *ssl;

    bigint p;

    void handle_clients ();

  public:
    ifstream inpf;
    ofstream outf;

    Server(unsigned int id, unsigned int port, unsigned int max_clients);

    ~Server();
    void init();
    void set_p(bigint p_val);
    void accept_clients();
    SSL * accept_single_client();
    bool should_accept_clients();
    bool should_handle_clients();
    void handshake(SSL * client_sd);
    int get_data_size();
    void put_random_triple(vector<gfp>& triple_share);
    void send_random_triples(SSL* ssl, int start, int end);
    void get_private_inputs(SSL* ssl, int dataset_size, int start, vector<gfp>& vc);
    vector<gfp> &get_data();
    void add_random_sint_share(tuple<int, gfp, gfp>& r);
    void construct_random_tuples();
  };
}

#endif
