#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id;
  vector <pair<string, int>> player_addresses; 

  if (argc < 2) {
    cout << "Usage: ./Client-Api.x <client_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  int max_players = 3;

  Client c(id, max_players);

  int port = 14000;

  while (port < 14003){
    player_addresses.push_back(make_pair("127.0.0.1", port));
    port++;
  }

  c.connect_to_players(player_addresses);
  
  c.run_protocol();
  return 0;
}
