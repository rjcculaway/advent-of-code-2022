//-------------------------------------------------------------------------------------------------
// Day 03 Part 02: Rucksack Reorganization
// by Rene Jotham C. Culaway
//
// Find the item type that corresponds to the badges of each three-Elf group.
// What is the sum of the priorities of those item types?
//-------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <map>
#include <vector>
#include <string>

const int ELVES_PER_GROUP = 3;

/// @brief Converts a character in ASCII to its equivalent priority. 1-26 is the priority for a to z, while 27-53 is the priority for A-Z.
/// @param c The character to convert
/// @return The priorty value of the character.
int item_type_to_priority(char c)
{
  int value = (int)c;
  bool is_lowercase = value > 96; // The first lowercase character begins at 97
  if (is_lowercase)
  {
    return value - 96;
  }

  return value - 64 + 26;
}

int main(int argc, char *argv[])
{
  std::ifstream file_handle;
  std::string current_line;

  file_handle.open("input.txt");

  if (!file_handle.good())
  {
    return 1;
  }

  std::map<char, int> item_type_frequency;
  std::vector<std::unordered_set<char>> rucksacks = {
      std::unordered_set<char>(),
      std::unordered_set<char>(),
      std::unordered_set<char>()};
  std::vector<char> badges;

  int elf_idx = 0;
  while (std::getline(file_handle, current_line))
  {

    for (char c : current_line)
    {
      rucksacks[elf_idx].insert(c);
    }

    if (++elf_idx == ELVES_PER_GROUP)
    {
      // Count frequency of each item type in each elf's rucksack
      for (auto i = rucksacks.begin(); i != rucksacks.end(); i++)
      {
        std::unordered_set<char> rucksack = *i;
        for (auto j = rucksack.begin(); j != rucksack.end(); j++)
        {
          char item_type = *j;
          // Item type not yet in map
          if (item_type_frequency.find((item_type)) == item_type_frequency.end())
          {
            item_type_frequency[item_type] = 1;
          }
          else // Item type already in map
          {
            item_type_frequency[item_type]++;
          }
        }
      }
      // Those with frequency of 3 are the badges; therefore include them in the badges list
      for (auto i = item_type_frequency.begin(); i != item_type_frequency.end(); i++)
      {
        std::pair<char, int> item_type = *i;
        int frequency = item_type.second;
        if (frequency == ELVES_PER_GROUP)
        {
          badges.emplace_back(item_type.first);
          break;
        }
      }

      // Reset rucksacks
      for (auto i = rucksacks.begin(); i != rucksacks.end(); i++)
      {
        (*i).clear();
      }
      // Reset item type frequencies
      item_type_frequency.clear();
      // Reset elf (start a new elf group)
      elf_idx = 0;
    }
  }

  int priority_sum = 0;

  for (auto i = badges.begin(); i != badges.end(); i++)
  {
    priority_sum += item_type_to_priority(*i);
  }

  std::cout << "sum:\t" << priority_sum;
  return 0;
}