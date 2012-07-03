/*
 * AtomGroup.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_GROUP_H_
#define ATOM_GROUP_H_

// INCLUDES ///////////////////////////////////

#include <vector>

#include <boost/shared_ptr.hpp>

#include <armadillo>

#include "common/AtomSpeciesId.h"
#include "common/Types.h"
#include "common/event/AtomGroupEvent.h"
#include "common/event/GenericEventSupport.h"
#include "common/event/IGenericEventListener.h"

// FORWARD DECLARES ///////////////////////////
namespace sstbx
{
namespace common
{
	class Atom;
	class StructureTreeEvent;
}
}

namespace sstbx
{
namespace common
{

class AtomGroup
{
public:

	typedef arma::Col<double>::fixed<3>	Vec3;
	typedef arma::Mat<double>			Mat;

	friend class Atom;

	AtomGroup();

	// GROUP POSITION //////////////////////////

	/** Get the position vector for the group. */
	const Vec3 & getPosition() const;

	/** Set the position vector for the group. */
	void setPosition(const Vec3 & pos);

	// ATOMS ///////////////////////////////////

	const std::vector<AtomPtr> & getAtoms() const;
	void insertAtom(Atom * const atom);
	bool removeAtom(const AtomPtr atom);

  Atom & getAtom(const size_t idx);
  const Atom & getAtom(const size_t idx) const;
  Atom & getAtomDescendent(const size_t idx);
  const Atom & getAtomDescendent(const size_t idx) const;

	// Positions
	void getAtomPositions(Mat & posMtx, const size_t startCol = 0) const;
	void setAtomPositions(const Mat & posMtx, const size_t startCol = 0);

	virtual void getAtomPositionsDescendent(Mat & posMtx, const size_t startCol = 0) const;
	virtual void setAtomPositionsDescendent(const Mat & posMtx, const size_t startCol = 0);

	// Species
	virtual size_t getNumAtomSpecies() const;
	virtual size_t getNumAtomSpeciesDescendent() const;
  virtual size_t getNumAtomsOfSpecies(const AtomSpeciesId::Value species) const;
  virtual size_t getNumAtomsOfSpeciesDescendent(const AtomSpeciesId::Value species) const;
  virtual void getAtomSpecies(::std::vector<AtomSpeciesId::Value> & species) const;
  virtual void getAtomSpeciesDescendent(::std::vector<AtomSpeciesId::Value> & species) const;

	/**
	/* Get the number of atoms in this atom group.
	/**/
	virtual size_t getNumAtoms() const;

	/**
	/* Get the number of atoms in this group and all those below it
	/**/
	virtual size_t getNumAtomsDescendent() const;

	// CHILD GROUPS ////////////////////////////////////////////

	const std::vector<AtomGroup *> & getGroups() const;
	virtual void insertGroup(AtomGroup * const childGroup);
	virtual bool removeGroup(AtomGroup * const childGroup);

protected:

	void setParent(AtomGroup * const parent);

	virtual void eventFired(const StructureTreeEvent & evt);

	AtomGroup *                     myParent;

	/** The atoms contained in this group */
	std::vector<AtomPtr>            myAtoms;

	/** The position of the atom group relative to its parent */
	Vec3                            position;

	/** The child atom groups */
	::std::vector<AtomGroup *>			groups;
};

}}

#endif /* ATOM_GROUP_H_ */
