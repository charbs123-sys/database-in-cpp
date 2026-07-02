#pragma once

#include <string>
#include <vector>

namespace Query
{
  enum Type 
  {
    q_select,
    q_insert,
    q_delete,
    q_create,
    q_drop,
    number_of_queries,
  };

  enum Inequalities 
  {
    equal,
    not_equal,
    less_than,
    less_than_equal,
    greater_than,
    greater_than_equal,
  };

  struct Instructions
  {
    std::vector<std::string_view> select_column_names{};
    std::string_view from_table_name{};

    struct Conditions 
    {
      std::string_view first_table_name {};
      Inequalities inequality{equal};
      std::string_view second_table_name {};
    };

    Conditions conditions{};

  };

  // this shouldn't be in the Query namespace
  enum SelectParsing
  {
    in_select,
    in_from,
    in_condition,
  };
}

void ltrim(std::string& s);
void rtrim(std::string& s);
std::string_view trim(std::string_view s);

std::ostream& operator<<(std::ostream& cout, const Query::Instructions& instructions);

class Parser
{
  private:
    std::string_view m_user_input{}; 
    const Query::Type identify_query_type();
    const std::string_view first_word();
    Query::Instructions user_instructions;

    void parse_user_query();
    void select_parser();
    Query::Inequalities inequality_to_enum(std::string_view inequality_substring);
    
    template <typename T>
    void populate_user_instructions(std::basic_string_view<T> token, Query::SelectParsing cur_parsing_area);
    template <typename T>
    std::vector<std::string_view> parse_in_select(std::basic_string_view<T> token);
    template <typename T>
    Query::Instructions::Conditions parse_in_conditions(std::basic_string_view<T> token);


  public:
    Parser(std::string_view user_input);
    Query::Instructions get_user_instructions();
};

