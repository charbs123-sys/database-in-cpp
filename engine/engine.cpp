#include "engine.h"
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <cassert>

Engine::Engine(Query::Instructions user_instructions) 
  : m_user_instructions {user_instructions}
{
  std::ifstream table_output;
  // simulates the from statement
  table_output.open(retrieve_table_file_location(), std::ios::out | std::ios::in);
  // simulates the select statement
  populate_select_columns(table_output);

  display_table(table_output);

  
} 



std::vector<std::string_view> Engine::parse_csv_line(std::string_view line)
{
  int cursor {0};
  std::vector<std::string_view> cols {};
  while (cursor != std::string_view::npos)
  {
    auto end {line.find_first_of(",", cursor)};
    auto col {line.substr(cursor, end - cursor)};
    // std::cout << "this is the end index " << end << '\n';
    cols.push_back(trim(col));
    if (end == std::string_view::npos) break;
    cursor = end + 1;
  }
  return cols;
}

void Engine::populate_select_columns(std::ifstream& table)
{
  std::string line;
  if (!table.is_open()) throw std::io_errc();

  std::getline(table, line);
  std::vector<std::string_view> cols {parse_csv_line(line)};
  select_columns(cols);
}

void Engine::select_columns(std::vector<std::string_view> table_cols)
{
  for (std::size_t i{0}; i < table_cols.size(); ++i)
  {
    for (const auto& select_col : m_user_instructions.select_column_names)
    {
      if (table_cols[i] == select_col)
      {
        implement_conds.select_cols_to_keep.push_back(i); 
      }
    }
  }
}

const void Engine::display_table(std::ifstream& table)
{
  std::cout << "starting to display table\n";
  std::string line;
  if (table.is_open())
  {
    std::cout << "table is open\n";
    // loop through each row
    while (std::getline(table, line))
    {
      // std::cout << line << '\n';
      std::vector<std::string_view> cols {parse_csv_line(line)};
      assert(implement_conds.select_cols_to_keep.size() <= cols.size());
      cols = impose_inequality_condition(cols);
      print_row(cols);
      std::cout << std::endl;
    }
    table.close();
  }
}

void Engine::print_row(std::vector<std::string_view> row_vals)
{
  for (const auto& select_columns : implement_conds.select_cols_to_keep)
  {
    std::cout << row_vals[select_columns] << " ";
  }
}

std::string Engine::retrieve_table_file_location()
{

  std::string relative_file_path {"../database/"};
  total_path = relative_file_path + static_cast<std::string>(m_user_instructions.from_table_name) + ".csv";
  std::cout << "This is the total path " << total_path << '\n';

  if (!is_file_exists(total_path))
    throw std::io_errc();

  return total_path;
}


bool is_file_exists(const std::string_view filename)
{
  return std::filesystem::exists(filename);
}

