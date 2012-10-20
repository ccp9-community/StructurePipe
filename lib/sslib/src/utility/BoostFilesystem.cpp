/*
 * BoostFilesystem.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "utility/BoostFilesystem.h"

namespace sstbx {
namespace utility {
namespace fs {

namespace boostfs = ::boost::filesystem;

// Return path when appended to a_From will resolve to same as a_To
boostfs::path make_relative(
  boostfs::path a_From,
  boostfs::path a_To )
{
  a_From = fs::absolute(a_From);
  a_To = fs::absolute(a_To);

  boostfs::path ret;
  boostfs::path::const_iterator itrFrom( a_From.begin() ), itrTo( a_To.begin() );
  
  // Find common base
  for(boostfs::path::const_iterator toEnd( a_To.end() ), fromEnd( a_From.end() ) ;
    itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo;
    ++itrFrom, ++itrTo );

  // Navigate backwards in directory to reach previously found base
  for( boostfs::path::const_iterator fromEnd( a_From.end() ); itrFrom != fromEnd; ++itrFrom )
  {
    if( (*itrFrom) != "." )
      ret /= "..";
  }
  // Now navigate down the directory branch
#ifdef SSLIB_USE_BOOSTFS_V2
  ret = ::sstbx::utility::append(ret, itrTo, a_To.end());
#else
  fs::append(ret, itrTo, a_To.end());
#endif
  return ret;
}

::std::string stemString(const boostfs::path & path)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return path.stem();
#else
  return path.stem().string();
#endif
}


boostfs::path append(
  boostfs::path appendTo,
  boostfs::path::iterator begin,
  boostfs::path::iterator end
)
{
  for( ; begin != end ; ++begin )
    appendTo /= *begin;
  return appendTo;
}

boostfs::path absolute(const boostfs::path & p)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return boostfs::system_complete(p);
#else
  return boostfs::absolute(p);
#endif
}

bool
isAbsolute(const boostfs::path & toCheck)
{
#ifdef SSLIB_USE_BOOSTFS_V2
  return toCheck.is_complete();
#else
  return toCheck.is_absolute();
#endif
}

}
}
}
