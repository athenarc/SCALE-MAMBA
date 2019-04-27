#include "Server.h"

sedp::Server::Server(unsigned int id, unsigned int port, unsigned int expected_clients){
  ProtocolEntity();
  player_id = id;
  port_number = port;
  max_clients = expected_clients;
  accepted_clients = 0;
  handled_clients = 0;
  total_data = 0;
};

sedp::Server::~Server() {

  if(accept_thread.joinable())
  {
    accept_thread.join();
  }

  if(handler_thread.joinable())
  {
    handler_thread.join();
  }

  cout << "Closing server...." << endl;
  close(socket_id);
}

void sedp::Server::init() {
  init_ssl();
  socket_id = OpenListener(port_number, max_clients);
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = sizeof(addr);

  cout << "Server (Player) " << player_id << ": Start listening at port " << port_number << endl;
  protocol_state = State::HANDSHAKE;

  accept_thread = std::thread(&sedp::Server::accept_clients, this);
  handler_thread = std::thread(&sedp::Server::handle_clients, this);
}

void sedp::Server::init_ssl() {
  SystemData SD("Data/NetworkData.txt");
  Init_SSL_CTX(ctx, player_id, SD);
}

void sedp::Server::set_p(bigint p_val){
  p = p_val;
}

SSL * sedp::Server::accept_single_client() {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = sizeof(addr);
  SSL* ssl = SSL_new(ctx);
  cout << "Waiting for client connection" << endl;
  int client_sd = accept(socket_id, (struct sockaddr *) &addr, &len);

  int ret = SSL_set_fd(ssl, client_sd);

  if (ret == 0){
    throw SSL_error("SSL_set_fd");
  }

  ret = SSL_accept(ssl);

  if (ret <= 0) {
      cout << SSL_get_error(ssl, ret) << endl;
      ERR_print_errors_fp(stdout);
      throw SSL_error("SSL_accept");
  }

  cout << "SSL_connection established" << endl;

  printf("Accepted Connection: %s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  return ssl;
}

bool sedp::Server::should_accept_clients() {
  lock_guard<mutex> g{mtx};
  return accepted_clients < max_clients;
}

bool sedp::Server::should_handle_clients() {
  lock_guard<mutex> g{mtx};
  return handled_clients < max_clients;
}

void sedp::Server::handshake(SSL* ssl) {
  send_int_to(ssl, player_id);
  int client_id = receive_int_from(ssl);
  int dataset_size = receive_int_from(ssl);
  lock_guard<mutex> g{mtx};
  vector<int>c{client_id,dataset_size};
  clients.insert(pair<vector<int>, SSL *>(c, ssl));
  total_data += dataset_size;

  cout << "Client with id " << client_id << " connected." <<endl;
  cout << "Dataset size: " << dataset_size << endl;
}

int sedp::Server::get_data_size() {
  unique_lock<mutex> lck{mtx_protocol};
  protocol_cond.wait(lck, [this]{ return protocol_state == State::RANDOMNESS; });
  return total_data;
}

vector<gfp> &sedp::Server::get_data() {
  unique_lock<mutex> lck{mtx_protocol};
  protocol_cond.wait(lck, [this]{ return protocol_state == State::FINISHED; });
 return data;
}


void sedp::Server::add_random_sint_share(tuple<int, gfp, gfp>& r) {
  random_integers.push_back(r);
}


void sedp::Server::construct_random_tuples() {

  vector<gfp> triple_share;

  for (auto &r : random_integers) {
      // int id = get<0>(r);
      gfp x = get<1>(r);
      // gfp mac = get<2>(r);
      triple_share.push_back(x);

      if (triple_share.size() == 5) {
        random_triples.push_back(triple_share);
        triple_share.clear();
      }
  }

  unique_lock<mutex> lck{mtx_protocol};
  protocol_state = State::DATA;
  protocol_cond.notify_all();
}

void sedp::Server::put_random_triple(vector<gfp>& triple_share) {
  unique_lock<mutex> lck{mtx_protocol};
  random_triples.push_back(triple_share);

  if (random_triples.size() >= total_data) {
    protocol_state = State::DATA;
    protocol_cond.notify_all();
  }
}

void sedp::Server::send_random_triples(SSL* ssl, int start, int end) {
  cout << "Thread:" << this_thread::get_id() << " Sending my Shares..." << endl;

  for (int i = start; i < end; i++) {
    string s;
    pack(random_triples.at(i), s);
    send_to(ssl, s);
  }

  cout << " Succesfully sent my shares!" << endl;
}

void sedp::Server::get_private_inputs(SSL* ssl, int dataset_size, int start, vector<gfp>& vc) {
  cout << "Thread:" << this_thread::get_id() << " Importing data..." << endl;

  for (int i = 0; i < dataset_size; i++) {
    string s;
    receive_from(ssl, s);

    gfp y = str_to_gfp(s);
    vc.push_back(y);

  }
}

void sedp::Server::accept_clients() {
  while(should_accept_clients()) {
    SSL *ssl = accept_single_client();
    pending_clients.put(ssl);
    accepted_clients++;
  }
}

void sedp::Server::handle_clients() {
  while(should_handle_clients()) {
    SSL* csd;
    pending_clients.get(csd);
    handshake(csd);
    handled_clients++;
  }

  {
    unique_lock<mutex> lck{mtx_protocol};
    cout << "All clients handshaked" << endl;
    protocol_state = State::RANDOMNESS;
    protocol_cond.notify_all();
  }

  {
    unique_lock<mutex> lck{mtx_protocol};
    protocol_cond.wait(lck, [this]{ return protocol_state == State::DATA; });
  }

  vector<future<vector<gfp>>> responses;
  map<vector<int>, SSL*>::iterator itr;
  int start = 0;
  int end = 0;

  for (itr = clients.begin(); itr != clients.end(); ++itr) {
    int client_id = itr->first[0];
    int dataset_size = itr -> first[1];
    SSL* ssl = itr->second;

    end += dataset_size;

    responses.push_back(async(launch::async, [this](SSL* ssl, int dataset_size, int start, int end)->vector<gfp> // return vector<int>
      {
        vector<gfp> v;
        v.reserve(dataset_size);
        send_random_triples(ssl, start, end);
        get_private_inputs(ssl, dataset_size, start, v);

        return v;
      },
      ssl, dataset_size, start, end
    ));

    start += dataset_size;
  }

  for (unsigned int i = 0; i != responses.size(); ++i) {
    vector<gfp> tmp = responses.at(i).get();
    lock_guard<mutex> g{mtx_data};
    data.insert(std::end(data), std::begin(tmp), std::end(tmp));
  }

  {
    unique_lock<mutex> lck{mtx_protocol};
    cout << "Data retrieved!" << endl;
    protocol_state = State::FINISHED;
    protocol_cond.notify_all();
  }
}
