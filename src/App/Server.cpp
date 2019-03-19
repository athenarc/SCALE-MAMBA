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

bool sedp::Server::finished_import() {
  lock_guard<mutex> g{mtx};
  return dataset_accepted >= max_clients;
}

void sedp::Server::accept_clients() {
  while(should_accept_clients()) {
    int client_sd = accept_single_client();

    pending_clients.put(async(launch::async, [=](int client_sd, int player_id)->vector<int> // return vector<int>
      {
        ServerThread sthread(client_sd, player_id);
        sthread.run_protocol();
        return sthread.get_data();
      },
      client_sd, player_id
    ));

    current_num_of_clients++;
  }
}

void sedp::Server::handle_clients() {
  while(!finished_import()) {
    future<vector<int>> f;
    pending_clients.get(f);
    vector<int> tmp = f.get();

    lock_guard<mutex> g{mtx_data};
    total_data.insert(end(total_data), begin(tmp), end(tmp));
    dataset_accepted++;
  }
}
