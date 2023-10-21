#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <numeric>
#include <string>
#include <sstream>

/// @brief A structure representing a rope segment.
struct RopeSegment
{
  std::string display = "R";
  std::pair<int, int> position = std::pair(0, 0);

public:
  int priority = 0; // The "z-index" of this rope segment.
  RopeSegment(std::string display, std::pair<int, int> position, int priority = 0) : display(display), position(position), priority(priority)
  {
  }
};

/// @brief A class representing a Bridge with a rope.
class Bridge
{
private:
  std::pair<int, int> min_coordinates = std::pair(0, 0); // These values are used for printing.
  std::pair<int, int> max_coordinates = std::pair(0, 0);

  std::vector<RopeSegment> rope; // A rope is simply a connected series of rope segments.

  std::set<std::pair<int, int>> tail_history; // The coordinates where the tail has passed through. Initially contains the starting position.

  /// @brief Checks if two vectors have a similar axis.
  /// @param from The first vector.
  /// @param from The second vector.
  /// @return Their difference.
  std::pair<int, int> relative_to_coordinate(std::pair<int, int> from, const std::pair<int, int> &to)
  {
    from.first -= to.first;
    from.second -= to.second;
    return from;
  }

  /// @brief Checks if two vectors have a similar axis.
  /// @param relative_vec The difference of the two vectors.
  /// @return `true` if they lie on the same axis, `false` if not.
  bool is_same_in_a_dimension(const std::pair<int, int> &relative_vec)
  {
    return relative_vec.first == 0 || relative_vec.second == 0;
  }

  /// @brief Selects the component of a vector that is lower.
  /// @param vec2 The vector to select.
  /// @return A pointer to the lesser magnitude element.
  int *get_lesser_magnitude_dimension(std::pair<int, int> &vec)
  {
    if (std::abs(vec.first) < std::abs(vec.second))
      return &(vec.first);
    return &(vec.second);
  }

  /// @brief Progresses the bridge to the next state.
  void next_state()
  {

    // A rope segment's next state is based on the rope segment before it.
    for (auto i = rope.begin() + 1; i != rope.end(); i++)
    {
      RopeSegment &from = *(i - 1);
      RopeSegment &to = *i;
      std::pair relative_position = relative_to_coordinate(from.position, to.position);
      int distance = std::sqrt(std::pow(relative_position.first, 2) + std::pow(relative_position.second, 2));
      if (distance > 1)
      {
        if (!is_same_in_a_dimension(relative_position)) // If the segment is in a diagonal from the previous segment, move it such that it now lies on the same axis.
        {
          auto smaller_component = get_lesser_magnitude_dimension(relative_position);
          *smaller_component = signum(*smaller_component) * (std::abs(*smaller_component) + 1);
        }
        to.position.first += relative_position.first / distance;
        to.position.second += relative_position.second / distance;
      }
    }
  }

public:
  void print_bridge(std::ostream &out = std::cout)
  {
    // out << head.position.first << ", " << head.position.second << "\n";
    for (int i = min_coordinates.second; i <= max_coordinates.second; i++)
    {
      for (int j = min_coordinates.first; j <= max_coordinates.first; j++)
      {
        char to_output = '.';
        int max_priority = -1;
        for (auto segment = rope.cbegin(); segment != rope.cend(); segment++)
        {
          auto &position = segment->position;
          auto &priority = segment->priority;
          if (position.first == j && position.second == i && priority > max_priority)
          {
            max_priority = priority;
            if (segment == rope.end() - 1)
            {
              to_output = 'T';
            }
            else if (segment == rope.begin())
            {
              to_output = 'H';
            }
            else
            {
              to_output = priority + '0';
            }
          }
        }

        out << to_output;
      }
      out << std::endl;
    }
  }

  void print_visited(std::ostream &out)
  {
    // std::cout << head.position.first << ", " << head.position.second << "\n";
    for (int i = min_coordinates.second; i <= max_coordinates.second; i++)
    {
      for (int j = min_coordinates.first; j <= max_coordinates.first; j++)
      {
        if (tail_history.find(std::pair<int, int>(j, i)) != tail_history.end())
        {
          out << "#";
        }
        else
        {
          out << ".";
        }
      }
      out << std::endl;
    }
  }

  Bridge(int num_segments = 2)
  {
    for (int i = 0; i < num_segments; i++)
    {
      rope.emplace_back(RopeSegment(std::to_string(i), std::pair(0, 0), num_segments - i - 1));
    }
    tail_history.insert(((*(rope.end() - 1)).position));
  }

  /// @brief Moves the head (the first element of `rope`) up.
  void move_head_up()
  {
    RopeSegment &head = *rope.begin();
    head.position.second -= 1;
    min_coordinates.second = std::min(head.position.second, min_coordinates.second);
    next_state();
    tail_history.insert(((*(rope.end() - 1)).position));
  }
  /// @brief Moves the head (the first element of `rope`) down.
  void move_head_down()
  {
    RopeSegment &head = *rope.begin();
    head.position.second += 1;
    max_coordinates.second = std::max(head.position.second, max_coordinates.second);
    next_state();
    tail_history.insert(((*(rope.end() - 1)).position));
  }
  /// @brief Moves the head (the first element of `rope`) to the left.
  void move_head_left()
  {
    RopeSegment &head = *rope.begin();
    head.position.first -= 1;
    min_coordinates.first = std::min(head.position.first, min_coordinates.first);
    next_state();
    tail_history.insert(((*(rope.end() - 1)).position));
  }
  /// @brief Moves the head (the first element of `rope`) to the right.
  void move_head_right()
  {
    RopeSegment &head = *rope.begin();
    head.position.first += 1;
    max_coordinates.first = std::max(head.position.first, max_coordinates.first);
    next_state();
    tail_history.insert(((*(rope.end() - 1)).position));
  }

  size_t count_positions_visited()
  {
    return tail_history.size();
  }
};

void parse_movement_from_file(const std::string &file_name, Bridge &bridge, std::ostream &out = std::cout)
{
  std::ifstream file_handle(file_name);

  if (!file_handle.is_open())
  {
    return;
  }

  std::string current_line;

  std::ostringstream temp_out;
  while (std::getline(file_handle, current_line))
  {
    char direction = current_line.at(0);
    int num_of_times = std::stoi(current_line.substr(2, current_line.length() - 2));

    // temp_out << "== " << direction << " " << num_of_times << " ==" << std::endl;
    switch (direction)
    {
    case 'U':
      for (int i = 0; i < num_of_times; i++)
      {
        bridge.move_head_up();
        // bridge.print_bridge(temp_out);
        // temp_out << std::endl;
      }
      break;
    case 'D':
      for (int i = 0; i < num_of_times; i++)
      {
        bridge.move_head_down();
        // bridge.print_bridge(temp_out);
        // temp_out << std::endl;
      }
      break;
    case 'L':
      for (int i = 0; i < num_of_times; i++)
      {
        bridge.move_head_left();
        // bridge.print_bridge(temp_out);
        // temp_out << std::endl;
      }
      break;
    case 'R':
      for (int i = 0; i < num_of_times; i++)
      {
        bridge.move_head_right();
        // bridge.print_bridge(temp_out);
        // temp_out << std::endl;
      }
      break;
    default:
      break;
    }
  }

  // out << temp_out.str();

  return;
}

/// @brief Gets the signum of an integer.
int signum(const int &magnitude)
{
  if (magnitude == 0)
    return 0;
  if (magnitude < 0)
    return -1;
  return 1;
}