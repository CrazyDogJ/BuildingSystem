#pragma once

#include "BuildingLogicObject.h"
#include "BuildingLogicTestTypes.generated.h"

// Native fixture keeps lifecycle tests independent of project blueprint behavior.
UCLASS(Transient, NotBlueprintable)
class UBuildingLogicTestObject : public UBuildingLogicObject
{
	GENERATED_BODY()
public:
	UPROPERTY(SaveGame)
	int32 SavedValue = 0;

	int32 TickCount = 0;
	TFunction<void()> OnTick;

	virtual void NativeTickLogic(float DeltaTime) override
	{
		++TickCount;
		Super::NativeTickLogic(DeltaTime);
		if (OnTick) OnTick();
	}
};
