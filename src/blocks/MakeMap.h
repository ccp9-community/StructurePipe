/*
 * MakeMap.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MAKE_MAP_H
#define MAKE_MAP_H

// INCLUDES /////////////////////////////////////////////
#include "StructurePipe.h"

#include <AbstractSimpleBlock.h>


// FORWARD DECLARATIONS ////////////////////////////////////


namespace spipe { namespace blocks {

class MakeMap : public pipelib::AbstractSimpleBlock<StructureDataTyp, SharedDataTyp>
{
public:

	MakeMap();

	virtual void in(spipe::common::StructureData & data);

};


}}

#endif /* MAKE_MAP_H */
