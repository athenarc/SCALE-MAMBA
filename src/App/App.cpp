#include "App.h"

unique_ptr<IO_Stream> App::make_IO() {
  unique_ptr<IO_Stream> io = IO_Factory::make_io(IO_SEDP, playerID, number_of_clients);
  unique_ptr<Input_Output_SEDP> iof(dynamic_cast<Input_Output_SEDP*>(io.release()));

  iof->set_file_names("src/App/files/in" + to_string(playerID) + ".txt", "src/App/files/out" + to_string(playerID) + ".txt");
  iof->init_streams(ifs, ofs, true);

  return iof;
}
