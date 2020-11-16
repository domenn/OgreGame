#pragma once

//#include <numeric>

class FramesPerSecond {
  constexpr static int BUF_SIZE = 20;
  constexpr static float DIVISOR = 1.0f / BUF_SIZE;

  float buffer_[BUF_SIZE]{};
  float buffer_sum_{0};
  float current_fps_{0};
  int idx_inserting{0};

  int idx_taking() const {
    const auto candidate = idx_inserting + 1;
    return candidate == BUF_SIZE ? 0 : candidate;
  }

 public:
  void add(float fps_value) {
    buffer_sum_ += fps_value;
    const auto idx_taking1 = idx_taking();
    buffer_sum_ -= buffer_[idx_taking1];
    // buffer_sum_ = std::accumulate(std::begin(buffer_), std::end(buffer_),0.f);
    idx_inserting = idx_taking1;
    buffer_[idx_inserting] = fps_value;
    current_fps_ = buffer_sum_ * DIVISOR;
  }

  void add_frame_time(float frame_time) { add(1.f / frame_time); }

  float get_fps() const { return current_fps_; }
};
