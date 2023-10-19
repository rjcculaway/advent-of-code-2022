#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <numeric>

/// @brief A class representing a grid of trees.
class TreeCover
{
private:
  std::vector<std::vector<unsigned int>> grid;

  bool is_edge(const size_t &row, const size_t &column)
  {
    return row == 0 || row == grid.size() - 1 || column == 0 || column == grid[0].size() - 1;
  }

  /// @brief Prepares a visibility mask for usage.
  /// @return A matrix of boolean values.
  std::vector<std::vector<bool>> create_visibility_mask()
  {
    size_t mask_rows = grid.size(), mask_columns = grid[0].size();
    std::vector<std::vector<bool>> visibility_mask;
    visibility_mask.resize(mask_rows);

    for (auto row = visibility_mask.begin(); row != visibility_mask.end(); row++)
    {
      *row = std::vector<bool>(mask_columns, false);
    }

    return visibility_mask;
  }

  /// @brief Gets the visibility mask for trees when viewed vertically from the edge.
  /// @param compute_idx The computation function for the indices.
  /// @return
  std::vector<std::vector<bool>> compute_visibility_mask_vertical_edge(std::function<size_t(size_t, size_t, size_t)> compute_idx)
  {
    size_t mask_rows = grid.size(), mask_columns = grid[0].size();
    std::vector<std::vector<bool>> visibility_mask = create_visibility_mask();

    std::vector<unsigned int> tallest_tree_lengths = std::vector<unsigned int>(grid[0].size());
    for (size_t j = 0; j < mask_columns; j++)
    {
      tallest_tree_lengths[j] = grid[compute_idx(0, mask_rows, mask_columns)][j];
    }

    for (size_t idx = 0; idx < mask_rows; idx++)
    {
      size_t i = compute_idx(idx, mask_rows, mask_columns);
      for (size_t j = 0; j < mask_columns; j++)
      {
        bool is_visible = is_edge(i, j) || (grid[i][j] > tallest_tree_lengths[j]);
        visibility_mask[i][j] = visibility_mask[i][j] || is_visible;
        tallest_tree_lengths[j] = std::max(grid[i][j], tallest_tree_lengths[j]);
      }
    }

    return visibility_mask;
  }

  /// @brief Gets the visibility mask for trees when viewed horizontally from the edge.
  /// @param compute_idx The computation function for the indices.
  /// @return
  std::vector<std::vector<bool>> compute_visibility_mask_horizontal_edge(std::function<size_t(size_t, size_t, size_t)> compute_idx)
  {
    size_t mask_rows = grid.size(), mask_columns = grid[0].size();
    std::vector<std::vector<bool>> visibility_mask = create_visibility_mask();

    std::vector<unsigned int> tallest_tree_lengths = std::vector<unsigned int>(grid[0].size());
    for (size_t i = 0; i < mask_columns; i++)
    {
      tallest_tree_lengths[i] = grid[i][compute_idx(0, mask_rows, mask_columns)];
    }

    for (size_t idx = 0; idx < mask_columns; idx++)
    {
      size_t j = compute_idx(idx, mask_rows, mask_columns);
      for (size_t i = 0; i < mask_rows; i++)
      {
        bool is_visible = is_edge(i, j) || (grid[i][j] > tallest_tree_lengths[i]);
        visibility_mask[i][j] = visibility_mask[i][j] || is_visible;
        tallest_tree_lengths[i] = std::max(grid[i][j], tallest_tree_lengths[i]);
      }
    }

    return visibility_mask;
  }

