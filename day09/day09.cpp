//-------------------------------------------------------------------------------------------------
// Day 09: Rope Bridge
// by Rene Jotham C. Culaway
//
// Simulate your complete hypothetical series of motions.
// How many positions does the tail of the rope visit at least once?
//-------------------------------------------------------------------------------------------------

#include "day09.hpp"

const std::string FILE_NAME = "input.txt";
const int NUM_OF_SEGMENTS = 2;

int main(int argc, char *argv[])
{
  Bridge bridge(NUM_OF_SEGMENTS);

  std::ofstream output_file_handle("output.txt");
  parse_movement_from_file(FILE_NAME, bridge, output_file_handle);

  bridge.print_visited(output_file_handle);
  std::cout << "Positions visited:\t" << bridge.count_positions_visited() << std::endl;

  output_file_handle.close();
  return 0;
}