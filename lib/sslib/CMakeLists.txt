cmake_minimum_required(VERSION 2.6)

project(SSLib)

# Include local settings
include(CMakeLocal.txt)

# Tell CMake where to find modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

set(SSLIB_VERSION_MAJOR 0)
set(SSLIB_VERSION_MAJOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/SSLibConfig.h.in"
  "${PROJECT_BINARY_DIR}/src/SSLibConfig.h"
  )



## FIND PACKAGES ########################################
# First add any user specified paths to the package search array
set(CMAKE_PREFIX_PATH "${BOOST_ROOT}" "${ARMADILLO_ROOT}")

# Boost #
if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
endif(WIN32)
find_package(Boost 1.36.0 COMPONENTS system filesystem REQUIRED)
# Armadillo #
find_package(Armadillo)
if("${ARMADILLO_INCLUDE_DIRS}" STREQUAL "")
  set(ARMADILLO_INCLUDE_DIRS "${ARMADILLO_ROOT}/include")
endif()

## END FIND PACKAGES ####################################

# Include directories with header files
  include_directories(
    ${Boost_INCLUDE_DIRS}
    ${ARMADILLO_INCLUDE_DIRS}
  )

include_directories(include src)

# Process header and source files
include(IncludeLists.txt)
include(SrcLists.txt)

set_property(TARGET PROPERTY PROJECT_LABEL "SSLib")

add_library(sslib ${sslib_Header_Files} ${sslib_Source_Files})

# Libraries we need to link to
target_link_libraries(sslib
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}	# filesystem depends on this
)
