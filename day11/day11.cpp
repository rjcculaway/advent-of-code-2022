//-------------------------------------------------------------------------------------------------
// Day 10: Monkey in the Middle
// by Rene Jotham C. Culaway
//
// Figure out which monkeys to chase by counting how many items they inspect over 20 rounds.
// What is the level of monkey business after 20 rounds of stuff-slinging simian shenanigans?
//
// Aside from learning more about the size of data types... WHAT THE HELL IS THE CHINESE REMAINDER
// THEOREM?????? I didn't get the solution for Part 2 on my own, so thanks to James Crawford for
// the tip: https://jactl.io/blog/2023/04/17/advent-of-code-2022-day11.html. Basically, if the
// divisors have no other common factor other than 1, then (n % (p * q)) % q == n % q, where p
// and q are divisors.
//-------------------------------------------------------------------------------------------------

#include "day11.hpp"

const int NUMBER_OF_ROUNDS = 10000;
const std::string FILE_NAME = "input.txt";

int main(int argc, char *argv[])
{
  MonkeyParty monkeyparty;
  monkeyparty.parse_party(FILE_NAME);

  std::cout << "The level of monkey business is: " << monkeyparty.commence_party(NUMBER_OF_ROUNDS, &std::ofstream("output.txt")) << std::endl;
  return 0;
}