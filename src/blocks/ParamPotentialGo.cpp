/*
 * ParamPotentialGo.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "blocks/ParamPotentialGo.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

// From SSTbx
#include <common/Structure.h>
#include <potential/PotentialData.h>
#include <potential/IGeomOptimiser.h>
#include <potential/IParameterisable.h>

#include "common/PipeFunctions.h"
#include "common/StructureData.h"
#include "common/SharedData.h"
#include "common/UtilityFunctions.h"

// NAMESPACES ////////////////////////////////


namespace spipe {
namespace blocks {

namespace fs = ::boost::filesystem;
namespace common = ::spipe::common;
namespace utility = ::spipe::utility;

ParamPotentialGo::ParamPotentialGo(
	::sstbx::potential::IParameterisable & paramPotential,
	const ::sstbx::potential::IGeomOptimiser & optimiser,
  const bool                  writeOutput):
pipelib::Block<StructureDataTyp, SharedDataTyp, SharedDataTyp>("Parameterised potential geometry optimisation"),
PotentialGo(optimiser, writeOutput),
myParamPotential(paramPotential)
{}

ParamPotentialGo::ParamPotentialGo(
	::sstbx::potential::IParameterisable & paramPotential,
	const ::sstbx::potential::IGeomOptimiser & optimiser,
  const ::sstbx::potential::OptimisationSettings & optimisationParams,
  const bool                  writeOutput):
pipelib::Block<StructureDataTyp, SharedDataTyp, SharedDataTyp>("Parameterised potential geometry optimisation"),
PotentialGo(optimiser, optimisationParams, writeOutput),
myParamPotential(paramPotential)
{}

void ParamPotentialGo::pipelineStarting()
{
  // The pipeline is starting so try and get the potential parameters
  common::ObjectData<arma::vec>
    params = common::getObject(common::GlobalKeys::POTENTIAL_PARAMS, getRunner()->memory());

  if(params.first != common::DataLocation::NONE)
  {
    // Need to get the actual parameters as the potential may use a combining rule or change
    // them in some way from those specified
    myCurrentParams = setPotentialParams(*params.second);

    // The potential may have changed the params so reset them in the shared data
    common::setObject(
      common::GlobalKeys::POTENTIAL_PARAMS,
      params.first,
      myCurrentParams,
      getRunner()->memory()
    );

    // Add a note to the table with the current parameter string
    myTableSupport.getTable().addTableNote("params: " + myParamPotential.getParamString());
  }

  // Call the parent to let them deal with the starting event too
  PotentialGo::pipelineStarting();
}

void ParamPotentialGo::copyOptimisationResults(
  const sstbx::potential::PotentialData & optData,
  spipe::common::StructureData & strData)
{
  // Copy over information from the optimisation results
  PotentialGo::copyOptimisationResults(optData, *strData.getStructure());

  // Add the potential parameters to its data
  strData.objectsStore[common::GlobalKeys::POTENTIAL_PARAMS] = myCurrentParams;
}

arma::vec ParamPotentialGo::setPotentialParams(const ::arma::vec & params)
{
	using ::std::string;

	myParamPotential.setParams(params);

  const arma::vec actualParams = myParamPotential.getParams();

  return actualParams;
}


}}

