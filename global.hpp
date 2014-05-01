/** @file global.hpp
    @brief Global operator overloads.
*/

#ifndef OGLWRAP_GLOBAL_HPP_
#define OGLWRAP_GLOBAL_HPP_

#include <iostream>
#include <string>

#include "shader.hpp"
#include "vertexAttrib.hpp"
#include "glm/glm/glm.hpp"

#if !OGLWRAP_CHECK_DEPENDENCIES || defined(glGetAttribLocation)
/// A global operator that is a VertexArray constructor with nicer syntax.
/** I think (prog | "Position") does look better than VertexAttribArray(prog, "Position") */
inline oglwrap::LazyVertexAttribArray operator|(oglwrap::Program& prog, const std::string& file) {
  return oglwrap::LazyVertexAttribArray(prog, file);
}
#endif

template<class T>
/// Interpolates two things.
/// @param a - the first thing.
/// @param b - the second thing.
/// @param alpha - Specifies how 'b' will the result be.
T interpolate(const T& a, const T& b, float alpha) {
  return a + alpha * (b - a);
}

template <typename T>
/// Prints a glm vector to a given ostream.
/** @param os - The ostream.
  * @param v - The vector. */
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec2<T>& v) {
  os << v.x << ", " << v.y;
  return os;
}

template <typename T>
/// Prints a glm vector to a given ostream.
/** @param os - The ostream.
  * @param v - The vector. */
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec3<T>& v) {
  os << v.x << ", " << v.y << ", " << v.z;
  return os;
}

template <typename T>
/// Prints a glm vector to a given ostream.
/** @param os - The ostream.
  * @param v - The vector. */
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec4<T>& v) {
  os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
  return os;
}

template <typename T>
/// Prints a glm matrix to a given ostream.
/** @param os - The ostream.
  * @param mat - The matrix. */
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tmat2x2<T>& mat) {
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 2; j++) {
      std::cout << mat[j][i];
      if(i != 1 || j != 1) {
        std::cout  << ", ";
      }
    }
    std::cout << std::endl;
  }
  return os;
}

template <typename T>
/// Prints a glm matrix to a given ostream.
/** @param os - The ostream.
  * @param mat - The matrix. */
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tmat3x3<T>& mat) {
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      std::cout << mat[j][i];
      if(i != 2 || j != 2) {
        std::cout  << ", ";
      }
    }
    std::cout << std::endl;
  }
  return os;
}

template <typename T>
/// Prints a glm matrix to a given ostream.
/** @param os - The ostream.
  * @param mat - The matrix. */
static inline std::ostream& operator<<(std::ostream& os, const glm::detail::tmat4x4<T>& mat) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      std::cout << mat[j][i];
      if(i != 3 || j != 3) {
        std::cout  << ", ";
      }
    }
    std::cout << std::endl;
  }
  return os;
}

template <typename T>
/// Clamps a value to a given range.
/** @param val - The value to clamp.
  * @param min - The lower bound of the range.
  * @param max - The upper bound of the range. */
inline T clamp(const T& val, const T& min, const T& max) {
  if(val < min) {
    return min;
  } else if(val > max) {
    return max;
  } else {
    return val;
  }
}

#endif // OGLWRAP_GLOBAL_HPP_