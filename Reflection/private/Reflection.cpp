#include "Reflection.h"

#include "Type.h"
#include "Property.h"

#include "TypeManager.h"

#include "GeneratedObjectMeta.h"
#include "ScriptingStructMeta.h"

#include "Job.h"

#include "ReflectionSettings.h"

#include "utils.h"

namespace
{
	reflection::IntType m_intType;
	reflection::FloatType m_floatType;
	reflection::StringType m_stringType;
}

void reflection::Boot()
{
	class BootJob : public jobs::Job
	{
		void Do()
		{
			ReflectionSettings* settings = new ReflectionSettings();

			//CreateTestStruct();
			//settings->StoreSettings();

			TypeManager& typeManager = TypeManager::GetInstance();
			typeManager.LoadGeneratedTypes();
			bool t = true;
		}
	};

	RunMain(new BootJob());

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
	testStruct->StoreGeneratedType();

	TypeManager& typeManager = TypeManager::GetInstance();
	typeManager.RegisterGeneratedType(*structMeta, *testStruct);
}
