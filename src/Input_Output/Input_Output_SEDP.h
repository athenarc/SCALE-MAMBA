/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2018, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _InputOutputSEDP
#define _InputOutputSEDP

/* A simple IO class which just uses standard
 * input/output to communicate values
 *
 * Whereas share values are input/output using
 * a steam, with either human or non-human form
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "App/Protocol.h"
#include "App/Server.h"
#include "IO_Stream.h"

using namespace std;

class Input_Output_SEDP : public IO_Stream
{
private:
  unsigned int player_id, number_of_clients;
  sedp::Server s;
  vector<gfp> data;
public:
  string inf_name;
  string onf_name;

  Input_Output_SEDP(unsigned int player_id, int number_of_clients)
      : IO_Stream(), player_id{player_id}, s{player_id, (14000 + player_id), number_of_clients}
  {
    ;
  }

  void set_file_names(string inputFile, string outputFile);

  virtual long open_channel(unsigned int channel);
  virtual void close_channel(unsigned int channel);

  virtual gfp private_input_gfp(unsigned int channel);
  virtual void private_output_gfp(const gfp &output, unsigned int channel);

  virtual void public_output_gfp(const gfp &output, unsigned int channel);
  virtual gfp public_input_gfp(unsigned int channel);

  virtual void public_output_int(const long output, unsigned int channel);
  virtual long public_input_int(unsigned int channel);

  virtual void output_share(const Share &S, unsigned int channel);
  virtual Share input_share(unsigned int channel);

  virtual void trigger(Schedule &schedule);

  virtual void debug_output(const stringstream &ss);

  virtual void crash(unsigned int PC, unsigned int thread_num);
};

#endif
