#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id;
  sockaddr_in addr;
  vector <struct sockaddr_in> Player_addresses; 

  if (argc < 2) {
    cout << "Usage: ./Client-Api.x <client_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  int max_players = 3;

  Client c(id, max_players);

  int port = 14000;
  while (port < 14003){
    bzero(&addr, sizeof(addr));
    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr= INADDR_ANY;
    addr.sin_port= htons(port);
    Player_addresses.push_back(addr);
    port++;
  }

  c.connect_to_players(Player_addresses);
  
  int counter = 0;
  while (counter < 3){
    c.run_protocol(counter);
    counter++;
  }
  return 0;
}
