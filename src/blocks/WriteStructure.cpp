/*
 * WriteStructure.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "blocks/WriteStructure.h"

#include <pipelib/IPipeline.h>

// From SSTbx
#include <io/StructureWriterManager.h>
#include <utility/BoostFilesystem.h>

// From local
#include "common/SharedData.h"
#include "common/StructureData.h"
#include "common/UtilityFunctions.h"


// NAMESPACES ////////////////////////////////


namespace spipe {
namespace blocks {

WriteStructure::WriteStructure(const ::sstbx::io::StructureWriterManager & writerManager):
pipelib::Block<StructureDataTyp, SharedDataTyp>("Write structures"),
myWriterManager(writerManager)
{}

void WriteStructure::in(::spipe::common::StructureData & data)
{
  namespace fs = ::boost::filesystem;
  namespace ssu = ::sstbx::utility;

	// Check if the structure has a name already, otherwise give it one
	if(!data.name)
	{
		data.name.reset(::spipe::common::generateUniqueName());
	}

	// Create the path to store the structure
	fs::path p(*data.name + ".res");

  // Prepend the pipe output path
  p = myPipeline->getSharedData().getOutputPath() / p;
	
  if(myWriterManager.writeStructure(*data.getStructure(), p, myPipeline->getGlobalData().getSpeciesDatabase()))
  {
    // Save the location that the file was written
    data.objectsStore[::spipe::common::StructureObjectKeys::LAST_ABS_SAVE_PATH] = ::sstbx::utility::absolute(p);
  }

	myOutput->in(data);
}

}}

