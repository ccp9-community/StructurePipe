/*
 * DistanceCalculatorDelegator.h
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef DISTANCE_CALCULATOR_DELEGATOR_H
#define DISTANCE_CALCULATOR_DELEGATOR_H

// INCLUDES ///////////////////////////////////
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>

#include <armadillo>

#include "common/DistanceCalculator.h"

namespace sstbx {
namespace common {

// FORWARD DECLARES ///////////////////////////
class Atom;
class Structure;

class DistanceCalculatorDelegator : public DistanceCalculator
{
public:

  DistanceCalculatorDelegator(const Structure & structure);

  DistanceCalculatorDelegator(const DistanceCalculatorDelegator & toCopy);

  // From DistanceCalculator /////////////////////

  virtual inline double getDistMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b) const
  { return myDelegate->getDistMinImg(a, b); }

  virtual inline double getDistMinImg(const Atom & atom1, const Atom & atom2) const
  { return myDelegate->getDistMinImg(atom1, atom2); }

  virtual inline double getDistSqMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b) const
  { return myDelegate->getDistSqMinImg(a, b); }

  virtual inline double getDistSqMinImg(const Atom & atom1, const Atom & atom2) const
  { return myDelegate->getDistSqMinImg(atom1, atom2); }

  virtual inline bool getDistsBetween(
    const ::arma::vec3 & a,
    const ::arma::vec3 & b,
    const double cutoff,
    ::std::vector<double> & outDistances,
    const size_t maxDistances = DEFAULT_MAX_OUTPUTS) const
  { return myDelegate->getDistsBetween(a, b, cutoff, outDistances, maxDistances); }

  virtual inline bool getDistsBetween(
    const Atom & atom1,
    const Atom & atom2,
    const double cutoff,
    ::std::vector<double> & outDistances,
    const size_t maxDistances = DEFAULT_MAX_OUTPUTS) const
  { return myDelegate->getDistsBetween(atom1, atom2, cutoff, outDistances, maxDistances); }

  virtual ::arma::vec3 getVecMinImg(const ::arma::vec3 & a, const ::arma::vec3 & b) const
  { return myDelegate->getVecMinImg(a, b); }

  virtual ::arma::vec3 getVecMinImg(const Atom & atom1, const Atom & atom2) const
  { return myDelegate->getVecMinImg(atom1, atom2); }

  virtual bool getVecsBetween(
    const ::arma::vec3 & a,
    const ::arma::vec3 & b,
    const double cutoff,
    ::std::vector< ::arma::vec3> & outVectors,
    const size_t maxVectors = DEFAULT_MAX_OUTPUTS) const
  { return myDelegate->getVecsBetween(a, b, cutoff, outVectors, maxVectors); }

  virtual bool getVecsBetween(
    const Atom & atom1,
    const Atom & atom2,
    const double cutoff,
    ::std::vector< ::arma::vec3> & outVectors,
    const size_t maxVectors = DEFAULT_MAX_OUTPUTS) const
  { return myDelegate->getVecsBetween(atom1, atom2, cutoff, outVectors, maxVectors); }

  bool isValid() const
  { return myDelegate->isValid(); }

  // End from DistanceCalculator /////////////////

  void unitCellChanged();

private:

  struct CalculatorType
  {
    enum Value
    {
      NONE,
      CLUSTER,
      UNIVERSAL_CRYSTAL,
      ORTHO_CELL
    };
  };

  typedef ::boost::scoped_ptr<DistanceCalculator> DistanceCalculatorPtr;

  void updateDelegate();

  void setDelegate(const CalculatorType::Value calcType);

  DistanceCalculatorPtr   myDelegate;
  CalculatorType::Value   myDelegateType;

};

}
} // Close the namespace

#endif /* DISTANCE_CALCULATOR_DELEGATOR_H */
