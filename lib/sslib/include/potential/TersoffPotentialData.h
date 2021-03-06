/*
 * TersoffPotentialData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef TERSOFF_POTENTIAL_DATA_H
#define TERSOFF_POTENTIAL_DATA_H

// INCLUDES /////////////////////////////////////////////
#include "StandardData.h"

#include "common/Structure.h"

// FORWARD DECLARATIONS ////////////////////////////////////


// DEFINITION //////////////////////////////////////////////

namespace sstbx { namespace potential {

template <typename FloatType = double>
struct TersoffPotentialData : public StandardData<FloatType>
{
	TersoffPotentialData(const sstbx::common::Structure & structure);

};

// IMPLEMENTATION //////////////////////////////////////////
template <typename FloatType>
TersoffPotentialData<FloatType>::TersoffPotentialData(const sstbx::common::Structure & structure):
StandardData<FloatType>(structure)
{
}

}}

#endif /* TERSOFF_POTENTIAL_DATA_H */
