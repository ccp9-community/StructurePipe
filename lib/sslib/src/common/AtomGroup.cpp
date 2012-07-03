/*
 * AtomGroup.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////
#include "SSLib.h"
#include "common/AtomGroup.h"

#include <set>

#include <boost/foreach.hpp>

// Local includes
#include "common/Atom.h"
#include "common/StructureTreeEvent.h"


namespace sstbx
{
namespace common
{

AtomGroup::AtomGroup():
myParent(NULL)
{}

const AtomGroup::Vec3 & AtomGroup::getPosition() const
{
	return position;
}

void AtomGroup::setPosition(const AtomGroup::Vec3 & pos)
{
	position = pos;
}

void AtomGroup::getAtomPositions(Mat & posMtx, const size_t startCol) const
{
	const size_t numAtoms = myAtoms.size();

	// Make sure the matrix of the right size
	if(posMtx.n_rows != 3 || posMtx.n_cols < numAtoms)
		posMtx.set_size(3, startCol + numAtoms);
	
	for(size_t i = startCol; i < startCol + numAtoms; ++i)
	{
		posMtx.col(i) = myAtoms[i]->getPosition();
	}
}

void AtomGroup::setAtomPositions(const Mat & posMtx, const size_t startCol)
{
	const size_t numAtoms = myAtoms.size();

	// Make sure the matrix has the correct proportions
	SSE_ASSERT(posMtx.n_rows == 3 && posMtx.n_cols >= (numAtoms + startCol));

	for(size_t i = startCol; i < numAtoms + startCol; ++i)
	{
		myAtoms[i]->setPosition(posMtx.col(i));
	}
}

void AtomGroup::getAtomPositionsDescendent(Mat & posMtx, const size_t startCol) const
{
	using namespace std;

	const size_t numAtoms		= getNumAtomsDescendent();

	// Make sure the matrix is big enough
	if(posMtx.n_rows != 3 || posMtx.n_cols < startCol + numAtoms)
		posMtx.set_size(3, startCol + numAtoms);

	// First get my atoms
	const size_t numGroupAtoms	= getNumAtoms();
	getAtomPositions(posMtx, startCol);

	size_t currentCol = startCol + numGroupAtoms;
	// Now all the atoms of my child groups
  const AtomGroup * childGroup;
  size_t numChildGroupAtoms;
  AtomGroup::Vec3 groupPos;
	for(vector<AtomGroup *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		childGroup = *it;
		numChildGroupAtoms = childGroup->getNumAtomsDescendent();

		childGroup->getAtomPositionsDescendent(posMtx, currentCol);

		// Now displace them by the current group position
		groupPos = childGroup->getPosition();
		for(size_t i = currentCol; i < currentCol + numChildGroupAtoms; ++i)
		{
			posMtx.col(i) += groupPos;
		}
		currentCol += numChildGroupAtoms;
	}
}

void AtomGroup::setAtomPositionsDescendent(const Mat & posMtx, const size_t startCol)
{
	using ::std::vector;

	const size_t numPositions	= posMtx.n_cols - startCol;
	const size_t myNumAtoms		= myAtoms.size();

	// First set my atoms
	setAtomPositions(posMtx, startCol);

	// Now all the atoms of my child groups
	size_t currentPos = myNumAtoms + startCol;
	for(vector<AtomGroup *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		AtomGroup * const childGroup = *it;
		const size_t childNumAtoms = childGroup->getNumAtomsDescendent();

		Mat childPositions = posMtx.cols(currentPos, currentPos + childNumAtoms - 1);

		// Now displace them by the current group position
		const Vec3 groupPos = childGroup->getPosition();
		for(size_t i = 0; i < childPositions.n_cols; ++i)
		{
			childPositions.col(i) -= groupPos;
		}

		// Finally set the positions
		childGroup->setAtomPositionsDescendent(childPositions, currentPos);

		currentPos += childNumAtoms;
	}

}

size_t AtomGroup::getNumAtoms() const
{
	return myAtoms.size();
}

size_t AtomGroup::getNumAtomsDescendent() const
{
	using namespace std;

	size_t totalAtoms = getNumAtoms();

	for(vector<AtomGroup *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		totalAtoms += (*it)->getNumAtoms();
	}

	return totalAtoms;
}

size_t AtomGroup::getNumAtomSpecies() const
{
  ::std::set<AtomSpeciesId::Value> speciesSet;
  AtomConstPtr atom;
	BOOST_FOREACH(atom, myAtoms)
	{
		speciesSet.insert(atom->getSpecies());
	}
	return speciesSet.size();
}

size_t AtomGroup::getNumAtomSpeciesDescendent() const
{
  ::std::vector<AtomSpeciesId::Value> species;
	getAtomSpeciesDescendent(species);

  ::std::set<AtomSpeciesId::Value> speciesSet;
	speciesSet.insert(species.begin(), species.end());

	return speciesSet.size();
}

size_t AtomGroup::getNumAtomsOfSpecies(const AtomSpeciesId::Value species) const
{
  size_t numAtoms = 0;
  AtomConstPtr atom;
  BOOST_FOREACH(atom, myAtoms)
  {
    if(atom->getSpecies() == species)
      ++numAtoms;
  }
  return numAtoms;
}

size_t AtomGroup::getNumAtomsOfSpeciesDescendent(const AtomSpeciesId::Value species) const
{
  size_t numAtoms = 0;
  BOOST_FOREACH(const AtomGroup * const g, groups)
  {
    numAtoms += g->getNumAtomsOfSpeciesDescendent(species);
  }
  numAtoms += getNumAtomsOfSpecies(species);

  return numAtoms;
}


void AtomGroup::getAtomSpecies(::std::vector<AtomSpeciesId::Value> & species) const
{
  AtomConstPtr atom;
	BOOST_FOREACH(atom, myAtoms)
	{
		species.push_back(atom->getSpecies());
	}
}

void AtomGroup::getAtomSpeciesDescendent(::std::vector<AtomSpeciesId::Value> & species) const
{
	// First do mine...
	getAtomSpecies(species);

	// Now all the child groups
	for(size_t i = 0; i < groups.size(); ++i)
	{
		getAtomSpeciesDescendent(species);
	}
}

const std::vector<AtomPtr> & AtomGroup::getAtoms() const
{
	return myAtoms;
}

void AtomGroup::insertAtom(Atom * const atom)
{
	myAtoms.push_back(AtomPtr(atom));

	atom->setParent(this);

	StructureTreeEvent evt(StructureTreeEvent::ATOM_INSERTED, *atom);
	eventFired(evt);
}

bool AtomGroup::removeAtom(const AtomPtr atom)
{
	::std::vector<AtomPtr>::iterator it = ::std::find(myAtoms.begin(), myAtoms.end(), atom);

	if(it == myAtoms.end()) return false;

	atom->setParent(NULL);
	myAtoms.erase(it);

	StructureTreeEvent evt(StructureTreeEvent::ATOM_REMOVED, *atom);
	eventFired(evt);

	return true;
}

Atom & AtomGroup::getAtom(const size_t idx)
{
  SSE_ASSERT(idx < getNumAtoms());

  return *myAtoms[idx].get();
}

const Atom & AtomGroup::getAtom(const size_t idx) const
{
  SSE_ASSERT(idx < getNumAtoms());

  return *myAtoms[idx].get();
}

Atom & AtomGroup::getAtomDescendent(const size_t idx)
{
  SSE_ASSERT(idx < getNumAtomsDescendent());

  // Is it in our set of atoms?
  size_t runningTotal = getNumAtoms();
  if(idx < runningTotal)
  {
    return *myAtoms[idx].get();
  }
  
  AtomGroup * group;
  Atom * atom;
  size_t numAtomsInGroup;
  for(size_t i = 0; i < groups.size(); ++i)
  {
    group = groups[i];
    numAtomsInGroup = group->getNumAtomsDescendent();

    if(idx < runningTotal + numAtomsInGroup)
    {
      atom = &group->getAtomDescendent(idx - runningTotal);
    }
  }
  return *atom;
}

const Atom & AtomGroup::getAtomDescendent(const size_t idx) const
{
  SSE_ASSERT(idx < getNumAtomsDescendent());

  // Is it in our set of atoms?
  size_t runningTotal = getNumAtoms();
  if(idx < runningTotal)
  {
    return *myAtoms[idx].get();
  }
  
  AtomGroup * group;
  Atom * atom;
  size_t numAtomsInGroup;
  for(size_t i = 0; i < groups.size(); ++i)
  {
    group = groups[i];
    numAtomsInGroup = group->getNumAtomsDescendent();

    if(idx < runningTotal + numAtomsInGroup)
    {
      atom = &group->getAtomDescendent(idx - runningTotal);
    }
  }
  return *atom;
}

const std::vector<AtomGroup *> & AtomGroup::getGroups() const
{
	return groups;
}

void AtomGroup::insertGroup(AtomGroup * const childGroup)
{
	groups.push_back(childGroup);

	childGroup->setParent(this);

	StructureTreeEvent evt(StructureTreeEvent::GROUP_INSERTED, *childGroup);
}

bool AtomGroup::removeGroup(AtomGroup * const childGroup)
{
	using namespace std;
	vector<AtomGroup *>::iterator it = find(groups.begin(), groups.end(), childGroup);

	// If we couldn't find the child then return false
	if(it == groups.end()) return false;

	childGroup->setParent(NULL);
	groups.erase(it);

	StructureTreeEvent evt(StructureTreeEvent::GROUP_REMOVED, *childGroup);
	eventFired(evt);

	return true;
}

void AtomGroup::setParent(sstbx::common::AtomGroup *const parent)
{
	myParent = parent;
}

void AtomGroup::eventFired(const StructureTreeEvent & evt)
{
	// Propagate this to my parent
	if(myParent)
	{
		myParent->eventFired(evt);
	}
}

}}
