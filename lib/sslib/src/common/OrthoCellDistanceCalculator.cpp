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

const double OrthoCellDistanceCalculator::VALID_ANGLE_TOLERANCE = 1e-5;

OrthoCellDistanceCalculator::OrthoCellDistanceCalculator(const sstbx::common::Structure &structure):
DistanceCalculator(structure)
{
  updateBufferedValues();
}

bool OrthoCellDistanceCalculator::getDistsBetween(
  const arma::vec3 & r1,
  const arma::vec3 & r2,
  double cutoff,
  std::vector<double> &outDistances,
  const size_t maxDistances) const
{
  // The cutoff has to be positive
  cutoff = abs(cutoff);
  const UnitCell & cell = *myStructure.getUnitCell();

  const ::arma::vec3 r12 = cell.wrapVec(r2) - cell.wrapVec(r1);
  const double (&params)[6] = cell.getLatticeParams();

  const double cutoffSq = cutoff * cutoff;

  const double rDotA =  ::arma::dot(r12, myANorm);
  const double rDotB = ::arma::dot(r12, myBNorm);
  const double rDotC = ::arma::dot(r12, myCNorm);

  // Maximum multiples of cell vectors we need to go to
  const int A_min = -(int)floor((cutoff + rDotA) * myARecip);
  const int A_max = (int)floor((cutoff - rDotA) * myARecip);
  int B_min = -(int)floor((cutoff + rDotB) * myBRecip);
  int B_max = (int)floor((cutoff - rDotB) * myBRecip);
  int C_min = -(int)floor((cutoff + rDotC) * myCRecip);
  int C_max = (int)floor((cutoff - rDotC) * myCRecip);

  const bool doFullDistanceCheck = worthDoingAccurately(A_max - A_min, B_max - B_min, C_max - C_min);

  // Loop variables
  size_t numDistances = 0;
  //double dRDistSq;
  ::arma::vec3 nA, nAPlusNB, dRImg;
  double r_x, r_y, r_z, aSq, bSq, testDistSq;

  //if(doFullDistanceCheck)
  //{
	 // for(int a = A_min; a <= A_max; ++a)
	 // {
  //    nA = a * myA;

  //    aSq = a * params[0] + rDotA;
  //    aSq *= aSq;
  //    B_min = -(int)floor((sqrt(cutoffSq + aSq) + rDotB) * myBRecip);
  //    B_max = (int)floor((sqrt(cutoffSq - aSq) - rDotB) * myBRecip);
  //    for(int b = B_min; b <= B_max; ++b)
	//    {
  //      nAPlusNB = nA + b * myB;

  //      bSq = b * params[1] + rDotB;
  //      bSq *= bSq;
  //      C_min = -(int)floor((sqrt(cutoffSq + aSq + bSq) + rDotC) * myCRecip);
  //      C_max = (int)floor((sqrt(cutoffSq - aSq - bSq) - rDotC) * myCRecip);
  //	    for(int c = C_min; c <= C_max; ++c)
  //	    {
  //        dRImg = c * myC + nAPlusNB + r12;
  //        dRDistSq = ::arma::dot(dRImg, dRImg);

		//		  if(dRDistSq < cutoffSq)
		//		  {
  //          outDistances.push_back(sqrt(dRDistSq));
  //          if(++numDistances >= maxDistances)
  //            return false;
		//		  }
		//	  }
		//  }
	 // }
  //}
  //else
  {
    for(int a = A_min; a <= A_max; ++a)
    {
      r_x = a * params[0] + rDotA;
      aSq = r_x * r_x;
	    for(int b = B_min; b <= B_max; ++b)
      {
        r_y = b * params[1] + rDotB;
        bSq = r_y * r_y;
        if(aSq + bSq < cutoffSq)
        {
		      for(int c = C_min; c <= C_max; ++c)
		      {
            r_z = c * params[2] + rDotC;
            testDistSq = aSq + bSq + r_z * r_z;

            if(testDistSq < cutoffSq)
			      {
              outDistances.push_back(sqrt(testDistSq));
              if(++numDistances >= maxDistances)
                return false;
			      }
          }
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
  double cutoff,
  ::std::vector< ::arma::vec3> & outVectors,
  const size_t maxValues) const
{
  // The cutoff has to be positive
  cutoff = abs(cutoff);
  const UnitCell & cell = *myStructure.getUnitCell();

  const ::arma::vec3 r12 = cell.wrapVec(r2) - cell.wrapVec(r1);
  const double (&params)[6] = cell.getLatticeParams();

  const double cutoffSq = cutoff * cutoff;

  const double rDotA =  ::arma::dot(r12, myANorm);
  const double rDotB = ::arma::dot(r12, myBNorm);
  const double rDotC = ::arma::dot(r12, myCNorm);

  // Maximum multiples of cell vectors we need to go to
  const int A_min = -(int)floor((cutoff + rDotA) * myARecip);
  const int A_max = (int)floor((cutoff - rDotA) * myARecip);
  int B_min = -(int)floor((cutoff + rDotB) * myBRecip);
  int B_max = (int)floor((cutoff - rDotB) * myBRecip);
  int C_min = -(int)floor((cutoff + rDotC) * myCRecip);
  int C_max = (int)floor((cutoff - rDotC) * myCRecip);

  // Loop variables
  size_t numVectors = 0;
  ::arma::vec3 dR, outVec;
  double r_x, r_y, r_z, aSq, bSq, testDistSq;
  for(int a = A_min; a <= A_max; ++a)
  {
    r_x = a * params[0] + rDotA;
    aSq = r_x * r_x;
	  for(int b = B_min; b <= B_max; ++b)
    {
      r_y = b * params[1] + rDotB;
      bSq = r_y * r_y;
      if(aSq + bSq < cutoffSq)
      {
		    for(int c = C_min; c <= C_max; ++c)
		    {
          r_z = c * params[2] + rDotC;
          testDistSq = aSq + bSq + r_z * r_z;

          if(testDistSq < cutoffSq)
			    {
            outVec[0] = r_x;
            outVec[1] = r_y;
            outVec[2] = r_z;
            outVectors.push_back(outVec);
            if(++numVectors >= maxValues)
              return false;
			    }
        }
      }
    }
  }

  return true;

  //double dRDistSq;
	//for(int a = A_min; a <= A_max; ++a)
	//{
 //   nA = a * myA;

 //   aSq = a * params[0] + rDotA;
 //   aSq *= aSq;
 //   B_min = -(int)floor((sqrt(cutoffSq + aSq) + rDotB) / params[1]);
 //   B_max = (int)floor((sqrt(cutoffSq - aSq) - rDotB) / params[1]);
	//	for(int b = B_min; b <= B_max; ++b)
	//	{
 //     nAPlusNB = nA + b * myB;

 //     bSq = b * params[1] + rDotB;
 //     bSq *= bSq;
 //     C_min = -(int)floor((sqrt(cutoffSq + aSq + bSq) + rDotC) / params[2]);
 //     C_max = (int)floor((sqrt(cutoffSq - aSq - bSq) - rDotC) / params[2]);
	//		for(int c = C_min; c <= C_max; ++c)
	//		{
 //       dRImg = c * myC + nAPlusNB + r12;
 //       dRDistSq = ::arma::dot(dRImg, dRImg);

	//			if(dRDistSq < cutoffSq)
	//			{
 //         outVectors.push_back(dRImg);
 //         if(++numVectors >= maxVectors)
 //           return false;
	//			}
	//		}
	//	}
	//}
}

bool OrthoCellDistanceCalculator::isValid() const
{
  if(!myStructure.getUnitCell())
    return false;

  const double (&params)[6] = myStructure.getUnitCell()->getLatticeParams();

  // All angles equal 90
  return
    utility::StableComp::eq(params[3], 90.0, VALID_ANGLE_TOLERANCE) &&
    utility::StableComp::eq(params[4], 90.0, VALID_ANGLE_TOLERANCE) &&
    utility::StableComp::eq(params[5], 90.0, VALID_ANGLE_TOLERANCE);
}

void OrthoCellDistanceCalculator::unitCellChanged()
{
  updateBufferedValues();
}

void OrthoCellDistanceCalculator::updateBufferedValues()
{
  const UnitCell & cell = *myStructure.getUnitCell();
  const double (&params)[6] = cell.getLatticeParams();

  myA = cell.getAVec();
  myB = cell.getBVec();
  myC = cell.getCVec();

  myANorm = myA / params[0];
  myBNorm = myB / params[1];
  myCNorm = myC / params[2];

  myARecip = 1.0 / params[0];
  myBRecip = 1.0 / params[1];
  myCRecip = 1.0 / params[2];
}

bool OrthoCellDistanceCalculator::worthDoingAccurately(const int dA, const int dB, const int dC) const
{
  int max = ::std::max(dA, ::std::max(dB, dC));

  return max > 250;
}

}
}
