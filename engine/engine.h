#pragma once
#include "../parser/parser.h"
#include <fstream>
#include <unordered_map>

struct QueryConditions
{
  std::vector<int> select_cols_to_keep{};
  std::unordered_map<std::string_view, int> select_cols_to_int{};
};

bool is_file_exists(const std::string_view filename);

class Engine 
{
  private:
    QueryConditions implement_conds{};
    void populate_select_columns(std::ifstream& table);
    std::vector<std::string_view> parse_csv_line(std::string_view line);
    void select_columns(std::vector<std::string_view> table_cols);
    void print_row(std::vector<std::string_view> row_vals); 

    Query::Instructions m_user_instructions {};
    std::string retrieve_table_file_location();
    const void display_table(std::ifstream& table);
    std::string total_path {};
  public:
    Engine(Query::Instructions user_instructions);
};
