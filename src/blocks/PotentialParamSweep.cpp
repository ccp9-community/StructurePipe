/*
 * PotentialParamSweep.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "blocks/PotentialParamSweep.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

// From SSTbx
#include <common/Structure.h>
#include <utility/MultiIdxRange.h>

#include "common/SharedData.h"
#include "common/StructureData.h"
#include "common/PipeFunctions.h"
#include "common/UtilityFunctions.h"

// NAMESPACES ////////////////////////////////

namespace spipe {
namespace blocks {

namespace fs = ::boost::filesystem;
namespace common = ::spipe::common;
namespace ssc = ::sstbx::common;
namespace ssio = ::sstbx::io;
namespace ssu = ::sstbx::utility;
namespace structure_properties = ssc::structure_properties;

PotentialParamSweep::PotentialParamSweep(
	const ::arma::vec	&		from,
	const ::arma::vec	&		step,
	const ::arma::Col<unsigned int> & nSteps,
	SpStartBlockTyp &		  sweepPipeline):
SpBlock("Potential param sweep"),
myFrom(from),
myStep(step),
myNSteps(nSteps),
mySweepPipeline(sweepPipeline),
myStepExtents(nSteps.n_rows)
{
	SP_ASSERT((myFrom.n_rows == myStep.n_rows) && (myFrom.n_rows == myNSteps.n_rows));

	myNumParams		= myNSteps.n_rows;
	for(size_t i = 0; i < myNumParams; ++i)
	{
		myStepExtents[i] = myNSteps(i);
	}
}

void PotentialParamSweep::pipelineInitialising()
{
	// Set the parameters in the shared data
	SharedDataTyp & sharedDat = getRunner()->memory().shared();
	sharedDat.potSweepFrom.reset(myFrom);
	sharedDat.potSweepStep.reset(myStep);
	sharedDat.potSweepNSteps.reset(myNSteps);

  myTableSupport.setFilename(getRunner()->memory().global().getOutputFileStem().string() + ".potparams");
  myTableSupport.registerRunner(*getRunner());
}

void PotentialParamSweep::start()
{
  ::std::string sweepPipeOutputPath;
  ::spipe::SharedDataTyp & sweepPipeSharedData = mySubpipeRunner->memory().shared();

  const ssu::MultiIdxRange<unsigned int> stepsRange(ssu::MultiIdx<unsigned int>(myStepExtents.dims()), myStepExtents);

  BOOST_FOREACH(const ssu::MultiIdx<unsigned int> & stepsIdx, stepsRange)
	{
		// Load the current potential parameters into the pipeline data
		::arma::vec params(myNumParams);
		for(size_t i = 0; i < myNumParams; ++i)
		{
			params(i) = myFrom(i) + (double)stepsIdx[i] * myStep(i);
		}
		// Store the potential parameters in global memory
    getRunner()->memory().global().objectsStore[::spipe::common::GlobalKeys::POTENTIAL_PARAMS] = params;

    // Set a directory for this set of parameters
    sweepPipeSharedData.appendToOutputDirName(common::generateUniqueName());

    // Get the relative path to where the pipeline write the structures to
    sweepPipeOutputPath = sweepPipeSharedData.getOutputPath(*getRunner()).string();

		// Start the sweep pipeline
		mySweepPipeline.start();

		// Send any finished structure data down my pipe
		releaseBufferedStructures(sweepPipeOutputPath);
	}
}

void PotentialParamSweep::runnerAttached(SpRunnerSetup & setup)
{
  mySubpipeRunner = setup.createChildRunner(mySweepPipeline);
	// Set outselves to collect any finished data from the sweep pipeline
	mySubpipeRunner->setFinishedDataSink(this);
}

void PotentialParamSweep::finished(SpStructureDataPtr data)
{
	// Copy over the parameters into the structure data
  const ::spipe::common::ObjectData<const ::arma::vec> result = ::spipe::common::getObjectConst(
    ::spipe::common::GlobalKeys::POTENTIAL_PARAMS,
    mySubpipeRunner->memory()
  );

  if(result.first != common::DataLocation::NONE)
  {
    data->objectsStore[common::GlobalKeys::POTENTIAL_PARAMS] = *result.second;
  }

	// Register the data with our pipeline to transfer ownership
	// Save it in the buffer for sending down the pipe
	myBuffer.push_back(&getRunner()->registerData(data));
}

void PotentialParamSweep::releaseBufferedStructures(
  const ::spipe::utility::DataTable::Key & key
)
{
	// Send any finished structure data down my pipe
	BOOST_FOREACH(StructureDataTyp * const sweepStrData, myBuffer)
	{
    updateTable(key, *sweepStrData);

		out(*sweepStrData);
	}
	myBuffer.clear();
}

void PotentialParamSweep::updateTable(const utility::DataTable::Key & key, const StructureDataTyp & sweepStrData)
{
  utility::DataTable & table = myTableSupport.getTable();

  const ::arma::vec * const params = sweepStrData.objectsStore.find(common::GlobalKeys::POTENTIAL_PARAMS);
  if(params)
  {
    // Update the table with the current parameters
    for(size_t i = 0; i < params->size(); ++i)
    {
      table.insert(
        key,
        "param" + ::boost::lexical_cast< ::std::string>(i),
        common::getString((*params)[i])
      );
    }
  }

  const ssc::Structure * const structure = sweepStrData.getStructure();
  if(structure)
  {
    const double * const internalEnergy = structure->getProperty(
      structure_properties::general::ENERGY_INTERNAL
      );
    if(internalEnergy)
    {
      const double energy = *internalEnergy;
      table.insert(
        key,
        "energy",
        common::getString(energy)
      );

      const size_t numAtoms = structure->getNumAtoms();
      table.insert(
        key,
        "energy/atom",
        common::getString(energy / numAtoms)
      );
    }

    const ssio::ResourceLocator locator = sweepStrData.getRelativeSavePath(*getRunner());
    if(!locator.empty())
    {
      table.insert(
        key,
        "path",
        locator.string()
      );
    }

    const unsigned int * const spacegroup = structure->getProperty(structure_properties::general::SPACEGROUP_NUMBER);
    if(spacegroup)
      table.insert(key, "sg", ::boost::lexical_cast< ::std::string>(*spacegroup));
  }
}

}
}

