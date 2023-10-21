//-------------------------------------------------------------------------------------------------
// Day 10: Cathode-Ray Tube
// by Rene Jotham C. Culaway
//
// Find the signal strength during the 20th, 60th, 100th, 140th, 180th, and 220th cycles.
// What is the sum of these six signal strengths?
//
// NGL, this one is the most fun day so far. Here I discovered circular references, but
// I just gave up on getting the compiler to cooperate (using forward declarations).
//-------------------------------------------------------------------------------------------------

#include "day10.hpp"

const char *FILE_NAME = "input.txt";

int main(int argc, char *argv[])
{
  CathodeRayTube crt;
  parse_commands(FILE_NAME, crt);
  int cumulative_signal_strength = crt.draw_screen(std::ofstream("output.txt"));
  std::cout << cumulative_signal_strength << std::endl;
  return 0;
}