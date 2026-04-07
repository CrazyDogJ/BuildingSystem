#include "BuildingSystemEditor.h"

#include "BuildingDefinition.h"
#include "BuildingDefThumbnailRenderer.h"
#include "ISettingsModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define LOCTEXT_NAMESPACE "FBuildingSystemEditorModule"

void FBuildingSystemEditorModule::StartupModule()
{
	if (GIsEditor)
	{
		UThumbnailManager::Get().RegisterCustomRenderer(UBuildingDefinition::StaticClass(),
			UBuildingDefThumbnailRenderer::StaticClass());
	}
}

void FBuildingSystemEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBuildingSystemEditorModule, BuildingSystemEditor)