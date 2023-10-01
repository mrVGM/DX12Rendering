#include "Reflection.h"

#include "Type.h"
#include "Property.h"

#include "TypeManager.h"

#include "GeneratedObjectMeta.h"
#include "ScriptingStructMeta.h"

#include "utils.h"

namespace
{
	reflection::IntType m_intType;
	reflection::FloatType m_floatType;
	reflection::StringType m_stringType;
}

void reflection::Boot()
{
	CreateTestStruct();
}

void reflection::CreateTestStruct()
{
	BaseObjectMeta* structMeta = new GeneratedObjectMeta(scripting::ScriptingStructMeta::GetInstance());
	StructType* testStruct = new StructType(*structMeta, GetNewId());

	Property& p = testStruct->AddProperty();
	p.SetName("Test Prop");
	p.SetDataType(m_intType);
	p.SetAccessType(AccessType::Public);
	p.SetStructureType(StructureType::Single);

	TypeManager& typeManager = TypeManager::GetInstance();
	typeManager.RegisterGeneratedType(*structMeta, *testStruct);
}
