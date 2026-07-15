# CMake config
cmake_minimum_required(VERSION 3.22)
project(cc)
set(CMAKE_C_STANDARD 11)
include(CMakeDependentOption)
cmake_dependent_option(CC_BUILD_SHARED "Build as a shared library" ON "BUILD_SHARED_LIBS" OFF)
cmake_dependent_option(CC_BUILD_TESTING "Build test units" ON "BUILD_TESTING" OFF)

# Gather sources
set(SOURCES 
	"${CMAKE_CURRENT_LIST_DIR}/src/deque.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/free_list.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/priority_queue.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/queue.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/stack.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/tree.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/unordered_map_str.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/unordered_map.c"
	"${CMAKE_CURRENT_LIST_DIR}/src/vector.c"
)
set(HEADERS 
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/deque.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/free_list.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/priority_queue.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/queue.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/stack.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/tree.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/unordered_map_str.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/unordered_map.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/vector.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/cc/common.h"
)

# Build interface
if (CC_BUILD_SHARED)
	add_library(cc SHARED ${SOURCES} ${HEADERS})
else()
	add_library(cc STATIC ${SOURCES} ${HEADERS})
endif()

# Include headers
target_include_directories(cc PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")