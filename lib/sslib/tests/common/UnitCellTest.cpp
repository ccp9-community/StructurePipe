/*
 * UnitCellTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include <armadillo>

#include <build_cell/RandomUnitCell.h>
#include <common/UnitCell.h>
#include <utility/StableComparison.h>

namespace ssbc = ::sstbx::build_cell;
namespace ssc = ::sstbx::common;
namespace ssu = ::sstbx::utility;

BOOST_AUTO_TEST_CASE(LongestDiagonal)
{
  const size_t numUnitCells = 100;

  ssbc::RandomUnitCell cellGen;

  ::arma::vec3 A, B, C, combination, longest, diag;
  double lengthSq, maxLengthSq;
  for(size_t i = 0; i < numUnitCells; ++i)
  {
    ::boost::shared_ptr<ssc::UnitCell> cell = cellGen.generateCell();

    // Get the longest cell vector
    A = cell->getAVec();
    B = cell->getBVec();
    C = cell->getCVec();
    
    longest = A + B + C;
    maxLengthSq = ::arma::dot(longest, longest);

    combination = A + B - C;
    lengthSq = ::arma::dot(combination, combination);
    if(lengthSq > maxLengthSq)
    {
      longest = combination;
      maxLengthSq = lengthSq;
    }

    combination = A - B + C;
    lengthSq = ::arma::dot(combination, combination);
    if(lengthSq > maxLengthSq)
    {
      longest = combination;
      maxLengthSq = lengthSq;
    }

    combination = A - B - C;
    lengthSq = ::arma::dot(combination, combination);
    if(lengthSq > maxLengthSq)
    {
      longest = combination;
      maxLengthSq = lengthSq;
    }

    diag = cell->getLongestDiagonal();
    const double diagLengthSq = ::arma::dot(diag, diag);

    BOOST_REQUIRE(ssu::StableComp::eq(maxLengthSq, ::arma::dot(diag, diag)));
  }
}
