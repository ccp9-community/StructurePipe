# [muhrin] Downloaded from http://dev.pointclouds.org/projects/pcl/repository/show/trunk/cmake
# at 12:42, 24/06/12, all rights belong to them apart from to portions added by me.

###############################################################################
# Find QHULL
#
# This sets the following variables:
# QHULL_FOUND - True if QHULL was found.
# QHULL_INCLUDE_DIRS - Directories containing the QHULL include files.
# QHULL_LIBRARIES - Libraries needed to use QHULL.
# QHULL_DEFINITIONS - Compiler flags for QHULL.
# If QHULL_USE_STATIC is specified then look for static libraries ONLY else 
# look for shared ones

set(QHULL_MAJOR_VERSION 6)

if(QHULL_USE_STATIC)
  set(QHULL_RELEASE_NAME qhullstatic)
  set(QHULL_DEBUG_NAME qhullstatic_d)
  set(QHULLCPP_RELEASE_NAME qhullcpp)
  set(QHULLCPP_DEBUG_NAME qhullcpp_d)
else(QHULL_USE_STATIC)
  set(QHULL_RELEASE_NAME qhull qhull${QHULL_MAJOR_VERSION})
  set(QHULL_DEBUG_NAME qhull_d qhull${QHULL_MAJOR_VERSION}_d qhull_d${QHULL_MAJOR_VERSION})
  set(QHULLCPP_RELEASE_NAME qhullcpp qhullcpp${QHULL_MAJOR_VERSION})
  set(QHULLCPP_DEBUG_NAME qhullcpp_d qhullcpp${QHULL_MAJOR_VERSION}_d qhullcpp_d${QHULL_MAJOR_VERSION})
endif(QHULL_USE_STATIC)

find_file(QHULL_HEADER
          NAMES libqhull/libqhull.h qhull.h
          HINTS "${QHULL_ROOT}" "$ENV{QHULL_ROOT}" "${QHULL_INCLUDE_DIR}"
          PATHS "$ENV{PROGRAMFILES}/QHull" "$ENV{PROGRAMW6432}/QHull" 
          PATH_SUFFIXES qhull src/libqhull libqhull include)

set(QHULL_HEADER "${QHULL_HEADER}" CACHE INTERNAL "QHull header" FORCE )

if(QHULL_HEADER)
  get_filename_component(qhull_header ${QHULL_HEADER} NAME_WE)
  if("${qhull_header}" STREQUAL "qhull")
    set(HAVE_QHULL_2011 OFF)
    get_filename_component(QHULL_INCLUDE_DIR ${QHULL_HEADER} PATH)
  elseif("${qhull_header}" STREQUAL "libqhull")
    set(HAVE_QHULL_2011 ON)
    get_filename_component(QHULL_INCLUDE_DIR ${QHULL_HEADER} PATH)
    get_filename_component(QHULL_INCLUDE_DIR ${QHULL_INCLUDE_DIR} PATH)
  endif()
else(QHULL_HEADER)
  set(QHULL_INCLUDE_DIR "QHULL_INCLUDE_DIR-NOTFOUND")
endif(QHULL_HEADER)

set(QHULL_INCLUDE_DIR "${QHULL_INCLUDE_DIR}" CACHE PATH "QHull include dir." FORCE)

find_library(QHULL_LIBRARY 
             NAMES ${QHULL_RELEASE_NAME}
             HINTS "${QHULL_ROOT}" "$ENV{QHULL_ROOT}"
             PATHS "$ENV{PROGRAMFILES}/QHull" "$ENV{PROGRAMW6432}/QHull" 
             PATH_SUFFIXES project build bin lib)

find_library(QHULL_LIBRARY_DEBUG 
             NAMES ${QHULL_DEBUG_NAME} ${QHULL_RELEASE_NAME}
             HINTS "${QHULL_ROOT}" "$ENV{QHULL_ROOT}"
             PATHS "$ENV{PROGRAMFILES}/QHull" "$ENV{PROGRAMW6432}/QHull" 
             PATH_SUFFIXES project build bin lib)

if(NOT QHULL_LIBRARY_DEBUG)
  set(QHULL_LIBRARY_DEBUG ${QHULL_LIBRARY})
endif(NOT QHULL_LIBRARY_DEBUG)

set(QHULL_INCLUDE_DIRS ${QHULL_INCLUDE_DIR})
set(QHULL_LIBRARIES optimized ${QHULL_LIBRARY} debug ${QHULL_LIBRARY_DEBUG})

include(FindPackageHandleStandardArgs)
# [muhrin] Should this contain QHULL_LIBRARIES instead of LIBRARY?
find_package_handle_standard_args(Qhull DEFAULT_MSG QHULL_LIBRARY QHULL_INCLUDE_DIR)

## QHull C++ component check

foreach(COMPONENT ${Qhull_FIND_COMPONENTS})
  if(${COMPONENT} STREQUAL "QhullCpp") 
    find_file(QHULLCPP_HEADER
      NAMES libqhullcpp/QHull.h
      HINTS "${QHULL_ROOT}" "$ENV{QHULL_ROOT}" "${QHULL_INCLUDE_DIR}"
      PATHS "$ENV{PROGRAMFILES}/QHull" "$ENV{PROGRAMW6432}/QHull" 
      PATH_SUFFIXES src/libqhullcpp libqhullcpp include)

    find_library(QHULLCPP_LIBRARY 
      NAMES ${QHULLCPP_RELEASE_NAME}
      HINTS "${QHULL_ROOT}" "$ENV{QHULL_ROOT}"
      PATHS "$ENV{PROGRAMFILES}/QHull" "$ENV{PROGRAMW6432}/QHull" 
      PATH_SUFFIXES project build bin lib)

    find_library(QHULLCPP_LIBRARY_DEBUG 
      NAMES ${QHULLCPP_DEBUG_NAME} ${QHULLCPP_RELEASE_NAME}
      HINTS "${QHULL_ROOT}" "$ENV{QHULL_ROOT}"
      PATHS "$ENV{PROGRAMFILES}/QHull" "$ENV{PROGRAMW6432}/QHull" 
      PATH_SUFFIXES project build bin lib)

    if(NOT "${QHULLCPP_HEADER}" STREQUAL "" AND NOT "${QHULL_LIBRARY}" STREQUAL "")
      set(QHULL_QHULLCPP_FOUND true)
      set(QHULL_LIBRARIES optimized ${QHULL_LIBRARY} ${QHULLCPP_LIBRARY} debug ${QHULL_LIBRARY_DEBUG} ${QHULLCPP_LIBRARY_DEBUG})
    else()
      set(QHULL_FOUND false)
    endif()

  endif()
endforeach()

mark_as_advanced(QHULL_LIBRARY QHULL_LIBRARY_DEBUG QHULL_INCLUDE_DIR)

if(QHULL_FOUND)
  set(HAVE_QHULL ON)
  if(NOT QHULL_USE_STATIC)
    message(STATUS "Adding definition /Dqh_QHpointer")
    add_definitions(/Dqh_QHpointer)
    if(MSVC)
      add_definitions("-Dqh_QHpointer_dllimport")
    endif(MSVC)
  endif(NOT QHULL_USE_STATIC)
  message(STATUS "QHULL found (include: ${QHULL_INCLUDE_DIRS}, lib: ${QHULL_LIBRARIES})")
endif(QHULL_FOUND)
