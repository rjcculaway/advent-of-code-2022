#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <stack>

class Node;
class File;
class Directory;
class Filesystem;

/**
 * Since we are dealing with a "filesystem", it's best to represent it as a tree.
 * */

/// @brief Abstract class representing a node in the directory tree.
class Node
{
protected:
  std::string name;
  std::shared_ptr<Directory> parent;

public:
  /// @brief Each node has a file size which must be implemented by the derived classes.
  /// @return The size of the node.
  virtual unsigned int get_node_size() = 0;

  /// @brief Returns the name of the node.
  /// @return The name of the node.
  const std::string get_name()
  {
    return name;
  }

  /// @brief Checks if this node can have children.
  /// @return `true` if it can, `false` if it cannot.
  virtual bool can_have_children() = 0;

  /// @brief Checks if this node has a parent.
  /// @return `true` if it has a parent, `false` if not.
  bool has_parent()
  {
    return parent != nullptr;
  }

  /// @brief Gets the parent of the node. Can only be a Directory, as other Nodes cannot have children.
  /// @return The parent Directory.
  const std::shared_ptr<Directory> &get_parent()
  {
    return parent;
  }

  /// @brief Changes the parent of the Node.
  /// @param parent The parent Directory of the Node.
  void set_parent(std::shared_ptr<Directory> parent)
  {
    this->parent = parent;
  }

  /// @brief Displays node information.
  virtual void print_node() = 0;

  Node(std::string name, std::shared_ptr<Directory> parent = nullptr) : name(name)
  {
    this->parent = parent;
  }
};

/// @brief A Node representing a directory. Having no size of its own, its size is derived from its children.
class Directory : public Node
{
private:
  std::vector<std::shared_ptr<Node>> children; // Each child is part of a vector.

public:
  Directory(std::string name, std::shared_ptr<Directory> parent = nullptr) : Node(name, parent)
  {
  }

  /// @brief The size of a Directory is dependent on its children.
  /// @return The size of the directory.
  unsigned int get_node_size()
  {
    unsigned int file_size = 0;
    for (auto i = children.begin(); i != children.end(); i++)
    {
      file_size += (*i)->get_node_size();
    }
    return file_size;
  }

  const std::string get_name()
  {
    return this->name;
  }

  bool can_have_children()
  {
    return true;
  }

  /// @brief Adds a child to this Directory.
  /// @param node The new child.
  void add_child(Node *node)
  {
    children.emplace_back(std::shared_ptr<Node>(node));
  }

  /// @brief Gets the children of this Node.
  /// @return A vector containing this Node's children.
  const std::vector<std::shared_ptr<Node>> &get_children()
  {
    return children;
  }

  void print_node()
  {
    std::cout << "- " << name << " (dir)\n";
    return;
  }
};

/// @brief Unlike a Directory, a File has an intrinsic size. However, it can only be a leaf node.
class File : public Node
{
private:
  unsigned int file_size = 0;

public:
  File(std::string name, std::shared_ptr<Directory> parent = nullptr, unsigned int size = 0) : Node(name, parent), file_size(size)
  {
  }

  /// @brief Gets the size of the File.
  /// @return The size of the file.
  unsigned int get_node_size()
  {
    return file_size;
  }

  const std::string get_name()
  {
    return this->name;
  }

  bool can_have_children()
  {
    return false;
  }

  void print_node()
  {
    std::cout << "- " << name << " (file, size=" << file_size << ")\n";
    return;
  }
};

/// @brief A tree representing the file system.
class Filesystem
{
private:
  std::shared_ptr<Directory> root = std::make_shared<Directory>(Directory("/"));
  std::shared_ptr<Directory> current_directory = root;
  unsigned int total_size;

  /// @brief From the current Directory, searches for a Node with the name `key`.
  /// @param key The Directory to find.
  /// @return A shared pointer to the found Node.
  std::shared_ptr<Node> find_name(std::string key)
  {
    auto dir = std::dynamic_pointer_cast<Directory>(current_directory);
    auto &children = (dir->get_children());

    for (auto i = children.begin(); i != children.end(); i++)
    {
      std::shared_ptr<Node> child = *i;
      if ((*child).get_name() == key)
      {
        return child;
      }
    }

    return nullptr;
  }

  /// @brief Changes this Filesystem's current directory to `new_directory`.
  /// @param new_directory A shared pointer to the new directory.
  void change_directory(std::shared_ptr<Directory> new_directory)
  {
    current_directory = new_directory;
  }

