#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <functional>

/// @brief A class representing a Command. I tried to implement here a (crude) version of the Command pattern.
class Command
{
protected:
  int cycles = 1;

public:
  /// @brief Checks if this command is done.
  /// @return `true` if it is done, `false` if not.
  bool is_done()
  {
    return cycles == 0;
  }
  /// @brief Moves progress of the command.
  void progress()
  {
    cycles--;
  }

  /// @brief The function to run once the command completes.
  std::function<void()> command_effect;

  /// @brief Gets the number of cycles needed to complete this command.
  /// @return The number of cycles.
  int get_cycles()
  {
    return cycles;
  }

  Command(std::function<void()> command_effect, int cycles) : command_effect(command_effect), cycles(cycles)
  {
  }
};

/// @brief A class simulating how a CRT screen works, a la Atari.
class CathodeRayTube
{
private:
  int current_cycle = 1;
  std::queue<Command> command_queue;

  /// Screen constants.
  const int width = 40;
  const int height = 6;
  const int sprite_width = 3;

public:
  int _register = 1;

  /// @brief Moves progress (clock cycle) of the "CRT". Progresses the current command being executed. When it is done, it gets executed and is removed from `command_queue`.
  void progress()
  {
    Command &current_command = command_queue.front();
    current_command.progress();
    if (current_command.is_done())
    {
      current_command.command_effect();
      command_queue.pop();
      current_command = command_queue.front();
    }
    current_cycle++;
    return;
  }

  /// @brief Interprets a vector of tokens and selects which command it is related to.
  /// @param tokens The tokens representing the command.
  void serialize_and_enqueue_command(std::vector<std::string> &tokens)
  {
    std::string command_token = *tokens.begin();

    std::function<void()> effect;
    int cycles = 1;
    // Select between the two operations possible for the CRT.
    if (command_token == "noop") // No operation.
    {
      effect = [&]()
      {
        return;
      };
    }
    else if (command_token == "addx") // Adds x to the register.
    {
      cycles = 2;
      std::string operand_token = *(tokens.begin() + 1);
      effect = [&, operand_token]()
      {
        _register += std::stoi(operand_token);
        return;
      };
    }
    command_queue.push(Command(effect, cycles));
  }

  int get_current_cycle()
  {
    return current_cycle;
  }

  /// @brief The signal strength is computed by multiplying the current cycle and the current value of the register.
  /// @return The signal strength at the current cycle.
  int get_signal_strength()
  {
    return current_cycle * _register;
  }

  /// @brief Checks if the CRT is still processing commands.
  /// @return `true` if the CRT can still process, `false` if not.
  bool has_commands()
  {
    return command_queue.size() > 0;
  }

  /// @brief Runs the CRT with the commands it is given.
  /// @param out Where the result will be drawn.
  /// @return The cumulative signal strength of the CRT.
  int draw_screen(std::ostream &out = std::cout)
  {
    int combined_signal_strength = 0;
    bool is_lit = false;

    int column = 0; // x
    do
    {
      column = (get_current_cycle() - 1) % width;
      is_lit = column <= _register + sprite_width / 2 && column >= _register - sprite_width / 2;
      if (is_lit)
      {
        out << "#";
      }
      else
      {
        out << ".";
      }
      if (column == width - 1) // After the width, we move to the next row
      {
        out << std::endl;
      }
      if ((get_current_cycle() - 20) % width == 0)
      {
        // out << "Signal strength at " << get_current_cycle() << " is:\t" << get_signal_strength() << "(" << _register << ")" << std::endl;
        combined_signal_strength += get_signal_strength();
      }
      progress();
    } while (has_commands());
    return combined_signal_strength;
  }
};

/// @brief Splits a string by a delimiter.
/// @param str The string to split.
/// @param delim The delimiter by which to split the string.
/// @return A `vector` of tokens split by the delimiter.
std::vector<std::string> split(const std::string &str, const std::string &delim)
{
  std::vector<std::string> tokens;

  size_t start = 0, end = 0;
  while (start < str.length())
  {
    end = str.find(delim, start);
    if (end == std::string::npos)
    {
      std::string right = str.substr(start, str.length() - start);
      if (right.length() > 0)
      {
        tokens.emplace_back(right);
        break;
      }
    }

    std::string left = str.substr(start, end - start);
    start = end + 1;

    if (left.length() > 0)
    {
      tokens.emplace_back(left);
    }
  }

  return tokens;
}

/// @brief Reads a file containing the deserialized commands and enqueues them to the CRT.
/// @param file_name The name of the file to be read.
/// @param crt The CRT.
void parse_commands(const char *&file_name, CathodeRayTube &crt)
{
  std::ifstream file_handle(file_name);
  if (!file_handle.is_open())
  {
    return;
  }

  std::string current_line;

  while (std::getline(file_handle, current_line))
  {
    std::vector<std::string> tokens = split(current_line, " ");
    const std::string &command = *tokens.cbegin();
    crt.serialize_and_enqueue_command(tokens);
  }

  file_handle.close();
  return;
}