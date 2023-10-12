#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdexcept>

enum Shape
{
  rock,
  paper,
  scissors
};

enum Result
{
  win,
  lose,
  draw
};

std::map<std::string, Shape> action_to_shape = {{"A", Shape::rock}, {"B", Shape::paper}, {"C", Shape::scissors}};
std::map<std::string, Result> response_to_result = {{"X", Result::lose}, {"Y", Result::draw}, {"Z", Result::win}};

std::map<Result, unsigned int> result_to_value = {{Result::lose, 0}, {Result::draw, 3}, {Result::win, 6}};
std::map<Shape, unsigned int> shape_to_value = {{Shape::rock, 1}, {Shape::paper, 2}, {Shape::scissors, 3}};

std::map<Shape, Shape> shape_to_win = {{Shape::rock, Shape::scissors}, {Shape::paper, Shape::rock}, {Shape::scissors, Shape::paper}};

// Given the enemy's shape and the desired result, determines which response to bring about the result.
Shape versus(const std::pair<Shape, Result> &action_result)
{
  switch (action_result.second)
  {
  case Result::win:
    return shape_to_win[shape_to_win[action_result.first]];
  case Result::draw:
    return action_result.first;
  case Result::lose:
    return shape_to_win[action_result.first];

  default:
    throw std::invalid_argument("Invalid result.");
  }
}

// Gets the first and second characters from the line
std::pair<Shape, Result> extract_action_response(const std::string &str)
{
  std::pair<Shape, Result> action_response;

  action_response.first = action_to_shape[str.substr(0, 1)];
  action_response.second = response_to_result[str.substr(2, 3)];

  return action_response;
}

unsigned int compute_score_for_round(const std::pair<Shape, Result> &action_result_pair)
{

  return shape_to_value[versus(action_result_pair)] + result_to_value[action_result_pair.second];
}

int main(int argc, char *argv[])
{
  std::ifstream input_file;
  std::string current_line;

  input_file.open("day02_input.txt");
  // input_file.open("test.txt");

  if (!input_file.good())
  {
    input_file.close();
    return 1;
  }

  unsigned int total_score = 0;
  while (std::getline(input_file, current_line))
  {
    std::pair<Shape, Result> action_result_pair = extract_action_response(current_line);

    unsigned int score_for_round = compute_score_for_round(action_result_pair);
    std::cout << "Score for round:\t" << score_for_round << "\n";

    total_score += score_for_round;
  }

  input_file.close();

  std::cout << "Total score if strategy guide is followed:\t" << total_score << "\n";

  return 0;
}