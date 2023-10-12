#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

// Implements a vector-based max-heap.
// This is one-indexed.
class Heap
{
  std::vector<unsigned int> heap;

private:
  unsigned int max_size;
  unsigned int size;

  void swap(unsigned int i, unsigned int j)
  {
    unsigned int temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
    return;
  }

  bool is_root(unsigned int i)
  {
    return i == 1;
  }

  bool is_left_child(unsigned int i)
  {
    if (i > size)
    {
      throw std::out_of_range("Element exceeds heap size.");
    }

    return i % 2 == 0 && !is_root(i);
  }

  bool is_right_child(unsigned int i)
  {
    if (i > size)
    {
      throw std::out_of_range("Element exceeds heap size.");
    }

    return i % 2 == 1;
  }

  unsigned int get_parent_index(unsigned int i)
  {
    if (is_root(i))
    {
      throw std::invalid_argument("Current index has no parent (it is the root).");
    }

    if (i > size)
    {
      throw std::out_of_range("Element exceeds heap size.");
    }

    if (is_left_child(i))
    {
      return i / 2;
    }

    if (is_right_child(i))
    {
      return (i - 1) / 2;
    }

    return -1;
  }

  unsigned int get_left_child_index(unsigned int i)
  {
    unsigned int left_index = 2 * i;
    if (left_index > size)
    {
      throw std::out_of_range("Left child exceeds heap size.");
    }
    return left_index;
  }

  unsigned int get_right_child_index(unsigned int i)
  {
    unsigned int right_index = 2 * i + 1;
    if (right_index > size)
    {
      throw std::out_of_range("Right child exceeds heap size.");
    }
    return right_index;
  }

  void percolate_up(unsigned int i)
  {
    unsigned int current_index = i;
    while (!is_root(current_index))
    {
      unsigned int parent_index = get_parent_index(current_index);
      if (heap[current_index] < heap[parent_index])
      {
        break;
      }
      swap(parent_index, current_index);
      current_index = parent_index;
    }
    return;
  }

  void percolate_down(unsigned int i)
  {
    unsigned int current_index = i;
    while (current_index < size)
    {
      unsigned int left_child_index = 0;
      unsigned int right_child_index = 0;
      try
      {
        left_child_index = get_left_child_index(current_index);
      }
      catch (std::out_of_range &)
      {
        break;
      }

      try
      {
        right_child_index = get_right_child_index(current_index);
      }
      catch (std::out_of_range &)
      {
        break;
      }

      unsigned int larger_child_index = heap[left_child_index] > heap[right_child_index] ? left_child_index : right_child_index;

      if (heap[current_index] > heap[larger_child_index])
      {
        break;
      }
      swap(current_index, larger_child_index);
      current_index = larger_child_index;
    }
    return;
  }

public:
  Heap(unsigned int max_size) : max_size(max_size)
  {
    heap.reserve(max_size + 1); // Due to one-indexing
    size = 0;
  }

  bool is_full()
  {
    return size >= max_size;
  }

  bool is_empty()
  {
    return size <= 0;
  }

  void insert(const unsigned int new_value)
  {
    if (size >= max_size)
    {
      throw std::overflow_error("Heap is full. Maximum size: " + std::to_string(max_size) + "\n");
    }
    heap[++size] = new_value;
    percolate_up(size);
  }

  unsigned int get_max()
  {
    if (is_empty())
    {
      throw std::underflow_error("Heap is empty.");
    }
    swap(1, size);
    unsigned int max_value = heap[size--];
    percolate_down(1);
    return max_value;
  }

  void print_heap_helper(int index, int tabs)
  {
    if (index > size)
      return;
    else
    {
      print_heap_helper(index * 2 + 1, tabs + 1);
      for (int i = 0; i < tabs; i++)
        std::cout << "\t";
      std::cout << heap[index] << "\n";
      print_heap_helper(index * 2, tabs + 1);
    }
  }

  void print_heap()
  {
    if (size > 0)
    {
      print_heap_helper(1, 0);
    }
    else
    {
      std::cout << "Empty heap!\n";
    }
  }
};

int main(int argc, char *argv[])
{
  std::ifstream input_file;

  input_file.open("day01_input.txt");

  if (!input_file.good())
  {
    input_file.close();
    return 0;
  }

  std::string current_line;

  bool is_first_elf = true;
  Heap calories(1000);
  unsigned int current_index = 0;
  unsigned int current_elf_calorie = 0;
  unsigned int combined_calories = 0;
  while (std::getline(input_file, current_line))
  {
    // std::cout << current_line;
    if (current_line.size() <= 0)
    {
      // std::cout << "Inserting " << current_elf_calorie << "\n";
      calories.insert(current_elf_calorie);

      current_elf_calorie = 0;
    }
    else
    {
      current_elf_calorie += std::stoi(current_line);
    }
  }

  input_file.close();

  // Get the three maximum calories
  unsigned int first_max = calories.get_max();
  unsigned int second_max = calories.get_max();
  unsigned int third_max = calories.get_max();
  std::cout << "first_max:\t" << first_max << "\n";
  std::cout << "second_max:\t" << second_max << "\n";
  std::cout << "third_max:\t" << third_max << "\n";
  combined_calories += first_max;
  combined_calories += second_max;
  combined_calories += third_max;

  std::cout << "Combined max calories:\t" << combined_calories << "\n";

  return 0;
}