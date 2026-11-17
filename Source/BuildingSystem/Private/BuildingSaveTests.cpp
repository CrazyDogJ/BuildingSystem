#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Graph/BuildingGraph.h"
#include "Graph/BuildingGraphVertex.h"
#include "BuildingLogicObject.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildingSaveGraphTest, "StreamingLevelSave.BuildingGraph", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBuildingSaveGraphTest::RunTest(const FString&)
{
	UClass* Derived = LoadClass<UBuildingLogicObject>(nullptr, TEXT("/BuildingSystem/BP_TestLogicObject.BP_TestLogicObject_C"));
	if (!TestNotNull(TEXT("derived building logic fixture"), Derived)) return false;
	auto* Graph = NewObject<UBuildingGraph>();
	const auto Handle = Graph->CreateVertex();
	auto* Vertex = CastChecked<UBuildingGraphVertex>(Handle.GetVertex());
	Vertex->SetBuildingLogicObject(NewObject<UBuildingLogicObject>(Vertex, Derived));
	TBuildingGraphSerialization Save;
	Save << *Graph;
	auto Data = Save.GetData();
	auto* Restored = NewObject<UBuildingGraph>();
	TBuildingGraphDeserialization Read(Data);
	Read >> *Restored;
	const auto RestoredHandle = FGraphVertexHandle(Handle.GetUniqueIndex(), Restored);
	auto* Result = Cast<UBuildingGraphVertex>(RestoredHandle.GetVertex());
	if (!TestNotNull(TEXT("restored vertex"), Result) || !TestNotNull(TEXT("restored logic"), Result->GetBuildingLogicObject())) return false;
	TestTrue(TEXT("recreated correct blueprint subclass"), Result->GetBuildingLogicObject()->GetClass() == Derived);
	Data.BuildingVertices.FindChecked(Handle).LogicClass.Reset();
	auto* Legacy = NewObject<UBuildingGraph>();
	TBuildingGraphDeserialization ReadLegacy(Data);
	ReadLegacy >> *Legacy;
	auto* LegacyVertex = CastChecked<UBuildingGraphVertex>(FGraphVertexHandle(Handle.GetUniqueIndex(), Legacy).GetVertex());
	TestTrue(TEXT("legacy payload retained until owner supplies its class"), LegacyVertex->PendingLogicData == Data.BuildingVertices.FindChecked(Handle).LogicObjectData);
	return true;
}
#endif