  /// @brief Gets the visibility mask for trees when viewed vertically relative to a tree.
  /// @param compute_idx The computation function for the indices.
  /// @return
  std::vector<std::vector<bool>> compute_visibility_mask_from_tree_vertical(std::pair<size_t, size_t> tree, std::function<int(size_t)> compute_idx)
  {
    std::vector<std::vector<bool>> visibility_mask = create_visibility_mask();
    size_t mask_rows = visibility_mask.size(), mask_columns = visibility_mask[0].size();

    const int tree_row_offset = tree.first;
    const int tree_col_offset = tree.second;

    for (int i = 1; tree_row_offset + compute_idx(i) >= 0 && tree_row_offset + compute_idx(i) < mask_rows; i++)
    {
      visibility_mask[tree_row_offset + compute_idx(i)][tree_col_offset] = true;
      if (grid[tree_row_offset][tree_col_offset] <= grid[tree_row_offset + compute_idx(i)][tree_col_offset])
      {
        break;
      }
    }

    return visibility_mask;
  }

  /// @brief Gets the visibility mask for trees when viewed horizontally relative to a tree.
  /// @param compute_idx The computation function for the indices.
  /// @return
  std::vector<std::vector<bool>> compute_visibility_mask_from_tree_horizontal(std::pair<size_t, size_t> tree, std::function<int(size_t)> compute_idx)
  {
    std::vector<std::vector<bool>> visibility_mask = create_visibility_mask();
    size_t mask_rows = visibility_mask.size(), mask_columns = visibility_mask[0].size();

    const int tree_row_offset = tree.first;
    const int tree_col_offset = tree.second;

    for (int j = 1; tree_col_offset + compute_idx(j) >= 0 && tree_col_offset + compute_idx(j) < mask_columns; j++)
    {
      visibility_mask[tree_row_offset][tree_col_offset + compute_idx(j)] = true;
      if (grid[tree_row_offset][tree_col_offset] <= grid[tree_row_offset][tree_col_offset + compute_idx(j)])
      {
        break;
      }
    }

    return visibility_mask;
  }

public:
  std::vector<std::vector<bool>> compute_visibility_mask_top_edge()
  {
    auto compute_idx = [](size_t i, size_t rows, size_t columns)
    {
      return i;
    };
    return compute_visibility_mask_vertical_edge(compute_idx);
  }

  std::vector<std::vector<bool>> compute_visibility_mask_left_edge()
  {
    auto compute_idx = [](size_t i, size_t rows, size_t columns)
    {
      return i;
    };
    return compute_visibility_mask_horizontal_edge(compute_idx);
  }

  std::vector<std::vector<bool>> compute_visibility_mask_bottom_edge()
  {
    auto compute_idx = [](size_t i, size_t rows, size_t columns)
    {
      return rows - i - 1;
    };
    return compute_visibility_mask_vertical_edge(compute_idx);
  }

  std::vector<std::vector<bool>> compute_visibility_mask_right_edge()
  {
    auto compute_idx = [](size_t i, size_t rows, size_t columns)
    {
      return columns - i - 1;
    };
    return compute_visibility_mask_horizontal_edge(compute_idx);
  }

  std::vector<std::vector<bool>> get_visible_trees()
  {
    std::vector<std::vector<std::vector<bool>>> visibility_masks{
        compute_visibility_mask_top_edge(),
        compute_visibility_mask_left_edge(),
        compute_visibility_mask_bottom_edge(),
        compute_visibility_mask_right_edge(),
    };

    std::vector<std::vector<bool>> combined_visibility = create_visibility_mask();

    for (size_t i = 0; i < combined_visibility.size(); i++)
    {
      for (size_t j = 0; j < combined_visibility[0].size(); j++)
      {
        if (combined_visibility[i][j])
          break;
        for (auto visibility_mask = visibility_masks.cbegin(); visibility_mask != visibility_masks.cend(); visibility_mask++)
        {
          combined_visibility[i][j] = combined_visibility[i][j] || (*visibility_mask)[i][j];
          if ((*visibility_mask)[i][j])
            break;
        }
      }
    }

    return combined_visibility;
  }

