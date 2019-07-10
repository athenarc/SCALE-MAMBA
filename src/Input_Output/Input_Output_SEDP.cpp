/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2018, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include "Input_Output_SEDP.h"
#include "Exceptions/Exceptions.h"

void Input_Output_SEDP::set_file_names(string a, string b) {
  inf_name = a;
  onf_name = b;
}

long Input_Output_SEDP::open_channel(unsigned int channel)
{
  if (channel == 0) {
    cout << "Init server!" << endl;
    s.init();
  }

  if (channel == 1) {
    cout << "@" << endl; // @ means the server is listening
  }


  if (channel == 3) {
    cout << "Constructing triples!" << endl;
    s.construct_random_tuples();
  }

  if (channel == 4) {
    cout << "Getting data!" << endl;
    data = s.get_data();
  }

  if (channel == 500){
    cout << "# OUTPUT START:" << endl;
  }

  return 0;
}

void Input_Output_SEDP::close_channel(unsigned int channel)
{
  if (channel == 0){
    cout << "Importation finished. Starting SMPC computation..." << endl;
  }

  if (channel == 500){
    cout << "$ OUTPUT END" << endl;
  }
}

gfp Input_Output_SEDP::private_input_gfp(unsigned int channel)
{
  cout << "Input channel " << channel << " : ";
  gfp y;
  y.assign(0);
  return y;
}

void Input_Output_SEDP::private_output_gfp(const gfp &output, unsigned int channel)
{
  cout << "Output channel " << channel << " : ";
  output.output((*outf), true);
  cout << endl;
}

gfp Input_Output_SEDP::public_input_gfp(unsigned int channel)
{
  gfp y;

  if (channel >= 1000){
    y = data.at(0);
    data.erase(data.begin());
    Update_Checker(y, channel);
  } else {
    cout << "Enter value on channel " << channel << " : ";
    y.assign(0);
    // Important to have this call in each version of public_input_gfp
    Update_Checker(y, channel);
  }

  return y;
}

void Input_Output_SEDP::public_output_gfp(const gfp &output, unsigned int channel)
{
  if (channel == 0) {
    output.output(cout, true);
    cout << endl;
  }
}

long Input_Output_SEDP::public_input_int(unsigned int channel)
{
  if (channel == 1) {
    cout << "Getting dataset size on channel: " << channel << endl;
    long x = s.get_data_size(); // returns int. Should fix to return long

    // Important to have this call in each version of public_input_gfp
    Update_Checker(x, channel);

    return x;
  }

  return 0;
}

void Input_Output_SEDP::public_output_int(const long output, unsigned int channel)
{
  if (channel == 0) {
    cout << output << ",";
  }
}

void Input_Output_SEDP::output_share(const Share &S, unsigned int channel)
{
  if (channel == 2) {
    stringstream os;
    int id;
    gfp r, mac;
    S.output(os, human);
    os >> id >> r >> mac;
    auto x = make_tuple(id, r, mac);
    s.add_random_sint_share(x);
    return;
  }
  cout << "WRONG Output Shares..." << endl;
  S.output(*outf, human);
}

Share Input_Output_SEDP::input_share(unsigned int channel)
{
  cout << "Enter value on channel " << channel << " : ";
  Share S;
  S.input(*inpf, human);
  return S;
}

void Input_Output_SEDP::trigger(Schedule &schedule)
{
  printf("Restart requested: Enter a number to proceed\n");
  int i;
  cin >> i;

  // Load new schedule file program streams, using the original
  // program name
  //
  // Here you could define programatically what the new
  // programs you want to run are, by directly editing the
  // public variables in the schedule object.
  unsigned int nthreads= schedule.Load_Programs();
  if (schedule.max_n_threads() < nthreads)
    {
      throw Processor_Error("Restart requires more threads, cannot do this");
    }
}

void Input_Output_SEDP::debug_output(const stringstream &ss)
{
  printf("%s", ss.str().c_str());
  fflush(stdout);
}

void Input_Output_SEDP::crash(unsigned int PC, unsigned int thread_num)
{
  printf("Crashing in thread %d at PC value %d\n", thread_num, PC);
  throw crash_requested();
}
