#pragma once
#include "../utils/utils.hpp"
#include <fstream>
#include <string>
#include <memory>
#include <queue>
#include <map>
#include <stack>
#include <functional>
#include <set>

/// @brief Type alias for this one cuz it gets tiring typing this one out.
using Coordinate = std::pair<size_t, size_t>;

/// @brief For printing.
enum MapLegend
{
  UNVISITED = '.',
  START = 'S',
  END = 'E',
  UP = '^',
  DOWN = 'v',
  LEFT = '<',
  RIGHT = '>'
};

/// @brief Converts a map legend to its corresponding "vector".
/// @param map_legend The map legend.
/// @return The corresponding vector.
std::pair<int, int> map_legend_to_vector(const MapLegend map_legend)
{
  switch (map_legend)
  {
  case UP:
    return std::pair(0, -1);
  case DOWN:
    return std::pair(0, 1);
  case LEFT:
    return std::pair(-1, 0);
  case RIGHT:
    return std::pair(1, 0);
  default:
    return std::pair(0, 0);
  }
}

/// @brief Class representing the trek.
class HillClimber
{
  std::vector<std::string> map;
  size_t width;
  size_t height;
  Coordinate start; // The starting point for the climber.
  Coordinate end;

private:
  /// @brief Computes the cost of climbing to this terrain.
  /// @param position The position to compute the cost to.
  /// @return The cost.
  int compute_cost(const Coordinate &position)
  {
    char terrain = map[position.second].at(position.first);

    int cost = 0;
    switch (terrain)
    {
    case 'S':
      cost = 'a' - 'a' + 1; // 1
      break;
    case 'E':
      cost = 'z' - 'a' + 1; // 26
      break;
    default:
      cost = terrain - 'a' + 1;
      break;
    }

    return cost;
  }

  /// @brief Lists valid positions for the searching algorithm to move to.
  /// @param current_position The current position.
  /// @return A vector of valid adjacent coordinates.
  std::vector<Coordinate> enumerate_valid_adjacents(const Coordinate &current_position)
  {
    std::vector<Coordinate> adjacent;

    auto x = current_position.first;
    auto y = current_position.second;

    if (y > 0)
    {
      auto next_position = Coordinate(x, y - 1);
      int delta = compute_cost(next_position) - compute_cost(current_position); // The change in elevation must be at most 1.
      if (delta <= 1)
      {
        adjacent.emplace_back(next_position);
      }
    }

    if (y < height - 1)
    {
      auto next_position = Coordinate(x, y + 1);
      int delta = compute_cost(next_position) - compute_cost(current_position);
      if (delta <= 1)
      {
        adjacent.emplace_back(next_position);
      }
    }

    if (x > 0)
    {
      auto next_position = Coordinate(x - 1, y);
      int delta = compute_cost(next_position) - compute_cost(current_position);
      if (delta <= 1)
      {
        adjacent.emplace_back(next_position);
      }
    }

    if (x < width - 1)
    {
      auto next_position = Coordinate(x + 1, y);
      int delta = compute_cost(next_position) - compute_cost(current_position);
      if (delta <= 1)
      {
        adjacent.emplace_back(next_position);
      }
    }

    return adjacent;
  }
  /// @brief Gets the MapLegend corresponding to the direction.
  /// @param c1 The first coordinate.
  /// @param c2 The second coordinate.
  /// @return The relative direction of `c1` to `c2`.
  MapLegend get_direction(const Coordinate &c1, const Coordinate &c2)
  {
    int offset_x = c1.first - c2.first;
    int offset_y = c1.second - c2.second;

    // Assume diagonal movement is impossible.
    switch (offset_x)
    {
    case -1:
      return LEFT;
    case 1:
      return RIGHT;
    }

    switch (offset_y)
    {
    case -1:
      return UP;
    case 1:
      return DOWN;
    }

    return UNVISITED;
  }

  /// @brief Checks if a coordinate is the end goal.
  /// @param position The coordinate.
  /// @return `true` if it is the end goal. `false` if not.
  bool is_goal(const Coordinate &position)
  {
    return (MapLegend)map[position.second].at(position.first) == MapLegend::END;
  }

