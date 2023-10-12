#include "Reflection.h"

#include "Type.h"
#include "Property.h"

#include "TypeManager.h"

#include "GeneratedObjectMeta.h"
#include "ScriptingStructMeta.h"

#include "Job.h"

#include "ReflectionSettings.h"

#include "ReflectionEntryPoint.h"

#include "utils.h"

namespace
{
	reflection::BoolType m_boolType;
	reflection::IntType m_intType;
	reflection::FloatType m_floatType;
	reflection::StringType m_stringType;
}

void reflection::Boot()
{
	ReflectionSettings* settings = new ReflectionSettings();
	TypeManager& typeManager = TypeManager::GetInstance();

	typeManager.StoreTypeInformation();
}

void reflection::CreateTestStruct()
{
	BaseObjectMeta* structMeta = new GeneratedObjectMeta(scripting::ScriptingStructMeta::GetInstance());
	StructType* testStruct = new StructType(*structMeta, GetNewId());

	testStruct->SetName("My Struct");

	Property& p = testStruct->AddProperty();
	p.SetName("Test Prop");
	p.SetDataType(m_intType);
	p.SetAccessType(AccessType::Public);
	p.SetStructureType(StructureType::Single);

	ReflectionSettings* reflectionSettings = GetReflectionSettings();
	reflectionSettings->GetSettings().m_files[testStruct->GetID()] = reflectionSettings->GetSettings().m_dirPath + testStruct->GetID() + ".xml";
	testStruct->StoreTypeInfo();

	TypeManager& typeManager = TypeManager::GetInstance();
	typeManager.RegisterGeneratedType(*structMeta, *testStruct);
}

void reflection::RegisterLib()
{
	new ReflectionEntryPoint();
}
