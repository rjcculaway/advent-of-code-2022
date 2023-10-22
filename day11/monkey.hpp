#pragma once
#include "monkey.fwd.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include "../utils/utils.hpp"

/// @brief A class representing a Monkey.
class Monkey
{
  int id;
  unsigned long long activity = 0;                         // How many times this monkey has looked through an item.
  std::vector<unsigned long long> items;                   // The items (or their worry levels) that the monkey holds.
  const std::function<void(unsigned long long &)> inspect; // Function that modifies an item's worry level.
  unsigned long long divisor;                              // This monkey's divisor.

  const std::pair<int, int> other_monkeys; // The indices of the monkeys which it can throw items to.

  // Reduces the item's worry value.
  void relieve_worry(unsigned long long &item, const unsigned long long &worry_divisor)
  {
    item = item % worry_divisor;
  }

public:
  Monkey(int id,
         const std::function<void(unsigned long long &)> &inspect,
         unsigned long long divisor,
         std::pair<int, int> other_monkeys,
         std::vector<unsigned long long> starting_items)
      : id(id), inspect(inspect), divisor(divisor), other_monkeys(other_monkeys), items{starting_items}
  {
  }

  void print_items(std::ostream *out = nullptr)
  {
    for (const auto &i : items)
    {
      *out << i << ", ";
    }
    *out << "\n";
  }

  int get_id()
  {
    return id;
  }

  const std::pair<int, int> &get_other_monkeys()
  {
    return other_monkeys;
  }

  void go_through_items(MonkeyParty *monkeyparty, const unsigned long long &worry_divisor, std::ostream *out = nullptr);

  /// @brief Receives an item
  /// @param item The item's worry level.
  void receive_item(unsigned long long &item)
  {
    items.emplace_back(item);
  }

  unsigned long long get_activity()
  {
    return activity;
  }
};

/// @brief A class representing the interactions of the monkeys.
class MonkeyParty
{
  unsigned long long worry_divisor = 1;
  std::vector<Monkey> monkeys;

public:
  /// @brief Simulates the playing of the monkeys.
  /// @param rounds Number of rounds the monkeys will play.
  /// @param out Where to output the current state of the party.
  /// @return The product of the two highest activities.
  unsigned long long commence_party(unsigned long long rounds = 20, std::ostream *out = nullptr)
  {
    utils::Heap<unsigned long long> heap(monkeys.size());
    for (auto i = 0; i < rounds; i++)
    {
      // if (out != nullptr)
      // {
      //   *out << "Round " << i + 1 << ": " << std::endl;
      // }
      for (auto &monkey : monkeys)
      {
        monkey.go_through_items(this, worry_divisor, out);
      }
      // if (out != nullptr)
      // {
      //   for (auto &monkey : monkeys)
      //   {
      //     *out << "Monkey " << monkey.get_id() << ": ";
      //     monkey.print_items(out);
      //   }
      //   *out << std::endl;
      // }
    }
    for (auto &monkey : monkeys)
    {
      // *out << "Monkey " << monkey.get_id() << " inspected items " << monkey.get_activity() << " times." << std::endl;
      heap.insert(monkey.get_activity());
    }
    return heap.get_max() * heap.get_max();
  }

  /// @brief Sends an item (worry value) to a monkey.
  /// @param target The index of the monkey to send to.
  /// @param item The item to send.
  void throw_item_to_monkey(int &target, unsigned long long &item)
  {
    monkeys[target].receive_item(item);
  }

