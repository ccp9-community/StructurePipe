/*
 * NiggliReduction.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "blocks/NiggliReduction.h"

#include "common/StructureData.h"

// From SSTbx

#include <common/AbstractFmidCell.h>
#include <common/Structure.h>

// NAMESPACES ////////////////////////////////


namespace spipe { namespace blocks {

NiggliReduction::NiggliReduction():
Block("Niggli reduction")
{}

void NiggliReduction::in(spipe::common::StructureData & data)
{
	data.getStructure()->getUnitCell()->compactNiggli();

	myOutput->in(data);
}
}}