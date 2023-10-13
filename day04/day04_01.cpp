//-------------------------------------------------------------------------------------------------
// Day 04 Part 01: Camp Cleanup
// by Rene Jotham C. Culaway
//
// In how many assignment pairs does one range fully contain the other?
//-------------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <string>

const char *FILE_NAME = "input.txt";

/// @brief Structure representing an inclusive nonnegative range.
struct Range
{
  int start, end;

  Range(int start, int end) : start(start), end(end)
  {
    if (start > end)
    {
      throw std::invalid_argument("Range start must be less than or equal to the range end.");
    }
  }

  int get_inclusive_range_length()
  {
    return end - start + 1;
  }

  bool operator==(Range b)
  {
    return start == b.start && end == b.end;
  }

  bool operator!=(Range b)
  {
    return !operator==(b);
  }

  /// @brief Get the intersection of two Ranges.
  /// @param b The other range
  /// @return A new range representing the intersection of the two ranges. If the ranges do not intersect, the start and end will be -1.
  Range operator&(Range b)
  {
    if (!operator&&(b))
    {
      return Range(-1, -1);
    }

    Range intersection = Range(std::max(start, b.start), std::min(end, b.end));

    if (intersection.end < intersection.start)
    {
      throw std::exception("Invalid start and end.");
    }
    return intersection;
  }

  /// @brief Checks if two Ranges intersect.
  /// @param b
  /// @return `true` if the Ranges intersect; `false` if not.
  bool operator&&(Range b)
  {
    return start > b.end || b.start > end;
  }

  /// @brief Checks if two ranges are fully contained. Two ranges are fully contained if their intersection is just the smaller or equal range.
  /// @param a The first range
  /// @param b The second range
  /// @return `true` if at least one of the ranges fully contain each other. `false` if not.
  static bool is_fully_contained(Range a, Range b)
  {
    Range smaller_range = a.get_inclusive_range_length() <= b.get_inclusive_range_length() ? a : b;
    return (a & b) == smaller_range;
  }
};

/// @brief Loads a file containing the pairs of ranges
/// @param file_name The file name
/// @return A vector containing the pairs of ranges for each elf pair.
std::vector<std::pair<Range, Range>> load_range_pairs_from_file(const char *file_name)
{
  const char pair_delimiter = ',';
  const char range_delimiter = '-';
  std::vector<std::pair<Range, Range>> ranges;
  std::ifstream file_handle;

  file_handle.open(FILE_NAME);

  if (!file_handle.good())
  {
    file_handle.close();
    return ranges;
  }

  std::string current_line;
  while (std::getline(file_handle, current_line))
  {
    // Split the string by the comma to get each range
    size_t pair_delimiter_location = current_line.find_first_of(pair_delimiter);
    std::string first_range_str = current_line.substr(0, pair_delimiter_location);
    std::string second_range_str = current_line.substr(pair_delimiter_location + 1, current_line.length());

    // Split each range string by the '-' to get each number
    size_t first_range_delimiter_location = first_range_str.find_first_of(range_delimiter);
    int first_range_start = std::stoi(first_range_str.substr(0, first_range_delimiter_location));
    int first_range_end = std::stoi(first_range_str.substr(first_range_delimiter_location + 1, first_range_str.length()));

    size_t second_range_delimiter_location = second_range_str.find_first_of(range_delimiter);
    int second_range_start = std::stoi(second_range_str.substr(0, second_range_delimiter_location));
    int second_range_end = std::stoi(second_range_str.substr(second_range_delimiter_location + 1, second_range_str.length()));

    ranges.emplace_back(std::pair<Range, Range>(Range(first_range_start, first_range_end), Range(second_range_start, second_range_end)));
  }

  return ranges;
}

/// @brief Main driver function for counting the fully contained pairs in an input file.
/// @return The number of fully contained pairs.
unsigned int count_fully_contained_pairs()
{
  unsigned int count = 0;

  std::vector<std::pair<Range, Range>> pairs = load_range_pairs_from_file(FILE_NAME);

  for (std::pair<Range, Range> pair : pairs)
  {
    Range first = pair.first;
    Range second = pair.second;
    Range intersection = first & second;
    /*
    std::cout << "<" << first.start << ", " << first.end << ">; "
              << "<" << second.start << ", " << second.end << ">"
              << "\t"
              << "->\t<" << intersection.start << ", " << intersection.end << ">\n";
    */

    if (Range::is_fully_contained(first, second))
      count++;
  }

  return count;
}

int main(int argc, char *argv[])
{
  unsigned int count = count_fully_contained_pairs();

  std::cout << "The fully contained pairs are " << count << ".\n";
  return 0;
}