  /// @brief Used for the A* algorithm. Computes the Manhattan distance between two points.
  /// @param c1 The first coordinate.
  /// @param c2 The second coordinate.
  /// @return The Manhattan distance between `c1` and `c2`.
  int heuristic_distance(const Coordinate &c1, const Coordinate &c2)
  {
    return std::abs((int)c1.first - (int)c2.first) + std::abs((int)c1.second - (int)c2.second);
  }

public:
  /// @brief Reads an input file.
  HillClimber(const std::string &file_name)
  {
    std::ifstream file_handle(file_name);

    if (!file_handle.is_open())
    {
      return;
    }

    std::string current_line;
    while (std::getline(file_handle, current_line))
    {
      map.emplace_back(current_line);
    }

    file_handle.close();

    height = map.size();
    width = map[0].size();

    for (size_t i = 0; i < height; i++)
    {
      for (size_t j = 0; j < width; j++)
      {
        char altitude = map[i].at(j);
        switch (altitude)
        {
        case 'S':
          start = std::pair(j, i);
          break;
        case 'E':
          end = std::pair(j, i);
          break;
        default:
          break;
        }
      }
    }

    return;
  }

  /// @brief Prints out the read map.
  /// @param out Where to output the map.
  void print_map(std::ostream *out)
  {
    for (auto &row : map)
    {
      *out << row << std::endl;
    }
  }

  /// @brief Prints a path.
  /// @param path The path to printm starting from the start location.
  /// @param out
  void print_path_map(std::stack<Coordinate> path, std::ostream *out)
  {
    std::vector<MapLegend> path_map(width * height, MapLegend::UNVISITED);
    path_map[utils::index_2d_to_1d(start.second, start.first, width)] = MapLegend::START;
    auto previous_coordinate = start;

    *out << "Path length: " << path.size() << std::endl;

    while (!path.empty())
    {
      auto coordinate = path.top();
      // std::cout << "from "
      //           << "[" << previous_coordinate.second << ", " << previous_coordinate.first << "]"
      //           << " to "
      //           << "[" << coordinate.second << ", " << coordinate.first << "]" << std::endl;
      path_map[utils::index_2d_to_1d(previous_coordinate.second, previous_coordinate.first, width)] = get_direction(coordinate, previous_coordinate);
      path.pop();
      previous_coordinate = coordinate;
    }

    for (size_t i = 0; i < height; i++)
    {
      for (size_t j = 0; j < width; j++)
      {
        *out << (char)path_map[utils::index_2d_to_1d(i, j, width)];
      }
      *out << std::endl;
    }
  }

  std::stack<Coordinate> get_path()
  {
    return get_path(start);
  }

  /// @brief Computes the shortest path from a set location to the end. A modified implementation of the A* algorithm.
  /// @param starting_position The starting position. Default is the input-file-specified start.
  /// @return A stack of coordinates to move to to get to the endpoint.
  std::stack<Coordinate> get_path(const Coordinate &starting_position)
  {
    std::map<Coordinate, Coordinate> from; // Stores the preceding position from a coordinate.
    std::map<Coordinate, int> costs;       // Stores the current minimum cost to get to a location.
    std::stack<Coordinate> path;           // The final path.

    // The top of the min-heap should have the minimum sum between the heuristic and the ongoing cost.
    auto compare = [&](const Coordinate &left, const Coordinate &right)
    { return heuristic_distance(end, left) + costs[left] > heuristic_distance(end, right) + costs[right]; };
    std::priority_queue<Coordinate, std::vector<Coordinate>, decltype(compare)> frontier(compare); // Underlying priority queue for coordinates.

    // Start with our first coordinate.
    frontier.push(starting_position);
    from[starting_position] = starting_position;
    costs[starting_position] = 0;

    while (!frontier.empty())
    {
      auto current_coordinate = frontier.top();
      frontier.pop();

      if (is_goal(current_coordinate))
      {
        Coordinate current_path = current_coordinate;
        while (current_path != starting_position)
        {
          // Build the path
          path.push(current_path);
          current_path = from[current_path];
        }

        break;
      }

      for (auto next : enumerate_valid_adjacents(current_coordinate))
      {
        int next_cost = costs[current_coordinate] + compute_cost(next);

        bool is_cost_recorded = costs.find(next) != costs.end();
        if (!is_cost_recorded || next_cost < costs[next])
        {
          costs[next] = next_cost;
          frontier.push(next);
          from[next] = current_coordinate;
        }
      }
    }
    return path;
  }

  /// @brief Gets the a starting point that results to the least amount of steps to the endpoint.
  /// @return The path that has the least steps from 'a'.
  std::stack<Coordinate> get_scenic_path()
  {
    std::stack<Coordinate> scenic_path;
    bool scenic_path_set = false;
    for (size_t i = 0; i < height; i++)
    {
      for (size_t j = 0; j < width; j++)
      {
        if (compute_cost(Coordinate(j, i)) == 1)
        {
          auto path = get_path(Coordinate(j, i));
          scenic_path = !scenic_path_set || (!path.empty() && path.size() < scenic_path.size()) ? path : scenic_path;
          scenic_path_set = true;
        }
      }
    }
    return scenic_path;
  }
};