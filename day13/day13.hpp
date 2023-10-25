#pragma once
#include <iostream>
#include <fstream>
#include <optional>
#include <set>
#include <functional>

#include "../utils/utils.hpp"

using packet = std::string; // Type alias to maintain my sanity

/// @brief Possible results for comparing each packet.
enum ComparisonResult
{
  EQUAL = '=',
  GREATER = '>',
  LESSER = '<'
};

/// @brief Gets the next subpacket.
/// @param pckt The packet.
/// @param start The (string) index to start from.
/// @return The start and end indices of the next subpacket. Optional type.
std::optional<std::pair<size_t, size_t>> get_next(const packet &pckt, size_t start) // The bracket of the list
{
  start += 1;
  if (start >= pckt.size())
  {
    return {};
  }

  // The subpacket is a nested list.
  // Determine the end of the list.
  if (pckt.at(start) == '[')
  {
    int num_brackets = 1;
    size_t end = start;
    while (num_brackets > 0)
    {
      end++;
      switch (pckt.at(end))
      {
      case '[':
        num_brackets++;
        break;
      case ']':
        num_brackets--;

        break;
      }
    }
    return std::pair<size_t, size_t>(start, end + 1);
  }

  // If the subpacket is a digit.
  if (std::isdigit(pckt.at(start)))
  {
    size_t end = start;
    while (std::isdigit(pckt.at(end)))
    {
      end++;
    }
    return std::pair<size_t, size_t>(start, end);
  }

  return {};
}

/// @brief Enumerates the subpackets of a (sub)packet.
/// @param packet The parent packet.
/// @return A vector containing the subpackets.
std::vector<packet> get_subpackets(const packet &packet)
{
  std::vector<std::string> subpackets;
  int i = 0;
  auto next = get_next(packet, i);
  while (next.has_value())
  {
    subpackets.emplace_back(packet.substr(next.value().first, next.value().second - next.value().first));
    i = next.value().second;
    next = get_next(packet, i);
  }
  return subpackets;
}

/// @brief Checks if a subpacket is yet another list.
/// @param subpacket The subpacket.
/// @return `true` if it is a list, `false` if not.
bool islist(const packet &subpacket)
{
  return subpacket.at(0) == '[';
}

/// @brief Compares two packets.
/// @param left The first packet.
/// @param right The second packet.
/// @return `GREATER` if the left packet is greater than the right packet. `LESSER` if the left packet is lesser than the right, and so on.
ComparisonResult compare_packets(const packet &left, const packet &right)
{
  // Check first if the compared (sub)packets are numbers.
  if (std::isdigit(left.at(0)) && std::isdigit(right.at(0)))
  {
    if (std::stoi(left) > std::stoi(right))
    {
      return GREATER;
    }

    if (std::stoi(left) < std::stoi(right))
    {
      return LESSER;
    }

    return EQUAL;
  }

  // If not, compare them as lists instead.
  if (islist(left) && islist(right))
  {
    auto left_subpackets = get_subpackets(left);
    auto right_subpackets = get_subpackets(right);

    size_t left_subpackets_size = left_subpackets.size();
    size_t right_subpackets_size = right_subpackets.size();

    for (size_t i = 0; i < std::min(left_subpackets_size, right_subpackets_size); i++)
    {
      auto left_subpacket = left_subpackets[i];
      auto right_subpacket = right_subpackets[i];
      ComparisonResult comparison = compare_packets(left_subpacket, right_subpacket);

      switch (comparison)
      {
      case EQUAL:
        break;
      default:
        return comparison;
      }
    }
    // If one of the lists become empty, then one of them is the greater packet.
    if (left_subpackets_size > right_subpackets_size)
    {
      return GREATER;
    }

    if (left_subpackets_size < right_subpackets_size)
    {
      return LESSER;
    }
  }

  // If only one is a list, convert the non-list into a list.
  if (islist(left) && !islist(right))
  {
    return compare_packets(left, "[" + right + "]");
  }

  if (!islist(left) && islist(right))
  {
    return compare_packets("[" + left + "]", right);
  }

  return EQUAL;
}

/// @brief Loads packets from a file.
/// @param file_name The file name.
/// @return A vector of packets.
std::vector<packet> load_packets_from_file(const std::string &file_name)
{
  std::vector<packet> packets;
  std::ifstream file_handle(file_name);

  if (!file_handle.is_open())
  {
    return packets;
  }

  int i = 0;
  packet current_line;
  while (std::getline(file_handle, current_line))
  {
    if (current_line.size() > 1)
      packets.emplace_back(current_line);
  }

  return packets;
}

/// @brief Loads packets from a file, sorted intrinsically due to the set.
/// @param file_name The file name.
/// @return The set of packets.
std::set<packet, std::function<bool(const packet &, const packet &)>> load_sorted_packets_from_file(const std::string &file_name)
{
  std::function<bool(const packet &, const packet &)> compare = [](const packet &left, const packet &right)
  { return compare_packets(left, right) == LESSER; };
  std::set<packet, decltype(compare)> packets(compare);
  std::ifstream file_handle(file_name);

  if (!file_handle.is_open())
  {
    return packets;
  }

  int i = 0;
  packet current_line;
  while (std::getline(file_handle, current_line))
  {
    if (current_line.size() > 1)
      packets.insert(current_line);
  }

  return packets;
}

/// @brief Inserts the divider packets into the set of packets.
/// @param divider_packets The divider packets to insert.
/// @param sorted_packets The sorted packets.
void with_divider_packets(const std::vector<packet> &divider_packets, std::set<packet, std::function<bool(const packet &, const packet &)>> &sorted_packets)
{
  for (auto &divider : divider_packets)
  {
    sorted_packets.insert(divider);
  }

  return;
}

/// @brief Computes the decoder key based in the indices of the divider packets.
/// @param divider_packets The divider packets.
/// @param sorted_packets The sorted packets.
/// @return The decoder key.
int get_decoder_key(const std::vector<packet> &divider_packets, std::set<packet, std::function<bool(const packet &, const packet &)>> &sorted_packets)
{
  int decoder_key = 1;
  auto vector_packets = std::vector<packet>(sorted_packets.cbegin(), sorted_packets.cend());

  for (size_t i = 0; i < vector_packets.size(); i++)
  {
    for (auto &divider : divider_packets)
    {
      if (divider == vector_packets[i])
        decoder_key *= i + 1;
    }
  }

  return decoder_key;
}

std::vector<bool> process_packets(std::vector<packet> packets)
{
  std::vector<bool> packet_pair_status(packets.size() / 2);
  int i = 0;
  for (i = 0; i < packets.size() / 2; i++)
  {
    auto comparison = compare_packets(packets[i * 2], packets[i * 2 + 1]);
    packet_pair_status[i] = comparison == GREATER ? false : true;
    // std::cout << packets[i * 2] << " vs. " << packets[i * 2 + 1] << ": " << (packet_pair_status[i] ? "true" : "false") << std::endl;
  }
  return packet_pair_status;
}

int sum_of_indices(const std::vector<bool> &correct_packets)
{
  int sum = 0;
  for (int i = 0; i < correct_packets.size(); i++)
  {
    if (correct_packets[i])
      sum += i + 1;
  }
  return sum;
}