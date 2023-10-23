//-------------------------------------------------------------------------------------------------
// Day 12: Hill Climbing Algorithm
// by Rene Jotham C. Culaway
//
// What is the fewest steps required to move from your current position to the location
// that should get the best signal?
//
// Now why does this scream A*? UPDATE: Yes it is indeed A*. Thanks to
// https://www.redblobgames.com/pathfinding/a-star/introduction.html for the refresher on A*.
//-------------------------------------------------------------------------------------------------

#include "day12.hpp"

const std::string FILE_NAME = "input.txt";

int main(int argc, char *argv[])
{
  HillClimber hill_climber(FILE_NAME);
  hill_climber.print_map(&std::cout);
  auto path = hill_climber.get_path();

  // while (!path.empty())
  // {
  //   std::cout << path.top().first << ", " << path.top().second << std::endl;
  //   path.pop();
  // }
  hill_climber.print_path_map(hill_climber.get_scenic_path(), &std::ofstream("output.txt"));
  return 0;
}