  /// @brief Helper function for `find_directories_of_size`.
  /// @param target_size
  /// @param current_dir
  /// @param directories
  void find_directories_of_size_helper(const unsigned int &target_size, std::shared_ptr<Directory> current_dir, std::vector<std::shared_ptr<Directory>> &directories)
  {
    if (!(current_dir->can_have_children()))
    {
      return;
    }
    if (current_dir->get_node_size() <= target_size)
    {
      directories.emplace_back(current_dir);
    }
    auto children = current_dir->get_children();
    for (auto child : children)
    {
      if (child->can_have_children())
      {
        std::shared_ptr<Directory> child_dir = std::dynamic_pointer_cast<Directory>(child);
        find_directories_of_size_helper(target_size, child_dir, directories);
      }
    }
  }

  /// @brief Helper function for `get_minimum_for_target_size_directory`.
  /// @param target_size
  /// @param free_space
  /// @param current_dir
  /// @return
  std::shared_ptr<Directory> get_minimum_for_target_size_directory_helper(const unsigned int &target_size, const unsigned int &free_space, std::shared_ptr<Directory> current_dir)
  {
    std::shared_ptr<Directory> min_dir = current_dir;
    unsigned int min_dir_size = current_dir->get_node_size();

    if (free_space + min_dir_size >= target_size)
    {
      for (auto &child : current_dir->get_children())
      {
        if (child->can_have_children())
        {
          auto &child_dir = get_minimum_for_target_size_directory_helper(target_size, free_space, std::dynamic_pointer_cast<Directory>(child));
          if (child_dir)
          {
            unsigned int child_dir_size = child_dir->get_node_size();
            if (child_dir_size < min_dir_size && free_space + child_dir_size >= target_size)
            {
              min_dir = child_dir;
              min_dir_size = child_dir_size;
            }
          }
        }
      }
    }
    else
    {
      min_dir = nullptr;
    }

    return min_dir;
  }

  /// @brief Helper function for `print_filesystem`.
  /// @param node
  /// @param tabs
  void print_filesystem_helper(std::shared_ptr<Node> node, int tabs)
  {
    for (size_t i = 0; i < tabs; i++)
    {
      std::cout << "\t";
    }
    node->print_node();
    if (node->can_have_children())
    {
      auto directory = std::dynamic_pointer_cast<Directory>(node);
      auto children = directory->get_children();
      for (auto child : children)
      {
        print_filesystem_helper(child, tabs + 1);
      }
    }
    return;
  }

public:
  Filesystem(unsigned int total_size) : total_size(total_size)
  {
  }

  /// @brief Adds a File node to the current directory.
  /// @param file
  void add_file_to_current_directory(File *file)
  {
    if (!current_directory->can_have_children())
    {
      return;
    }
    auto directory = std::dynamic_pointer_cast<Directory>(current_directory);

    file->set_parent(current_directory);
    current_directory->add_child(file);

    // std::cout << "Adding file " << file->get_name() << " to " << current_directory->get_name() << "\n";
    return;
  }

  /// @brief Adds a Directory node to the current directory.
  /// @param directory
  void add_folder_to_current_directory(Directory *directory)
  {
    if (!current_directory->can_have_children())
    {
      return;
    }

    directory->set_parent(current_directory);
    current_directory->add_child(directory);

    // std::cout << "Adding file " << directory->get_name() << " to " << current_directory->get_name() << "\n";

    return;
  }

  /// @brief Changes the current directory to root.
  void change_directory_to_root()
  {
    change_directory(root);
  }

  /// @brief Moves up one directory from the current directory.
  void change_directory_to_above()
  {
    if (current_directory->has_parent())
    {
      auto &parent = current_directory->get_parent();
      change_directory(parent);
      // std::cout << "Moving to:\t" << parent->get_name() << "\n";
    }
  }

  /// @brief Moves to a new directory specified by `dir_name`.
  /// @param dir_name The directory to change to.
  void change_directory_by_name(std::string dir_name) noexcept(false)
  {
    if (!current_directory->can_have_children())
    {
      return;
    }

    auto &directory = std::dynamic_pointer_cast<Directory>(current_directory);
    auto &children = (directory->get_children());

    for (auto i = children.begin(); i != children.end(); i++)
    {
      if ((*i)->get_name() == dir_name)
      {
        if (!(*i)->can_have_children())
        {
          throw std::invalid_argument(dir_name + " is not a directory.");
        }
        change_directory(std::dynamic_pointer_cast<Directory>(*i));
        // std::cout << "Moving to:\t" << (*i)->get_name() << "\n";
      }
    }

    return;
  }

