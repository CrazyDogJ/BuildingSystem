#include "BuildingSystemEditor.h"

#include "BuildingDefinition.h"
#include "BuildingDefThumbnailRenderer.h"
#include "BuildingSystemSettings.h"
#include "ISettingsModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define LOCTEXT_NAMESPACE "FBuildingSystemEditorModule"

void FBuildingSystemEditorModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Building System", LOCTEXT("RuntimeSettingsName", "Building System"), LOCTEXT("RuntimeSettingsDescription", "Configure Building System"), GetMutableDefault<UBuildingSystemSettings>());
	}

	if (GIsEditor)
	{
		UThumbnailManager::Get().RegisterCustomRenderer(UBuildingDefinition::StaticClass(),
			UBuildingDefThumbnailRenderer::StaticClass());
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