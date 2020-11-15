#include "simple_random_generator.hpp"

SimpleRandomGenerator SimpleRandomGenerator::instance{};

SimpleRandomGenerator::SimpleRandomGenerator() : mt_(seed()) {}

unsigned int SimpleRandomGenerator::seed() {
  std::random_device dev;
  const auto value = dev();
  return value;
}