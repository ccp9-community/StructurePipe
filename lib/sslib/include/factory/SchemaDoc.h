/*
 * SchemaDoc.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_DOC_H
#define SCHEMA_DOC_H

// INCLUDES /////////////////////////////////////////////
#include <map>

#include "factory/ISchemaElementInstance.h"
#include "factory/SchemaElement.h"

// DEFINES //////////////////////////////////////////////

namespace sstbx
{
namespace factory
{

// FORWARD DECLARATIONS ////////////////////////////////////
class SchemaObjectUid;

class SchemaDoc
{
public:

  typedef ::boost::shared_ptr<SchemaElement>          ElementPointer;
  typedef ::boost::shared_ptr<ISchemaElementInstance> ElementInstPointer;

  SchemaDoc();
  SchemaDoc(ElementInstPointer & root);

  void registerElement(const ElementPointer & obj);

protected:

  typedef ::std::map<SchemaObjectUid, ElementPointer> ElementsContainer;

  ElementInstPointer            myRoot;
  ElementsContainer             myElements;
};
}
}

#endif /* SCHEMA_DOC_H */

