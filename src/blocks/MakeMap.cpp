/*
 * MakeMap.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "blocks/MakeMap.h"

#include "common/StructureData.h"

// From Pipelib
#include <IPipeline.h>

// NAMESPACES ////////////////////////////////


namespace spipe { namespace blocks {

MakeMap::MakeMap():
Block("Make map")
{}

void MakeMap::in(spipe::common::StructureData & data)
{
	myOutput->in(data);
}

}}