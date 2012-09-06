/*
 * OrthoCellDistanceCalculator.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////////////////////////
#include "common/OrthoCellDistanceCalculator.h"

#include "common/Structure.h"
#include "common/UnitCell.h"
#include "utility/StableComparison.h"

namespace sstbx {
namespace common {

OrthoCellDistanceCalculator::OrthoCellDistanceCalculator(const sstbx::common::Structure &structure):
DistanceCalculator(structure)
{}

bool OrthoCellDistanceCalculator::getDistsBetween(
  const arma::vec3 & r1,
  const arma::vec3 & r2,
  const double cutoff,
  std::vector<double> &outDistances,
  const size_t maxDistances) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

  //const ::arma::vec3 r12 = getVecMinImg(r1, r2);
  const ::arma::vec3 r12 = cell.wrapVec(r2) - cell.wrapVec(r1);
  const double (&params)[6] = cell.getLatticeParams();

  // Maximum multiples of cell vectors we need to go to
  const int maxA = (int)ceil(cutoff / params[0]);
  const int maxB = (int)ceil(cutoff / params[1]);
  const int maxC = (int)ceil(cutoff / params[2]);

  const double cutoffSq = cutoff * cutoff;
  ::arma::vec3 dFrac, dRImg;
  const ::arma::mat33 & orthoMtx = cell.getOrthoMtx();
  double dRDistSq;
  size_t numDistances = 0;
	for(int a = -maxA; a <= maxA; ++a)
	{
		dFrac[0] = a;
		for(int b = -maxB; b <= maxB; ++b)
		{
		  dFrac[1] = b;
			for(int c = -maxC; c <= maxC; ++c)
			{
				dFrac[2] = c;

        dRImg = orthoMtx * dFrac + r12;
        dRDistSq = ::arma::dot(dRImg, dRImg);

				if(dRDistSq < cutoffSq)
				{
          outDistances.push_back(sqrt(dRDistSq));
          if(++numDistances >= maxDistances)
            return false;
				}
			}
		}
	}
  return true;
}

::arma::vec3 OrthoCellDistanceCalculator::getVecMinImg(const ::arma::vec3 & r1, const ::arma::vec3 & r2) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

  const ::arma::mat33 & fracMtx = cell.getFracMtx();
  const ::arma::mat33 & orthoMtx = cell.getOrthoMtx();

  ::arma::vec3 r12 = r2 - r1;

  r12 = fracMtx * r12;
  r12[0] -= floor(r12[0] + 0.5);
  r12[1] -= floor(r12[1] + 0.5);
  r12[2] -= floor(r12[2] + 0.5);
  r12 = orthoMtx * r12;

  return r12;
}

bool OrthoCellDistanceCalculator::getVecsBetween(
  const ::arma::vec3 & r1,
  const ::arma::vec3 & r2,
  const double cutoff,
  ::std::vector< ::arma::vec3> & outVectors,
  const size_t maxVectors) const
{
  const UnitCell & cell = *myStructure.getUnitCell();

  const ::arma::vec3 r12 = getVecMinImg(r1, r2);
  const double (&params)[6] = cell.getLatticeParams();

  // Maximum multiples of cell vectors we need to go to
  const int maxA = (int)ceil(cutoff / params[0]);
  const int maxB = (int)ceil(cutoff / params[1]);
  const int maxC = (int)ceil(cutoff / params[2]);

  const double cutoffSq = cutoff * cutoff;
  ::arma::vec3 dFrac, dRImg;
  const ::arma::mat33 & orthoMtx = cell.getOrthoMtx();
  double dRDistSq;
  size_t numVectors = 0;
	for(int a = -maxA; a <= maxA; ++a)
	{
		dFrac[0] = a;
		for(int b = -maxB; b <= maxB; ++b)
		{
		  dFrac[1] = b;
			for(int c = -maxC; c <= maxC; ++c)
			{
				dFrac[2] = c;

        dRImg = orthoMtx * dFrac + r12;
        dRDistSq = ::arma::dot(dRImg, dRImg);

				if(dRDistSq < cutoffSq)
				{
          outVectors.push_back(dRImg);
          if(++numVectors >= maxVectors)
            return false;
				}
			}
		}
	}
  return true;
}

bool OrthoCellDistanceCalculator::isValid() const
{
  if(!myStructure.getUnitCell())
    return false;

  const double (&params)[6] = myStructure.getUnitCell()->getLatticeParams();

  // All angles equal 90
  return
    utility::StableComp::eq(params[3], 90.0) &&
    utility::StableComp::eq(params[4], 90.0) &&
    utility::StableComp::eq(params[5], 90.0);
}

}
}
