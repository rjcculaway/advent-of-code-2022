//-------------------------------------------------------------------------------------------------
// Day 06 Part 01 and Part 02: Tuning Trouble
// by Rene Jotham C. Culaway
//
// How many characters need to be processed before the first start-of-packet marker is detected?
//-------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <set>

const char *FILE_NAME = "input.txt";
const int WINDOW_SIZE = 4;
const int WINDOW_SIZE_2 = 14;

/// @brief Custom class representing a queue
class Queue
{
private:
  size_t max_length;
  size_t length = 0;

public:
  char *array;
  Queue(size_t max_length) : max_length(max_length)
  {
    array = new char[max_length];
  }

  ~Queue()
  {
    delete array;
  }

  size_t get_length()
  {
    return length;
  }

  size_t get_max_length()
  {
    return max_length;
  }

  /// @brief Checks if the queue is full.
  /// @return `true` if it is full, `false` if it is not.
  bool is_full()
  {
    return length >= max_length;
  }

  /// @brief Checks if the queue is empty.
  /// @return `true` if it is empty, `false` if it is not.
  bool is_empty()
  {
    return length < max_length;
  }

  /// @brief Enqueues the next value into the queue.
  /// @param val The value to enqueue.
  void enqueue(char val)
  {
    if (is_full())
    {
      throw std::overflow_error("Queue is full.");
    }
    array[length++] = val;
    return;
  }

  /// @brief Removes the latest value into the queue.
  /// @return The most latest value in the queue.
  char dequeue()
  {
    if (is_empty())
    {
      throw std::underflow_error("Queue is empty.");
    }
    char first = array[0];
    std::memmove(array, array + 1, length - 1); // Shift the queue to the left
    length--;
    return first;
  }

  void print_queue()
  {
    std::cout << "[";
    for (size_t i = 0; i < length; i++)
    {
      std::cout << array[i];
      if (i < length - 1)
      {
        std::cout << ", ";
      }
    }
    std::cout << "]\n";
  }
};

/// @brief From an input file, find the index where characters of a window (with size `window_size`) are all unique.
/// @param file_name The name of the file to load.
/// @param window_size The window size.
/// @return The starting index of the string where the start packet begins.
unsigned int find_first_start_of_packet_from_file(const char *file_name, const int &window_size)
{
  unsigned int idx = 0;
  std::ifstream file_handle(file_name);
  Queue buffer(window_size);

  if (file_handle.bad())
  {
    file_handle.close();
    return idx;
  }

  char current_char;
  while (file_handle >> current_char)
  {
    idx++;
    if (buffer.is_full())
    {
      buffer.dequeue();
    }
    buffer.enqueue(current_char);
    // buffer.print_queue();

    if (buffer.is_full())
    {
      std::set<char> chars;
      for (int i = 0; i < buffer.get_length(); i++)
      {
        chars.insert(buffer.array[i]);
      }
      if (chars.size() == window_size) // Sets only have unique elements, so if the set's size is the same as the window size, then all of the window's elements are unique.
      {
        break;
      }
    }
  }

  return idx;
}

int main(int argc, char *argv[])
{
  std::cout << find_first_start_of_packet_from_file(FILE_NAME, WINDOW_SIZE) << "\n";
  std::cout << find_first_start_of_packet_from_file(FILE_NAME, WINDOW_SIZE_2) << "\n";
  return 0;
}