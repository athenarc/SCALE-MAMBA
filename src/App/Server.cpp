#include "Server.h"
#include "ServerThread.h"

sedp::Server::Server(unsigned int id, unsigned int port, unsigned int max_clients):
  ProtocolEntity(), player_id{id}, port_number{port}, max_clients{max_clients},
  accepted_clients{0}, handled_clients{0}, total_data{0}
{
  cout << "Server (Player) " << player_id << ": Start listening at port " << port << endl;
  socket_id = OpenListener(port_number, max_clients);
}

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
 protocol_state = State::HANDSHAKE;
 accept_thread = std::thread(&sedp::Server::accept_clients, this);
 handler_thread = std::thread(&sedp::Server::handle_clients, this);
}

int sedp::Server::accept_single_client() {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = sizeof(addr);

  cout << "Waiting for client connection" << endl;

  int client_sd = accept(socket_id, (struct sockaddr *) &addr, &len);

  if (client_sd == -1)
  {
    string err= "Unable to accept connections : Error code " + errno;
    throw Networking_error(err);
  }

  printf("Accepted Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  return client_sd;
}

bool sedp::Server::should_accept_clients() {
  lock_guard<mutex> g{mtx};
  return accepted_clients < max_clients;
}

bool sedp::Server::should_handle_clients() {
  lock_guard<mutex> g{mtx};
  return handled_clients < max_clients;
}

void sedp::Server::handshake(int client_sd) {
  send_int_to(client_sd, player_id);
  int client_id = receive_int_from(client_sd);
  int dataset_size = receive_int_from(client_sd);

  vector<int> c{client_sd, dataset_size};

  lock_guard<mutex> g{mtx};
  clients.insert(pair<int, vector<int>>(client_id, c));
  total_data += dataset_size;

  cout << "Client with id " << client_id << " connected." <<endl;
  cout << "Dataset size: " << dataset_size << endl;
}

int sedp::Server::get_data_size() {
  unique_lock<mutex> lck{mtx_protocol};
  protocol_cond.wait(lck, [this]{ return protocol_state == State::RANDOMNESS; });
  return total_data;
}

vector<int> &sedp::Server::get_data() {
  unique_lock<mutex> lck{mtx_protocol};
  protocol_cond.wait(lck, [this]{ return protocol_state == State::FINISHED; });
 return data;
}

void sedp::Server::put_random_triple(const int& s) {
  unique_lock<mutex> lck{mtx_protocol};
  random_triples.push_back(s);

  if (random_triples.size() == total_data) {
    protocol_state = State::DATA;
    protocol_cond.notify_all();
  }
}

void sedp::Server::send_random_triples(int client_sd, int start, int end) {
  cout << "Thread:" << this_thread::get_id() << " Sending my Shares..." << endl;
  this_thread::sleep_for(std::chrono::seconds(3));

  for (int i = start; i < end; i++) {
    send_int_to(client_sd, random_triples.at(i)); // Need to send actuall shares!
  }

  cout << " Succesfully sent my shares!" << endl;
}

void sedp::Server::get_private_inputs(int client_sd, int dataset_size, int start, vector<int>& vc) {
  cout << "Thread:" << this_thread::get_id() << " Importing data..." << endl;
  this_thread::sleep_for(std::chrono::seconds(3));

  for (int i = 0; i < dataset_size; i++) {
    int datum = receive_int_from(client_sd);
    int secret_share;
    
    secret_share = datum + random_triples.at(start + i);

    vc.push_back(secret_share);
  }
}

void sedp::Server::accept_clients() {
  unique_lock<mutex> lck{mtx_protocol};
  protocol_cond.wait(lck, [this]{ return protocol_state == State::HANDSHAKE; });

  while(should_accept_clients()) {
    int client_sd = accept_single_client();
    pending_clients.put(client_sd);
    accepted_clients++;
  }
}

void sedp::Server::handle_clients() {
  {
    unique_lock<mutex> lck{mtx_protocol};
    protocol_cond.wait(lck, [this]{ return protocol_state == State::HANDSHAKE; });

    while(should_handle_clients()) {
      int csd;
      pending_clients.get(csd);
      handshake(csd);
      handled_clients++;
    }
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

  vector<future<vector<int>>> responses;
  map<int, vector<int>>::iterator itr;
  int start = 0;
  int end = 0;

  for (itr = clients.begin(); itr != clients.end(); ++itr) {
    int client_sd = itr->second[0];
    int dataset_size = itr->second[1];

    end += dataset_size;

    responses.push_back(async(launch::async, [this](int client_sd, int dataset_size, int start, int end)->vector<int> // return vector<int>
      {
        vector<int> v;
        v.reserve(dataset_size);
        send_random_triples(client_sd, start, end);
        get_private_inputs(client_sd, dataset_size, start, v);

        return v;
      },
      client_sd, dataset_size, start, end
    ));

    start += dataset_size;
  }

  for (unsigned int i = 0; i != responses.size(); ++i) {
    vector<int> tmp = responses.at(i).get();
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
