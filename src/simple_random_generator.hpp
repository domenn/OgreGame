#pragma once

#include <random>

// Use class's static functions to get random numbers.
class SimpleRandomGenerator {
  static SimpleRandomGenerator instance;
  static constexpr float NORMALIZER = 1.0f / std::mt19937::max();
  static unsigned int seed();

  mutable std::mt19937 mt_;

  SimpleRandomGenerator();

 public:
  SimpleRandomGenerator(const SimpleRandomGenerator& other) = delete;
  // Returns random number, min is inclusive, max is exclusive.
  static float generate(float min, float max) {
    return instance.mt_() * NORMALIZER * (max - min) + min;
  }

  /**
   * Returns number in range  [`min`, `max`).
   * \param min Inclusive min integer.
   * \param max Exclusive max integer.
   * \return generated random value.
   */
  template <typename tp_integer_type>
  static tp_integer_type random_integer(uint32_t min, uint32_t max) {
    return static_cast<tp_integer_type>(instance.mt_() % (max - min) + min);
  }
};
