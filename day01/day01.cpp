#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
  std::ifstream input_file;

  input_file.open("day01_input.txt");

  if (!input_file.good())
  {
    input_file.close();
    return 0;
  }

  std::string current_line;

  bool is_first_elf = true;
  unsigned int max_calorie = 0;
  unsigned int current_elf_calorie = 0;
  while (std::getline(input_file, current_line))
  {
    // std::cout << current_line;
    if (current_line.size() <= 0)
    {
      // The first elf is always the first max.
      if (is_first_elf || current_elf_calorie > max_calorie)
      {
        max_calorie = current_elf_calorie;
        is_first_elf = false;
        std::cout << "Current max calorie:\t" << max_calorie << "\n";
      }

      current_elf_calorie = 0;
    }
    else
    {
      current_elf_calorie += std::stoi(current_line);
    }
  }

  input_file.close();

  return 0;
}