  /// @brief Counts visible trees from any vantage point.
  /// @return The number of visible trees.
  unsigned int count_visible_trees()
  {
    unsigned int count = 0;
    std::vector<std::vector<bool>> visibility_mask = get_visible_trees();

    for (auto i : visibility_mask)
    {
      for (auto j : i)
      {
        if (j)
          count++;
      }
    }

    return count;
  }

  std::vector<std::vector<bool>> compute_visibility_mask_tree_top(const std::pair<size_t, size_t> &tree)
  {
    return compute_visibility_mask_from_tree_vertical(tree, [](size_t i)
                                                      { return -i; });
  }

  std::vector<std::vector<bool>> compute_visibility_mask_tree_bottom(const std::pair<size_t, size_t> &tree)
  {
    return compute_visibility_mask_from_tree_vertical(tree, [](size_t i)
                                                      { return i; });
  }

  std::vector<std::vector<bool>> compute_visibility_mask_tree_right(const std::pair<size_t, size_t> &tree)
  {
    return compute_visibility_mask_from_tree_horizontal(tree, [](size_t i)
                                                        { return i; });
  }

  std::vector<std::vector<bool>> compute_visibility_mask_tree_left(const std::pair<size_t, size_t> &tree)
  {
    return compute_visibility_mask_from_tree_horizontal(tree, [](size_t i)
                                                        { return -i; });
  }

  /// @brief Computes a tree's scenic score (how many trees it can see from all angles.)
  /// @param tree The tree's coordinates.
  /// @return The tree's scenic score, which is the product of all of its scores from different vantage points.
  unsigned int compute_tree_scenic_score(const std::pair<size_t, size_t> &tree)
  {
    std::vector<std::vector<std::vector<bool>>> visibility_masks = {
        compute_visibility_mask_tree_top(tree),
        compute_visibility_mask_tree_left(tree),
        compute_visibility_mask_tree_right(tree),
        compute_visibility_mask_tree_bottom(tree),
    };

    std::vector<unsigned int> scores;
    std::transform(visibility_masks.begin(), visibility_masks.end(), std::back_inserter(scores),
                   [&](std::vector<std::vector<bool>> mask)
                   {
                     unsigned int score = 0;
                     for (size_t i = 0; i < mask.size(); i++)
                     {
                       for (size_t j = 0; j < mask.size(); j++)
                       {
                         if (mask[i][j])
                         {
                           score += 1;
                         }
                       }
                     }
                     return score;
                   });

    return std::accumulate(scores.begin() + 1, scores.end(), *scores.begin(),
                           [](unsigned int prev, unsigned int curr)
                           {
                             return prev * curr;
                           });
  }

  unsigned int get_maximum_scenic_score()
  {
    unsigned int max = 0;
    for (size_t i = 0; i < grid.size(); i++)
    {
      for (size_t j = 0; j < grid[0].size(); j++)
      {
        max = std::max(max, compute_tree_scenic_score(std::pair(i, j)));
      }
    }
    return max;
  }

  void print_grid()
  {
    for (auto i = grid.begin(); i != grid.end(); i++)
    {
      for (auto j = (*i).begin(); j != (*i).end(); j++)
      {
        std::cout << *j << "\t";
      }
      std::cout << "\n";
    }
  }

  static void print_visbility_mask(std::vector<std::vector<bool>> mask)
  {
    for (auto i = mask.begin(); i != mask.end(); i++)
    {
      for (auto j = (*i).begin(); j != (*i).end(); j++)
      {
        std::cout << *j << "\t";
      }
      std::cout << "\n";
    }
  }

  TreeCover(const std::string &file_name)
  {
    std::ifstream file_handle(file_name);

    if (file_handle.bad())
    {
      return;
    }

    std::string current_line;
    while (std::getline(file_handle, current_line))
    {
      std::vector<unsigned int> row;
      row.reserve(current_line.length());

      for (char i : current_line)
      {
        row.emplace_back(i - '0');
      }

      grid.emplace_back(row);
    }

    return;
  }
};