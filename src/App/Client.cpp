#include "Client.h"

sedp::Client::Client(unsigned int id, unsigned int max_players, string dataset):
  client_id{id}, max_players{max_players}, dataset_file_path{dataset}, dataset_size{0}
{
  ssl.resize(max_players);
}

sedp::Client::~Client() {
  cout << "Closing client..." << endl;

  // for (vector<SSL *>::iterator it = players.begin() ; it != players.end(); ++it){
  //   SSL_close(*it);
  // }

  cout << "Client closed!"<<endl;
}

void sedp::Client::init() {
  int datum;

  initialise_fields("Data/SharingData.txt");

  inpf.open(dataset_file_path);

  if (inpf.fail()){
    throw file_error(dataset_file_path.c_str());
  }

  while (inpf >> datum) {
    gfp x;
    x.assign(datum);
    data.push_back(datum);
  }

  inpf.close();

  dataset_size = data.size();
  cout << "Dataset size: " << dataset_size << endl;

  // Initialize shares matrix
  triples.assign(dataset_size, vector<gfp>(5));
  Init_SSL_CTX(ctx);
}

SSL_CTX *InitCTX(void)
{
  const SSL_METHOD *method;
  SSL_CTX *ctx;

  method= TLS_method();     /* create new server-method instance */
  ctx= SSL_CTX_new(method); /* create new context from method */

  if (ctx == NULL)
    {
      ERR_print_errors_fp(stdout);
      throw SSL_error("InitCTX");
    }

  SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

  return ctx;
}

void sedp::Client::Init_SSL_CTX(SSL_CTX *&ctx)
{
  // Initialize the SSL library
  OPENSSL_init_ssl(
      OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
  ctx = InitCTX();

  // Load in my certificates
  string str_crt= "Cert-Store/Client" + to_string(client_id) + ".crt";
  string str_key= str_crt.substr(0, str_crt.length() - 3) + "key";
  cout << str_crt << str_key << endl;
  LoadCertificates(ctx, str_crt.c_str(), str_key.c_str());

  // Turn on client auth via cert
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                     NULL);

  // Load in root CA
  string str= "Cert-Store/RootCA.crt";
  SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(str.c_str()));
  SSL_CTX_load_verify_locations(ctx, str.c_str(), NULL);
}

void sedp::Client::LoadCertificates(SSL_CTX *ctx, const char *CertFile, const char *KeyFile)
{
  /* set the local certificate from CertFile */
  if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
    {
      ERR_print_errors_fp(stdout);
      throw SSL_error("LoadCertificates 1");
    }
  /* set the private key from KeyFile (may be the same as CertFile) */
  if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
    {
      ERR_print_errors_fp(stdout);
      throw SSL_error("LoadCertificates 2");
    }
  /* verify private key */
  if (!SSL_CTX_check_private_key(ctx))
    {
      throw SSL_error("Private key does not match the public certificate");
    }
}

void sedp::Client::initialise_fields(const string& filename)
{
  bigint p;

  cout << "loading params from: " << filename << endl;

  ifstream inpf(filename.c_str());
  if (inpf.fail()) { throw file_error(filename.c_str()); }

  inpf >> p;
  cout << "p = " << p << endl;
  inpf.close();

  gfp::init_field(p);
  gf2n::init_field(128); // Assumes 128-bit prime generation
}


SSL * sedp::Client::connect_to_player(string ip, int port) {
  int socket_id = OpenConnection(ip, port);

  SSL *ssl_ = SSL_new(ctx);

  SSL_set_fd(ssl_, socket_id);
  if ( SSL_connect(ssl_) <= 0 ){   /* perform the connection */
    ERR_print_errors_fp(stderr);
  }

  cout << "SSL_connection established"<<endl;
  return ssl_;
}

void sedp::Client::handshake(int player_id) {
  lock_guard<mutex> g{mtx};
  int p_id = receive_int_from(players.at(player_id)); // should we save the id that the player sent ?
  cout << "Connected to player with id: " << player_id << endl;
  send_int_to(players.at(player_id), client_id);
  send_int_to(players.at(player_id), dataset_size);
}

void sedp::Client::connect_to_players(const vector <pair <string, int>>& player_addresses) {
  vector<pair <string, int>>::const_iterator it;

  for (it = player_addresses.begin() ; it != player_addresses.end(); ++it) {
    SSL * player_sd = connect_to_player((*it).first, (*it).second);
    players.push_back(player_sd);
  }

}

int sedp::Client::get_id(){
  return client_id;
}

void sedp::Client::compute_mask() {
  for (int i = 0; i < dataset_size; i++)
  {
    mask.push_back(data[i] - triples[i][0]);
  }
}

void sedp::Client::send_dataset_size(int player_id) {
  lock_guard<mutex> g{mtx};
  send_int_to(players.at(player_id), dataset_size);
  cout << "Succesfully sent dataset size!" <<endl;
}

void sedp::Client::send_private_inputs(int player_id) {
  lock_guard<mutex> g{mtx};
  cout << "Sending private data..." << endl;

  for (int i = 0; i < dataset_size; i++) {
    string s = gfp_to_str(mask[i]);
    send_to(players.at(player_id), s);
  }

  cout << "Succesfully sent my data to player " + to_string(player_id) + "!" << endl;

}

void sedp::Client::get_random_tuples(int player_id) {
  lock_guard<mutex> g{mtx};
  cout << "Listening for shares of player " + to_string(player_id) + "..." << endl;

  for (int i = 0; i < dataset_size; i++) {
    string s;
    receive_from(players.at(player_id), s);

    vector<gfp> triple_shares;
    unpack(s, triple_shares);

    for (int j = 0; j < 5; j++)
    {
      triples[i][j] += triple_shares[j];
    }

  }

  cout << "Succesfully received shares of player " + to_string(player_id) + "!" << endl;

}

void sedp::Client::verify_triples() {
  for (int i = 0; i < dataset_size; i++)
  {
    if (triples[i][0] * triples[i][1] != triples[i][2])
    {
      cerr << "Incorrect triple at " << i << ", aborting\n";
      cout << triples[i][0] * triples[i][1] << endl;
      cout << triples[i][2] << endl;
      // exit(1);
    }
    if (triples[i][1] * triples[i][3] != triples[i][4])
    {
      cerr << "Incorrect triple at " << i << ", aborting\n";
      cout << triples[i][0] * triples[i][1] << endl;
      cout << triples[i][2] << endl;
      // exit(1);
    }
  }
}

void sedp::Client::run_protocol() {
  while(protocol_state != State::FINISHED) {
    switch(protocol_state) {
      case State::INITIAL: {
        // here should init & connect to players. Client object should be initialized with player_addresses
        protocol_state= State::HANDSHAKE;
        break;
      }

      case State::HANDSHAKE: {
        execute(&Client::handshake);
        protocol_state= State::RANDOMNESS;
        break;
      }

      case State::RANDOMNESS: {
        execute(&Client::get_random_tuples);
        verify_triples();
        protocol_state = State::DATA;
        break;
      }

      case State::DATA: {
        cout << "Computing Mask ..." << endl;
        compute_mask();
        cout << "Mask Computed ..." << endl;
        execute(&Client::send_private_inputs);
        protocol_state = State::FINISHED;
        break;
      }

      case State::FINISHED:{
        break;
      }

    } // end switch
  } // end while-loop
} // end run_protocol
