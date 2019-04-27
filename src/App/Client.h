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
#include <fstream>
#include <mutex>
#include <thread>
#include <future>

#include "App/App.h"
#include "System/Player.h"
#include "System/Networking.h"
#include "Exceptions/Exceptions.h"
#include "Tools/int.h"

#include "Protocol.h"
#include "ProtocolEntity.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

namespace sedp {
  class Client: public ProtocolEntity {
  private:
    State protocol_state = State::INITIAL;
    unsigned int client_id;
    unsigned int max_players;
    string dataset_file_path;
    int dataset_size;
    mutex mtx;
    vector<vector<gfp>> triples;
    vector<SSL *> players;
    vector<gfp> data, mask;
    vector <bigint> p;
    const SSL_METHOD *method;
    vector <SSL *> ssl;
    SSL_CTX *ctx;

    template <typename F>
    void execute(F cb);

  public:
    ifstream inpf;

    Client(unsigned int id, unsigned int max_players, string dataset);

    ~Client();
    State get_state();
    int get_id();
    void run_protocol();
    void handshake(int player_id);
    void LoadCertificates(SSL_CTX *ctx, const char *CertFile, const char *KeyFile);
    SSL * connect_to_player(string ip, int port);
    void connect_to_players(const vector <pair <string, int>>& p_addresses);
    void send_dataset_size(int player_id);
    void compute_mask();
    void send_private_inputs(int player_id);
    void get_random_tuples(int player_id);
    void init();
    void Init_SSL_CTX(SSL_CTX *&ctx);
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
