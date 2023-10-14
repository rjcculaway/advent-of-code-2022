//-------------------------------------------------------------------------------------------------
// Day 05 Part 01: Supply Stacks
// by Rene Jotham C. Culaway
//
// After the rearrangement procedure completes, what crate ends up on top of each stack?
//-------------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>

const char *FILE_NAME = "input.txt";

/// @brief Structure representing each step in rearranging the crates.
struct RearrangementStep
{
  int origin; // The source stack
  int dest;   // The destination stack
  int count;  // The number of crates to move (at a time)
};

/// @brief Parses an input file for the stacks and rearrangement procedure
/// @param file_name The file name.
/// @return A pair containing the all of the stacks, as well as the rearrangement procedure.
std::pair<std::vector<std::stack<char>>, std::vector<RearrangementStep>> load_rearrangement_procedure_from_file(const char *file_name)
{
  std::vector<std::stack<char>> stacks;
  std::vector<RearrangementStep> rearrangement_procedure;
  std::ifstream file_handle(FILE_NAME);

  if (file_handle.bad())
  {
    return std::pair(stacks, rearrangement_procedure);
  }

  std::string current_line = "";
  std::stack<std::string> lines;
  std::streampos list_line;
  // Find the empty space signifying the end of the stack layout definition
  while (std::getline(file_handle, current_line))
  {
    if (current_line.length() == 0)
    {
      list_line = file_handle.tellg();
      break;
    }
    lines.push(current_line);
  }
  // Keep note of the number of stacks and add that amount to the list of stacks
  int num_of_stacks = (lines.top().length() + 1) / 4;
  for (int i = 0; i < num_of_stacks; i++)
  {
    stacks.emplace_back(std::stack<char>());
  }
  // Remove the last line (the stack labels)
  lines.pop();

  while (!lines.empty())
  {
    std::vector<std::string> crates;
    std::string line = lines.top();
    int absolute_delimiter_location = 0;
    while (true)
    {
      int relative_delimiter_location = line.substr(absolute_delimiter_location, line.length()).find_first_of("[");
      if (relative_delimiter_location == std::string::npos)
      {
        break;
      }
      // After the "[" character is the crate content so we add 1
      absolute_delimiter_location += relative_delimiter_location + 1;
      int stack_idx = (absolute_delimiter_location + 1) / 4; // Since each crate occupies three characters, we divide by 3;
      stacks[stack_idx].push(line[absolute_delimiter_location]);
    }

    lines.pop();
  }

  const std::string rearrangement_step_delimiters[] = {
      "move ",
      "from ",
      "to "};
  // Continue file reading; this time parse the commands
  while (std::getline(file_handle, current_line))
  {
    size_t delimiter_locations[] = {
        0, 0, 0};

    for (size_t i = 0; i < 3; i++)
    {
      delimiter_locations[i] = current_line.find(rearrangement_step_delimiters[i]);
    }

    // Extract the parts of each command by splitting the string
    std::string move_part = current_line.substr(delimiter_locations[0], delimiter_locations[1] - 1);                          // Subtract by 1 to exclude the whitespace
    std::string from_part = current_line.substr(delimiter_locations[1], delimiter_locations[2] - delimiter_locations[1] - 1); // Subtract by 1 to exclude the whitespace
    std::string to_part = current_line.substr(delimiter_locations[2], current_line.length() - delimiter_locations[2]);
    // std::cout << move_part << "|" << from_part << "|" << to_part << "\n";

    // Create a RearrangementStep by extracting the numbers in each split string
    RearrangementStep step;
    step.count = std::stoi(move_part.substr(move_part.find_first_of(" ") + 1, move_part.length()));
    step.origin = std::stoi(from_part.substr(from_part.find_first_of(" ") + 1, from_part.length()));
    step.dest = std::stoi(to_part.substr(to_part.find_first_of(" ") + 1, to_part.length()));

    // std::cout
    //     << step.count << ", "
    //     << step.origin << ", "
    //     << step.dest << "\n";
    rearrangement_procedure.emplace_back(step);
  }

  file_handle.close();
  return std::pair(stacks, rearrangement_procedure);
}

/// @brief Rearranges the crates one at a time.
/// @param crate_stacks A vector of crate stacks.
/// @param rearrangement_procedure Step-by-step procedure on how to move the crates.
/// @return A vector of crate stacks, with the crates already moved.
std::vector<std::stack<char>> rearrange_crates(std::vector<std::stack<char>> crate_stacks, const std::vector<RearrangementStep> &rearrangement_procedure)
{
  for (RearrangementStep step : rearrangement_procedure)
  {
    for (int current_movement = 0; current_movement < step.count; current_movement++)
    {
      // Subtract 1 due to zero-indexing
      char crate = crate_stacks[step.origin - 1].top();
      crate_stacks[step.origin - 1].pop();
      crate_stacks[step.dest - 1].push(crate);
    }
  }
  return crate_stacks;
}

/// @brief Concatenates all of the top of stacks onto a single string.
/// @param stacks A vector containing all of the stacks.
/// @return A string representing all of the tops of the stacks.
std::string get_top_of_stacks(const std::vector<std::stack<char>> &stacks)
{
  std::string tops = "";
  for (auto stack : stacks)
  {
    tops += stack.top();
  }
  return tops;
}

int main(int argc, char *argv[])
{
  std::pair<std::vector<std::stack<char>>, std::vector<RearrangementStep>> stacks = load_rearrangement_procedure_from_file(FILE_NAME);
  std::cout << "TOS: " << get_top_of_stacks(rearrange_crates(stacks.first, stacks.second)) << "\n";
  return 0;
}