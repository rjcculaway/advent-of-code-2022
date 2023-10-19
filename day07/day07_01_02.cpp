//-------------------------------------------------------------------------------------------------
// Day 07: No Space Left On Device
// by Rene Jotham C. Culaway
//
// Find all of the directories with a total size of at most 100000.
// What is the sum of the total sizes of those directories?
//-------------------------------------------------------------------------------------------------

#include "day07.hpp"

const std::string FILE_NAME = "input.txt";
const unsigned int TOTAL_SIZE = 70000000;
const unsigned int TARGET_SIZE = 100000;
const unsigned int TARGET_SIZE_FOR_DELETION = 30000000;

int main(int argc, char *argv[])
{
  Filesystem filesystem(TOTAL_SIZE);
  parse_commands(filesystem, file_lines_to_vector(FILE_NAME));
  std::vector<std::shared_ptr<Directory>> large_directories = filesystem.find_directories_of_size(TARGET_SIZE);

  std::cout << "Cumulative size: " << compute_directory_sizes(large_directories) << "\n";
  auto for_deletion = filesystem.get_minimum_for_target_size_directory(TARGET_SIZE_FOR_DELETION);
  std::cout << "Directory to delete: " << for_deletion->get_name() << "(" << for_deletion->get_node_size() << ")"
            << "\n";

  return 0;
}