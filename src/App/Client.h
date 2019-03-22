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
#include <mutex>
#include <thread>
#include <future>

#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "Protocol.h"
#include "ProtocolEntity.h"

using namespace std;

namespace sedp {
  class Client: public ProtocolEntity {
  private:
    State protocol_state = State::INITIAL;
    unsigned int client_id;
    unsigned int max_players;
    int dataset_size;
    mutex mtx;
    vector<vector<gfp>> triples;
    vector<int> players;
    vector<gfp> data; 
    
    template <typename F>
    void execute(F cb);

  public:
    ifstream inpf;

    Client(unsigned int id, unsigned int max_players);

    ~Client();
    State get_state();
    int get_id();
    void run_protocol();
    void handshake(int player_id);
    int connect_to_player(string ip, int port);
    void connect_to_players(const vector <pair <string, int>>& p_addresses);
    void send_dataset_size(int player_id);
    void compute_mask();
    void send_private_inputs(int player_id);
    void get_random_triples(int player_id);
    void init();
    void initialise_fields(const string& filename);
    void verify_triples();
  };
}

template <typename F>
void sedp::Client::execute(F cb) {
  vector<future<void>> res;

  for (unsigned int i = 0; i < players.size(); i++) {
    res.push_back(async(launch::async, cb, this, i));
  }

  for (auto& r : res) {
    r.get(); // wait for all calls to finish
  }
}

#endif
