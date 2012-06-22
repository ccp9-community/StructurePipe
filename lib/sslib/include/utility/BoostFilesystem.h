/*
 * BoostFilesystem.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef BOOST_FILESYSTEM_H
#define BOOST_FILESYSTEM_H

// INCLUDES /////////////////////////////////////////////

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

// DEFINES ///////////////////////////////////////////////

/** Should we use version 2 of the boost filesystem library? */
#if (BOOST_VERSION / 100000) <= 1 && ((BOOST_VERSION / 100) % 1000) <= 45
#define SSLIB_USE_BOOSTFS_V2
#endif

// FORWARD DECLARATIONS ////////////////////////////////////

namespace boost
{
namespace filesystem3
{

template <>
path & path::append< typename path::iterator >(
  typename path::iterator begin,
  typename path::iterator end,
  const codecvt_type& cvt);

// Return path when appended to a_From will resolve to same as a_To
boost::filesystem::path make_relative(
  boost::filesystem::path a_From,
  boost::filesystem::path a_To );

}
}

namespace boost { namespace filesystem { using filesystem3::make_relative; } }

namespace sstbx
{
namespace utility
{

::std::string stemString(const ::boost::filesystem::path & path);


}
}

#endif /* BOOST_FILESYSTEM_H */