  /// @brief Gets the used space of the Filesystem, which is simply the size of the root directory.
  /// @return The used space.
  unsigned int get_used_space()
  {
    return root->get_node_size();
  }

  /// @brief Gets all Directories of size `target_size` and above.
  /// @param target_size The target size for a directory to meet.
  /// @return
  std::vector<std::shared_ptr<Directory>> find_directories_of_size(const unsigned int target_size = 100000)
  {
    std::vector<std::shared_ptr<Directory>> directories;

    find_directories_of_size_helper(target_size, root, directories); // Start with the root directory and passing a ref to the vector.

    return directories;
  }

  /// @brief Gets the smallest directory that can be deleted to have `target_size` free space in the Filesystem.
  /// @param target_size The target size to meet.
  /// @return The shared pointer to the minimum-sized Directory.
  std::shared_ptr<Directory> get_minimum_for_target_size_directory(const unsigned int target_size = 100000)
  {
    const unsigned int &free_space = total_size - get_used_space();
    return get_minimum_for_target_size_directory_helper(target_size, free_space, root);
  }

  void print_filesystem()
  {
    print_filesystem_helper(root, 0);
    return;
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

/// @brief Parses an input file's lines as a vector of lines.
/// @param file_name The file to load.
/// @return A vector of each line in the file.
std::vector<std::string> file_lines_to_vector(const std::string &file_name)
{
  std::vector<std::string> lines;
  std::ifstream file_handle(file_name);

  if (file_handle.bad())
  {
    std::cout << "Failed to read the file.\n";
    file_handle.close();
    return;
  }

  std::string current_line;
  while (std::getline(file_handle, current_line))
  {
    lines.emplace_back(current_line);
  }

  file_handle.close();

  return lines;
}

void parse_user_command(Filesystem &filesystem, std::vector<std::string>::iterator &line_i, std::vector<std::string>::iterator &lines_end, std::stack<std::string> &parsing_stack)
{
  parsing_stack.pop();
  std::string token = parsing_stack.top();
  parsing_stack.pop();
  if (token == "cd") // Change directory
  {
    token = parsing_stack.top();
    parsing_stack.pop();

    if (token == "..")
    {
      filesystem.change_directory_to_above();
    }
    else if (token == "/")
    {
      filesystem.change_directory_to_root();
    }
    else
    {
      filesystem.change_directory_by_name(token);
    }
  }

  if (token == "ls") // List files (update the filesystem)
  {
    std::vector<std::string> lines;
    while (true)
    {
      line_i++;

      if (line_i == lines_end || (*line_i).at(0) == '$')
      {
        line_i--;
        break;
      }
      lines.emplace_back(*line_i);
    }

    for (auto line = lines.begin(); line != lines.end(); line++)
    {
      std::vector<std::string> line_tokenized = split(*line, " ");
      for (auto line_token_i = line_tokenized.rbegin(); line_token_i != line_tokenized.rend(); line_token_i++)
      {
        auto line_token = *line_token_i;
        parsing_stack.push(line_token);
        if (line_token == "dir")
        {
          parsing_stack.pop();
          std::string dir_name = parsing_stack.top();
          parsing_stack.pop();
          filesystem.add_folder_to_current_directory(new Directory(dir_name));
        }

        try
        {
          unsigned int file_size = std::stoi(line_token);
          parsing_stack.pop();
          std::string file_name = parsing_stack.top();
          filesystem.add_file_to_current_directory(new File(file_name, nullptr, file_size));
        }
        catch (std::invalid_argument)
        {
        }
      }
    }
  }
  return;
}

bool parse_commands(Filesystem &filesystem, std::vector<std::string> &lines)
{

  std::stack<std::string> parsing_stack;

  for (auto line_i = lines.begin(); line_i != lines.end(); line_i++)
  {
    std::vector<std::string> tokens = split(*line_i, " ");
    for (auto token_i = tokens.rbegin(); token_i != tokens.rend(); token_i++)
    {
      auto token = *token_i;
      parsing_stack.push(token);
      if (token == "$")
      {
        parse_user_command(filesystem, line_i, lines.end(), parsing_stack);
      }
    }
  }

  return true;
}

unsigned int compute_directory_sizes(std::vector<std::shared_ptr<Directory>> directories)
{
  unsigned int directory_sizes = 0;
  for (auto &i : directories)
  {
    directory_sizes += i->get_node_size();
  }
  return directory_sizes;
}