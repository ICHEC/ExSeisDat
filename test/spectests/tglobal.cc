#include "tglobal.hh"

template bool testing::internal::TypeIdHelper<MPIIOTest>::dummy_;
template bool testing::internal::TypeIdHelper<RuleFixList>::dummy_;
template bool testing::internal::TypeIdHelper<RuleFixEmpty>::dummy_;
template bool testing::internal::TypeIdHelper<RuleFixDefault>::dummy_;
template bool testing::internal::TypeIdHelper<FileReadSEGYTest>::dummy_;
template bool testing::internal::TypeIdHelper<FileWriteSEGYTest>::dummy_;
template bool testing::internal::TypeIdHelper<SetTest>::dummy_;

template typename testing::DefaultValue<unsigned long>::ValueProducer* testing::DefaultValue<unsigned long>::producer_;
template typename testing::DefaultValue<double>::ValueProducer* testing::DefaultValue<double>::producer_;
