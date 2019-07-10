#include "IO_Factory.h"

unique_ptr<IO_Stream> IO_Factory::make_io(unsigned int choice, unsigned int player_id, int number_of_clients)
{

  if (choice == IO_SIMPLE) {
    return unique_ptr<IO_Stream>(new Input_Output_Simple);
  }
  else if (choice == IO_FILE) {
    return unique_ptr<IO_Stream>(new Input_Output_File);
  }
  else if (choice == IO_SEDP) {
    return unique_ptr<IO_Stream>(new Input_Output_SEDP(player_id, number_of_clients));
  }

  return unique_ptr<IO_Stream>(new Input_Output_Simple);

}
