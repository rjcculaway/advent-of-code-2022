#pragma once
#include "utils.fwd.hpp"
#include <exception>
#include <iostream>

namespace utils
{
  // Implements a vector-based max-heap.
  // This is one-indexed.
  template <class T>
  class Heap
  {
    std::vector<T> heap;

  private:
    size_t max_size;
    size_t size;

    void swap(size_t i, size_t j)
    {
      size_t temp = heap[i];
      heap[i] = heap[j];
      heap[j] = temp;
      return;
    }

    bool is_root(size_t i)
    {
      return i == 1;
    }

    bool is_left_child(size_t i)
    {
      if (i > size)
      {
        throw std::out_of_range("Element exceeds heap size.");
      }

      return i % 2 == 0 && !is_root(i);
    }

    bool is_right_child(size_t i)
    {
      if (i > size)
      {
        throw std::out_of_range("Element exceeds heap size.");
      }

      return i % 2 == 1;
    }

    size_t get_parent_index(size_t i)
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

    size_t get_left_child_index(size_t i)
    {
      size_t left_index = 2 * i;
      if (left_index > size)
      {
        throw std::out_of_range("Left child exceeds heap size.");
      }
      return left_index;
    }

    size_t get_right_child_index(size_t i)
    {
      size_t right_index = 2 * i + 1;
      if (right_index > size)
      {
        throw std::out_of_range("Right child exceeds heap size.");
      }
      return right_index;
    }

    void percolate_up(size_t i)
    {
      size_t current_index = i;
      while (!is_root(current_index))
      {
        size_t parent_index = get_parent_index(current_index);
        if (heap[current_index] < heap[parent_index])
        {
          break;
        }
        swap(parent_index, current_index);
        current_index = parent_index;
      }
      return;
    }

    void percolate_down(size_t i)
    {
      size_t current_index = i;
      while (current_index < size)
      {
        size_t left_child_index = 0;
        size_t right_child_index = 0;
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

        size_t larger_child_index = heap[left_child_index] > heap[right_child_index] ? left_child_index : right_child_index;

        if (heap[current_index] > heap[larger_child_index])
        {
          break;
        }
        swap(current_index, larger_child_index);
        current_index = larger_child_index;
      }
      return;
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

  public:
    Heap(size_t max_size) : max_size(max_size)
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

    void insert(const T new_value)
    {
      if (size >= max_size)
      {
        throw std::overflow_error("Heap is full. Maximum size: " + std::to_string(max_size) + "\n");
      }
      heap[++size] = new_value;
      percolate_up(size);
    }

    T get_max()
    {
      if (is_empty())
      {
        throw std::underflow_error("Heap is empty.");
      }
      swap(1, size);
      T max_value = heap[size--];
      percolate_down(1);
      return max_value;
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

  /// @brief Splits a string by a delimiter.
  /// @param str The string to split.
  /// @param delim The delimiter by which to split the string.
  /// @return A `vector` of tokens split by the delimiter.
  std::vector<std::string> split(const std::string &str, const std::string &delim)
  {
    std::vector<std::string> tokens;

    size_t start = 0, end = 0;
    while (start < str.length())
    {
      end = str.find(delim, start);
      if (end == std::string::npos)
      {
        std::string right = str.substr(start, str.length() - start);
        if (right.length() > 0)
        {
          tokens.emplace_back(right);
          break;
        }
      }

      std::string left = str.substr(start, end - start);
      start = end + 1;

      if (left.length() > 0)
      {
        tokens.emplace_back(left);
      }
    }

    return tokens;
  }
}
