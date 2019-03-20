#include <iostream>
#include <sstream>

#include "Math/gfp.h"
#include "Math/gf2n.h"
#include "Tools/int.h"
#include "App/Protocol.h"
#include "App/Server.h"

using namespace std;
using namespace sedp;

void parseRandomVariable(string& line, gfp& x) {
  stringstream ss(line);
  gfp mac;
  ss >> x >> mac;
}

int main(int argc, const char *argv[]) {
  int id;
  int max_clients;

  if (argc < 3) {
    cout << "Usage: ./Server-Api.x <server_id> <total_clients>" << endl;
    exit(-1);
  }

  id = atoi(argv[1]);
  max_clients = atoi(argv[2]);

  // use clock as seed
  srand(time(0) + id);

  bigint p;
  string filename = "Data/SharingData.txt";

  cout << "loading params from: " << filename << endl;

  ifstream inpf(filename.c_str());
  if (inpf.fail()) { throw file_error(filename.c_str()); }

  inpf >> p;

  inpf.close();

  gfp::init_field(p);
  gf2n::init_field(128); // Assumes 128-bit prime generation

  // open_channel(0)
  Server s(id, 14000 + id, max_clients);
  s.init();

  // open_channel(1)
  // get_public_input(1)
  int n = s.get_data_size();
  cout << "size: " << n << endl;

  // open_channel(2)
  vector<vector<gfp>> triples;
  string line;
  ifstream triples_file("Player_shares" + to_string(id) + ".txt");

  while (getline(triples_file, line)) {
    gfp a, b, c;
    parseRandomVariable(line, a);

    getline(triples_file, line);
    parseRandomVariable(line, b);

    getline(triples_file, line);
    parseRandomVariable(line, c);

    vector<gfp> triple_shares{a, b, c};
    triples.push_back(triple_shares);
  }

  triples_file.close();

  for (int i = 0; i < n; i++) {
    s.put_random_triple(shares.at(i));
  }

  // open_channel(3)
  vector<int>& data = s.get_data();

  for (auto &d : data) {
    // get_private_input(3);
    cout << d << endl;
  }
} 
