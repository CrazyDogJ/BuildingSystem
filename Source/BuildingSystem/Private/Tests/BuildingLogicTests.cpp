#if WITH_DEV_AUTOMATION_TESTS

#include "Tests/BuildingLogicTestTypes.h"
#include "Actors/BuildingActor.h"
#include "Actors/BuildingGraphData.h"
#include "BuildingManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/WorldSettings.h"
#include "Graph/BuildingGraphVertex.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildingLogicLifecycleTest, "BuildingSystem.Logic.Lifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBuildingLogicLifecycleTest::RunTest(const FString&)
{
	UGameInstance* Game = NewObject<UGameInstance>(GEngine);
	Game->InitializeStandalone();
	UWorld* World = Game->GetWorld();
	ON_SCOPE_EXIT
	{
		World->EndPlay(EEndPlayReason::Quit);
		Game->Shutdown();
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
	};
	auto* Subsystem = World->GetSubsystem<UBuildingManagerSubsystem>();
	if (!TestNotNull(TEXT("building subsystem"), Subsystem)) return false;
	TestFalse(TEXT("no gameplay tick before BeginPlay"), Subsystem->IsTickable());
	World->InitializeActorsForPlay(FURL());
	World->GetWorldSettings()->NotifyBeginPlay();
	TestTrue(TEXT("server subsystem ticks during play"), Subsystem->IsTickable());

	auto* Actor = World->SpawnActor<ABuildingActor>();
	Actor->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	Subsystem->AddVertex(Actor);
	const FGraphVertexHandle Handle = Actor->GetGraphVertexHandle();
	auto* Vertex = CastChecked<UBuildingGraphVertex>(Handle.GetVertex());
	auto* Logic = Cast<UBuildingLogicTestObject>(Actor->GetBuildingLogicObject());
	if (!TestNotNull(TEXT("new building creates logic"), Logic)) return false;
	TestTrue(TEXT("actor and vertex share one instance"), Logic == Vertex->GetBuildingLogicObject());
	TestTrue(TEXT("loaded outer is actor"), Logic->GetOuter() == Actor);
	TestTrue(TEXT("loaded logic resolves world"), Logic->GetWorld() == World);
	Actor->InitLogicObject();
	Actor->PostGameLoaded();
	TestTrue(TEXT("repeated initialization preserves identity"), Logic == Actor->GetBuildingLogicObject());
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("exactly one tick while loaded"), Logic->TickCount, 1);
	Logic->SavedValue = 73;

	// StreamingLevelSave destroys runtime actors; level streaming also uses RemovedFromWorld.
	Actor->RouteEndPlay(EEndPlayReason::RemovedFromWorld);
	TestNull(TEXT("unloaded actor releases its reference"), Actor->GetBuildingLogicObject());
	TestTrue(TEXT("unloaded outer is vertex"), Logic->GetOuter() == Vertex);
	TestTrue(TEXT("unloaded logic resolves world"), Logic->GetWorld() == World);
	Actor->Destroy(true);
	TWeakObjectPtr<UBuildingLogicObject> WeakLogic = Logic;
	CollectGarbage(RF_NoFlags);
	if (!TestTrue(TEXT("graph keeps unloaded logic alive through GC"), WeakLogic.IsValid())) return false;
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("tick continues without actor"), Logic->TickCount, 2);

	auto* Replacement = World->SpawnActor<ABuildingActor>();
	Replacement->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	// Save archives preserve the ID but not the weak graph pointer.
	Replacement->SetGraphVertexHandle(FGraphVertexHandle(Handle.GetUniqueIndex(), nullptr));
	TestTrue(TEXT("streamed replacement reuses identical object"), Replacement->GetBuildingLogicObject() == Logic);
	TestTrue(TEXT("outer transfers to replacement actor"), Logic->GetOuter() == Replacement);
	TestEqual(TEXT("state survives actor replacement"), Logic->SavedValue, 73);
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("one tick after reload"), Logic->TickCount, 3);

	// Overlapping registration must detach the old actor before its late EndPlay arrives.
	auto* NewOccupant = World->SpawnActor<ABuildingActor>();
	NewOccupant->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	NewOccupant->SetGraphVertexHandle(Handle);
	Replacement->Destroy(true);
	TestTrue(TEXT("late EndPlay preserves current owner"), Subsystem->GetBuildingActorByVertexHandle(Handle) == NewOccupant);
	TestTrue(TEXT("late EndPlay preserves current outer"), Logic->GetOuter() == NewOccupant);

	const auto Saved = Subsystem->GetGraphSaveGameData();
	Logic->SavedValue = 99;
	Subsystem->LoadGraphSaveGameData(Saved);
	TestTrue(TEXT("in-place save restore preserves object identity"), NewOccupant->GetBuildingLogicObject() == Logic);
	TestEqual(TEXT("save state applied to existing object"), Logic->SavedValue, 73);
	TestTrue(TEXT("actor rebound to new runtime graph"), NewOccupant->GetGraphVertexHandle().GetGraph() == Subsystem->GetBuildingGraph());
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("one tick after graph replacement"), Logic->TickCount, 4);

	NewOccupant->Destroy(true);
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("Destroy path also preserves tick"), Logic->TickCount, 5);
	Subsystem->LoadGraphSaveGameData(Saved);
	Vertex = CastChecked<UBuildingGraphVertex>(FGraphVertexHandle(Handle.GetUniqueIndex(), Subsystem->GetBuildingGraph()).GetVertex());
	TestTrue(TEXT("unloaded save restore also preserves identity"), Vertex->GetBuildingLogicObject() == Logic);
	TestTrue(TEXT("unloaded restored outer follows new vertex"), Logic->GetOuter() == Vertex);
	Subsystem->GetBuildingGraph()->RemoveVertex(Vertex->Handle());
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("removed building no longer ticks"), Logic->TickCount, 5);

	// A Blueprint TickLogic can remove another vertex while the subsystem is iterating.
	auto* FirstActor = World->SpawnActor<ABuildingActor>();
	FirstActor->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	Subsystem->AddVertex(FirstActor);
	auto* SecondActor = World->SpawnActor<ABuildingActor>();
	SecondActor->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	Subsystem->AddVertex(SecondActor);
	const auto Vertices = Subsystem->GetBuildingGraph()->GetVertices();
	auto It = Vertices.CreateConstIterator();
	auto* FirstLogic = CastChecked<UBuildingLogicTestObject>(CastChecked<UBuildingGraphVertex>(It.Value())->GetBuildingLogicObject());
	++It;
	const auto RemovedHandle = It.Key();
	TStrongObjectPtr<UBuildingLogicTestObject> RemovedLogic(CastChecked<UBuildingLogicTestObject>(CastChecked<UBuildingGraphVertex>(It.Value())->GetBuildingLogicObject()));
	FirstLogic->OnTick = [Subsystem, RemovedHandle] { Subsystem->GetBuildingGraph()->RemoveVertex(RemovedHandle); };
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("tick callback runs once"), FirstLogic->TickCount, 1);
	TestEqual(TEXT("vertex removed during another tick is skipped"), RemovedLogic->TickCount, 0);
	FirstLogic->OnTick = nullptr;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildingLogicLevelDataTest, "BuildingSystem.Logic.LevelData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBuildingLogicLevelDataTest::RunTest(const FString&)
{
	UGameInstance* Game = NewObject<UGameInstance>(GEngine);
	Game->InitializeStandalone();
	UWorld* World = Game->GetWorld();
	ON_SCOPE_EXIT
	{
		Game->Shutdown();
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
	};
	TStrongObjectPtr<UBuildingGraph> EditorGraph(NewObject<UBuildingGraph>());
	const auto EditorHandle = EditorGraph->CreateVertex();
	auto* EditorVertex = CastChecked<UBuildingGraphVertex>(EditorHandle.GetVertex());
	EditorVertex->PendingLogicClass = UBuildingLogicTestObject::StaticClass();
	TBuildingGraphSerialization Save;
	Save << *EditorGraph;
	auto* DataActor = World->SpawnActor<ABuildingGraphData>();
	DataActor->BuildingGraphData = Save.GetData();
#if WITH_EDITORONLY_DATA
	TestNull(TEXT("game world never creates GraphData editor cache"), DataActor->BuildingGraph);
#endif
	TBuildingGraphDeserialization ReadEditor(Save.GetData(), false);
	ReadEditor >> *EditorGraph;
	EditorVertex = CastChecked<UBuildingGraphVertex>(EditorHandle.GetVertex());
	TestNull(TEXT("editor cache never instantiates logic"), EditorVertex->GetBuildingLogicObject());
	TestTrue(TEXT("editor cache preserves class data"), !EditorVertex->PendingLogicClass.IsNull());

	// Actor registration can precede GraphData BeginPlay and start with an editor handle.
	auto* Actor = World->SpawnActor<ABuildingActor>();
	Actor->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	Actor->SetGraphVertexHandle(EditorHandle);
	auto* Subsystem = World->GetSubsystem<UBuildingManagerSubsystem>();
	auto* Logic = Actor->GetBuildingLogicObject();
	if (!TestNotNull(TEXT("level data loaded before actor initialization"), Logic)) return false;
	TestTrue(TEXT("editor handle rebound to server graph"), Actor->GetGraphVertexHandle().GetGraph() == Subsystem->GetBuildingGraph());
	TestNull(TEXT("editor vertex still has no runtime instance"), EditorVertex->GetBuildingLogicObject());
	Subsystem->LoadLevelBuildingGraphData(DataActor);
	TestTrue(TEXT("late GraphData BeginPlay cannot recreate logic"), Actor->GetBuildingLogicObject() == Logic);
	TestTrue(TEXT("actor remains bound to canonical vertex"), CastChecked<UBuildingGraphVertex>(Actor->GetGraphVertexHandle().GetVertex())->GetBuildingLogicObject() == Logic);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildingLogicStreamingTest, "BuildingSystem.Logic.StreamingIntegration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBuildingLogicStreamingTest::RunTest(const FString&)
{
	// Optional integration: exercise the real StreamingLevelSave delegates without
	// making the building plugin depend on that plugin for ordinary use.
	UClass* MarkerClass = FindObject<UClass>(nullptr, TEXT("/Script/StreamingLevelSave.StreamingLevelSaveComponent"));
	if (!MarkerClass)
	{
		AddInfo(TEXT("StreamingLevelSave is not enabled; optional integration skipped."));
		return true;
	}
	UGameInstance* Game = NewObject<UGameInstance>(GEngine);
	Game->InitializeStandalone();
	UWorld* World = Game->GetWorld();
	ON_SCOPE_EXIT
	{
		World->EndPlay(EEndPlayReason::Quit);
		Game->Shutdown();
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
	};
	World->InitializeActorsForPlay(FURL());
	World->GetWorldSettings()->NotifyBeginPlay();
	FCoreUObjectDelegates::PostLoadMapWithWorld.Broadcast(World);
	auto* Subsystem = World->GetSubsystem<UBuildingManagerSubsystem>();
	auto* Actor = World->SpawnActor<ABuildingActor>();
	Actor->BuildingLogicObjectClass = UBuildingLogicTestObject::StaticClass();
	Subsystem->AddVertex(Actor);
	auto* Marker = NewObject<UActorComponent>(Actor, MarkerClass);
	Actor->AddInstanceComponent(Marker);
	Marker->RegisterComponent();
	auto* Logic = CastChecked<UBuildingLogicTestObject>(Actor->GetBuildingLogicObject());
	Logic->SavedValue = 42;
	const auto Handle = Actor->GetGraphVertexHandle();
	FWorldDelegates::PreLevelRemovedFromWorld.Broadcast(World->PersistentLevel, World);
	TestTrue(TEXT("StreamingLevelSave unload destroys runtime building"), Actor->IsActorBeingDestroyed());
	TestNull(TEXT("streaming unload clears actor registration"), Subsystem->GetBuildingActorByVertexHandle(Handle));
	TestTrue(TEXT("streaming unload moves same object into graph"), Logic->GetOuter() == Handle.GetVertex());
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("logic ticks between cell unload and reload"), Logic->TickCount, 1);
	Logic->SavedValue = 84;
	FWorldDelegates::LevelAddedToWorld.Broadcast(World->PersistentLevel, World);
	ABuildingActor* Restored = nullptr;
	for (TActorIterator<ABuildingActor> It(World); It; ++It)
	{
		if (IsValid(*It) && !It->IsActorBeingDestroyed()) Restored = *It;
	}
	if (!TestNotNull(TEXT("StreamingLevelSave respawned building"), Restored)) return false;
	// The project's PostLoadSaveData blueprint calls this building restore hook.
	Restored->PostGameLoaded();
	TestTrue(TEXT("streaming archive preserves vertex identity"), Restored->GetGraphVertexHandle() == Handle);
	TestTrue(TEXT("streaming reload reuses exact instance"), Restored->GetBuildingLogicObject() == Logic);
	TestTrue(TEXT("streaming reload changes outer to new actor"), Logic->GetOuter() == Restored);
	TestEqual(TEXT("offline state is not overwritten by stale actor data"), Logic->SavedValue, 84);
	Subsystem->Tick(0.1f);
	TestEqual(TEXT("streaming reload resumes one tick per frame"), Logic->TickCount, 2);
	return true;
}

#endif
