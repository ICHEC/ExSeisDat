#include "googletest_variable_instances.hh"

template testing::DefaultValue<PIOL::RuleEntry*>::ValueProducer*
  testing::DefaultValue<PIOL::RuleEntry*>::producer_;

template testing::DefaultValue<size_t>::ValueProducer*
  testing::DefaultValue<size_t>::producer_;

template testing::DefaultValue<bool>::ValueProducer*
  testing::DefaultValue<bool>::producer_;

template testing::DefaultValue<double>::ValueProducer*
  testing::DefaultValue<double>::producer_;

template testing::DefaultValue<PIOL::llint>::ValueProducer*
  testing::DefaultValue<PIOL::llint>::producer_;

template testing::DefaultValue<int16_t>::ValueProducer*
  testing::DefaultValue<int16_t>::producer_;

template const std::string* testing::DefaultValue<const std::string&>::address_;

template testing::DefaultValue<std::string>::ValueProducer*
  testing::DefaultValue<std::string>::producer_;

template testing::DefaultValue<std::vector<std::string>>::ValueProducer*
  testing::DefaultValue<std::vector<std::string>>::producer_;
