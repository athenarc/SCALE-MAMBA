#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id;
  sockaddr_in addr;
  vector <pair<string,int>> player_addresses; 

  if (argc < 2) {
    cout << "Usage: ./Client-Api.x <client_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  int max_players = 3;

  Client c(id, max_players);

  int port = 14000;
  pair<string,int> address;
  while (port < 14003){
    address.first = "172.0.0.1";
    address.second = port;
    player_addresses.push_back(address);
    port++;
  }

  c.connect_to_players(player_addresses);
  
  int counter = 0;
  while (counter < 3){
    c.run_protocol(counter);
    counter++;
  }
  return 0;
}
