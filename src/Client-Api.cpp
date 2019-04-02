#include <iostream>
#include "App/Client.h"

using namespace std;
using namespace sedp;

int main(int argc, const char *argv[]) {

  int id, n_players;
  string rootCertName;
  vector <pair<string, int>> player_addresses;
  string networkDataFile = "Data/NetworkData.txt";

  if (argc < 2) {
    cout << "Usage: ./Client-Api.x <client_id>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);

  Client c(id, n_players);
  int port = 14000;
  vector <string> ipNumbers;

  ifstream inp(networkDataFile.c_str());
  if (inp.fail())
  {
    throw file_error(networkDataFile.c_str());
  }

  inp >> rootCertName;
  inp >> n_players;

  ipNumbers.resize(n_players);

  for (int i= 0; i < n_players; i++)
  { 
    string cert, name;
    int j;
    inp >> j >> ipNumbers[i] >> cert >> name;
  }

  for (int i= 0; i < n_players; i++)
  {
    player_addresses.push_back(make_pair(ipNumbers[i], port));
    port++;
  }

  c.init();
  c.connect_to_players(player_addresses);
  c.run_protocol();

  return 0;
}
