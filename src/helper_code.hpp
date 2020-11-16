#pragma once
#include <Ogre.h>
#include <sstream>
#include <string>

namespace fpsgame {
inline std::string vec_to_str(const Ogre::Vector3& v) {
  std::ostringstream oss;
  oss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return oss.str();
}

inline std::string vec_to_str(const Ogre::Vector2& v) {
  std::ostringstream oss;
  oss << "(" << v.x << ", " << v.y << ")";
  return oss.str();
}

inline bool floats_close(float a, float b, float epsilon = 0.001f) {
  const auto diff = a - b;
  return (diff < epsilon && diff > -epsilon);
}
}  // namespace fpsgame
