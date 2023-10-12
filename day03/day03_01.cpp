#include <iostream>
#include <fstream>
#include <unordered_set>
#include <set>
#include <string>
#include <numeric>
#include <execution>

int item_type_to_priority(char c)
{
  int value = (int)c;
  bool is_lowercase = value > 96;
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

  auto cmp = [](char a, char b)
  {
    int a_priority = item_type_to_priority(a);
    int b_priority = item_type_to_priority(b);
    return a_priority < b_priority;
  };
  std::multiset<char, decltype(cmp)> intersection = std::multiset<char, decltype(cmp)>(cmp);
  while (std::getline(file_handle, current_line))
  {
    size_t line_length = current_line.length();

    // Split string into two halves
    std::string first_half = current_line.substr(0, line_length / 2);
    std::string second_half = current_line.substr(line_length / 2, line_length);

    // Store each character into their half's set
    std::unordered_set<char> first_half_set = std::unordered_set<char>();
    std::unordered_set<char> second_half_set = std::unordered_set<char>();
    for (char c : first_half)
    {
      first_half_set.insert(c);
    }
    for (char c : second_half)
    {
      second_half_set.insert(c);
    }

    std::cout << first_half << "\n"
              << second_half << "\n\n";

    // If an item in the first half is also in the second half, then include them in the intersection
    for (const char c : first_half_set)
    {
      if (second_half_set.find(c) != second_half_set.end())
      {
        intersection.insert(c);
      }
    }
  }

  std::cout << "Intersection:\t";
  for (const char c : intersection)
  {
    std::cout << c << ": " << item_type_to_priority(c) << "\t";
  }
  std::cout << "\n";

  auto priority_sum_eval = [](char a, char b)
  {
    std::cout << "b:\t" << item_type_to_priority(a) << "\n";
    return item_type_to_priority(a) + item_type_to_priority(b);
  };

  int priority_sum = 0;

  for (auto i = intersection.begin(); i != intersection.end(); i++)
  {
    priority_sum += item_type_to_priority(*i);
  }

  std::cout << "sum:\t" << priority_sum;
  return 0;
}