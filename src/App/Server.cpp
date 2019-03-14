#include "Server.h"
#include "ServerThread.h"

sedp::Server::Server(unsigned int id, unsigned int port, unsigned int max_clients):
  ProtocolEntity(), player_id{id}, port_number{port}, max_clients{max_clients}, current_num_of_clients{0}
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
  return current_num_of_clients < max_clients; 
}


void sedp::Server::accept_clients() {
  while(should_accept_clients()) {
    int client_sd = accept_single_client();

    pending_clients.put(async(launch::async, [=](int client_sd, int player_id)
      {
        ServerThread sthread(client_sd, player_id);
        return sthread.run_protocol();
      },
      client_sd, player_id
    ));

    lock_guard<mutex> g{mtx};
    current_num_of_clients++;
  }
}

void sedp::Server::handle_clients() {
  while(should_accept_clients()) {
    shared_future<void> f;
    pending_clients.get(f);
    f.get(); 
  }
}
