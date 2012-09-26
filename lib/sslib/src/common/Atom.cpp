/*
 * Atom.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/Atom.h"

#include "common/Structure.h"

namespace sstbx {
namespace common {

const Structure & Atom::getStructure() const
{
  return myStructure;
}

Structure & Atom::getStructure()
{
  return myStructure;
}

const ::arma::vec3 & Atom::getPosition() const
{
	return myPosition;
}

void Atom::setPosition(const ::arma::vec3 & pos)
{
	myPosition = pos;
  myStructure.atomMoved(*this);
}

void Atom::setPosition(const double x, const double y, const double z)
{
	myPosition(0) = x;
  myPosition(1) = y;
  myPosition(2) = z;
  myStructure.atomMoved(*this);
}

double Atom::getRadius() const
{
  return myRadius;
}

void Atom::setRadius(const double radius)
{
  myRadius = radius;
}

const AtomSpeciesId::Value  Atom::getSpecies() const
{
	return mySpecies;
}

size_t Atom::getIndex() const
{
  return myIndex;
}

Atom::Atom(const AtomSpeciesId::Value species, Structure & structure, const size_t index):
mySpecies(species),
myStructure(structure),
myIndex(index),
myRadius(-1.0)
{}

Atom::Atom(const Atom & toCopy, Structure & structure, const size_t index):
mySpecies(toCopy.getSpecies()),
myRadius(toCopy.getRadius()),
myPosition(toCopy.getPosition()),
myStructure(structure),
myIndex(index)
{}

void Atom::setIndex(const size_t index)
{
  myIndex = index;
}

}
}