  /// @brief Preps the party from a file.
  /// @param file_name The file to read the input.
  void parse_party(const std::string &file_name)
  {
    worry_divisor = 1; // Reset the worry divisor

    // Constants for parsing through each line.
    const int STARTING_ITEMS_START = 17;
    const int OPERATION_START = 22;
    const int TEST_START = 20;
    const int OTHER_MONKEY_1_START = 28;
    const int OTHER_MONKEY_2_START = 29;

    std::ifstream file_handle(file_name);

    if (!file_handle.is_open())
    {
      return;
    }

    std::string current_line;

    while (true)
    {
      // Get the monkey's ID
      if (!std::getline(file_handle, current_line))
      {
        break;
      }
      int monkey_id = std::stoi(utils::split(current_line, " ")[1]);

      // Get the monkey's starting items
      if (!std::getline(file_handle, current_line))
      {
        break;
      }
      std::vector<unsigned long long> monkey_starting_items;
      std::vector<std::string> starting_items_str = utils::split(current_line.substr(STARTING_ITEMS_START), ", ");
      std::transform(starting_items_str.cbegin(), starting_items_str.cend(), std::back_inserter(monkey_starting_items), [](std::string str)
                     { return std::stoi(str); });

      // Get the operation for each item
      if (!std::getline(file_handle, current_line))
      {
        break;
      }
      std::vector<std::string> operation_tokens = utils::split(current_line.substr(OPERATION_START), " ");

      char &monkey_operator = operation_tokens[0].at(0);
      std::string &monkey_operand = operation_tokens[1];

      std::function<void(unsigned long long &)> operation;
      try
      {
        unsigned long long operation_value = std::stoi(monkey_operand);
        switch (monkey_operator)
        {
        case '*':
          operation = [operation_value](unsigned long long &worry)
          { worry = worry * operation_value; };
          break;
        case '+':
          operation = [operation_value](unsigned long long &worry)
          { worry = worry + operation_value; };
          break;
        }
      }
      catch (std::invalid_argument)
      {
        if (monkey_operand == "old")
        {
          switch (monkey_operator)
          {
          case '*':
            operation = [](unsigned long long &worry)
            { worry *= worry; };
            break;
          case '+':
            operation = [](unsigned long long &worry)
            { worry += worry; };
            break;
          }
        }
      }
      // Get the test
      if (!std::getline(file_handle, current_line))
      {
        break;
      }
      unsigned long long divisor = std::stoi(current_line.substr(TEST_START));
      worry_divisor *= divisor;

      // Get the monkeys to throw to
      if (!std::getline(file_handle, current_line))
      {
        break;
      }
      int other_monkey_first = std::stoi(current_line.substr(OTHER_MONKEY_1_START));
      if (!std::getline(file_handle, current_line))
      {
        break;
      }
      int other_monkey_second = std::stoi(current_line.substr(OTHER_MONKEY_2_START));

      std::pair<int, int> other_monkeys(other_monkey_first, other_monkey_second);

      monkeys.emplace_back(Monkey(monkey_id, operation, divisor, other_monkeys, monkey_starting_items));

      std::getline(file_handle, current_line);
    }

    std::cout << "divisor: " << worry_divisor << std::endl;
    return;
  }
};

/// @brief Go through each item the monkey has.
/// @param monkeyparty The monkey party the monkey is part of.
/// @param worry_divisor The worry divisor.
/// @param out Where to output the current state of the monkey.
void Monkey::go_through_items(MonkeyParty *monkeyparty, const unsigned long long &worry_divisor, std::ostream *out)
{
  while (!items.empty())
  {
    auto item = items.front();
    // if (out != nullptr)
    // {
    //   *out << "\tMonkey inspects an item with a worry level of " << item << "." << std::endl;
    // }
    inspect(item);
    // if (out != nullptr)
    // {
    //   *out << "\t\tWorry level changes to " << item << "." << std::endl;
    // }
    relieve_worry(item, worry_divisor);
    // if (out != nullptr)
    // {
    //   *out << "\t\tMonkey gets bored with item. Worry level is divided by " << worry_divisor << " to " << item << "." << std::endl;
    // }
    const int *which_monkey_to_throw_to = item % divisor == 0 ? &other_monkeys.first : &other_monkeys.second;
    int monkey_to_throw_to = *which_monkey_to_throw_to;
    // if (out != nullptr)
    // {
    //   *out << "\t\tItem with worry level " << item << " is thrown to monkey " << monkey_to_throw_to << "." << std::endl;
    // }
    (*monkeyparty).throw_item_to_monkey(monkey_to_throw_to, item);
    items.erase(items.begin());
    ++activity;
  }
}
