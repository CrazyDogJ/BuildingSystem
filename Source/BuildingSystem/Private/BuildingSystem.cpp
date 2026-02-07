// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildingSystem.h"

#include "BuildingSystemSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FBuildingSystemModule"

DEFINE_LOG_CATEGORY(LogBuildingSystem);

void FBuildingSystemModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Building System", LOCTEXT("RuntimeSettingsName", "Building System"), LOCTEXT("RuntimeSettingsDescription", "Configure Building System"), GetMutableDefault<UBuildingSystemSettings>());
	}
}

void FBuildingSystemModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Building System");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBuildingSystemModule, BuildingSystem)