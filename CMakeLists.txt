cmake_minimum_required(VERSION 3.13)
project(OgreGame)
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(/MP)
    get_directory_property(OUT_CO_TOP COMPILE_OPTIONS)
    if (NOT "/utf-8" IN_LIST OUT_CO_TOP)
      add_compile_options(/utf-8)
    endif ()
  endif ()
endif ()

file(GLOB_RECURSE _source_list *.cpp* *.cc* *.h* *.hpp*)
foreach (_source IN ITEMS ${_source_list})
  get_filename_component(_source_path "${_source}" PATH)
  string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
  string(REPLACE "/" "\\" _group_path "${_group_path}")
  source_group("${_group_path}" FILES "${_source}")
endforeach ()

file(GLOB_RECURSE mainSources LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# set(OGRE_DIR C:/OgreSdk/CMake)
# set(OGRE_DIR C:/OgreSdk/ogre_src/vsbuild_debug/sdk/CMake)
set(OGRE_DIR C:/OgreSdk/installDEBUG/CMake)
find_package(OGRE 1.11 REQUIRED COMPONENTS Bites RTShaderSystem)
# file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${OGRE_CONFIG_DIR}/debug/lib/resources.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/debug_config)
#file(COPY ${OGRE_CONFIG_DIR}/debug/lib/plugins.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/debug_config)

#message(STATUS OGRE_STATIC ${OGRE_STATIC})
#message(STATUS OGRE_PLUGIN_DIR ${OGRE_PLUGIN_DIR})
message(STATUS OGRE_MEDIA_DIR ${OGRE_MEDIA_DIR})
message(STATUS "OGRE_CONFIG_DIR is  ${OGRE_CONFIG_DIR}")


add_executable(OgreGame ${mainSources})
target_link_libraries(OgreGame PRIVATE OgreBites OgreRTShaderSystem)