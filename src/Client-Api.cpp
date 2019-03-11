#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id, n_players;
  vector <pair<string, int>> player_addresses; 

  if (argc < 3) {
    cout << "Usage: ./Client-Api.x <client_id> <n_players>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  n_players = atoi(argv[2]);

  Client c(id, n_players);
  int port = 14000;

  while (port < 14000 + n_players){
    player_addresses.push_back(make_pair("127.0.0.1", port));
    port++;
  }

  c.connect_to_players(player_addresses);
  
  c.run_protocol();
  return 0;
}
