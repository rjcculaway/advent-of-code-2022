//-------------------------------------------------------------------------------------------------
// Day 08: No Space Left On Device
// by Rene Jotham C. Culaway
//
// Find all of the directories with a total size of at most 100000.
// What is the sum of the total sizes of those directories?
//-------------------------------------------------------------------------------------------------

#include "day08.hpp"

const std::string FILE_NAME = "input.txt";

// This code is seriously unoptimized. But that isn't the focus here lol.
int main(int argc, char *argv[])
{
  TreeCover tree_cover(FILE_NAME);
  std::pair<size_t, size_t> tree(3, 2);
  // tree_cover.print_grid();
  // std::cout << "top \n";
  // tree_cover.print_visbility_mask(tree_cover.compute_visibility_mask_tree_top(tree));
  // std::cout << "left \n";
  // tree_cover.print_visbility_mask(tree_cover.compute_visibility_mask_tree_left(tree));
  // std::cout << "right \n";
  // tree_cover.print_visbility_mask(tree_cover.compute_visibility_mask_tree_right(tree));
  // std::cout << "bottom \n";
  // tree_cover.print_visbility_mask(tree_cover.compute_visibility_mask_tree_bottom(tree));
  // std::cout << "\n";
  // tree_cover.print_visbility_mask(tree_cover.get_visible_trees());
  std::cout << tree_cover.get_maximum_scenic_score() << "\n";
  return 0;
}