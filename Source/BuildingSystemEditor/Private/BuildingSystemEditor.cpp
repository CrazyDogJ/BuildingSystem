#include "BuildingSystemEditor.h"
#include "BuildingSystemSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FBuildingSystemEditorModule"

void FBuildingSystemEditorModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Building System", LOCTEXT("RuntimeSettingsName", "Building System"), LOCTEXT("RuntimeSettingsDescription", "Configure Building System"), GetMutableDefault<UBuildingSystemSettings>());
	}
}

void FBuildingSystemEditorModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Building System");
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBuildingSystemEditorModule, BuildingSystemEditor)