#pragma once
#include <Ogre.h>
#include <string>
#include <sstream>

namespace fpsgame {
inline std::string vec_to_str(const Ogre::Vector3& v) {
  std::ostringstream oss;
  oss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return oss.str();
}

inline std::string quat_to_str(const Ogre::Quaternion& q) {
  std::ostringstream oss;
  //q.
  //oss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return oss.str();
}
}  // namespace fpsgame
