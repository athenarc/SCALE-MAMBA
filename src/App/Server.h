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
    unsigned int current_num_of_clients;
    unsigned int dataset_accepted;
    thread accept_thread;
    thread handler_thread;
    mutex mtx;
    Concurrent_Queue<shared_future<void>> pending_clients;

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
    bool finished_import();
  };
}

#endif
