//-------------------------------------------------------------------------------------------------
// Day 13: Distress Signal
// by Rene Jotham C. Culaway
//
// Determine which pairs of packets are already in the right order.
// What is the sum of the indices of those pairs?
//
// Parsing. AGAIN. I HATE PARSING! It's especially not fun in C++!
// There is also sorting involved, but I won't get into that for now.
//-------------------------------------------------------------------------------------------------

#include "day13.hpp"

const std::string FILE_NAME = "input.txt";
const std::string SAMPLE = "input_sample.txt";
const std::vector<packet> divider_packets{
    "[[2]]",
    "[[6]]"};

int main(int argc, char *argv[])
{

  auto packets = load_sorted_packets_from_file(FILE_NAME);     // Load the packets, already sorted.
  with_divider_packets(divider_packets, packets);              // Add the divider packets.
  int decoder_key = get_decoder_key(divider_packets, packets); // Compute the decoder key.
  for (auto &packet : packets)
  {
    std::cout << packet << std::endl;
  }

  std::cout << "decoder key: " << decoder_key << std::endl;
  // auto correct_packets = process_packets(packets);
  // for (auto &i : correct_packets)
  // {
  //   std::cout << (i ? "true" : "false") << std::endl;
  // }
  // std::cout << "sum of indices: " << sum_of_indices(correct_packets) << std::endl;
  return 0;
}