//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/Visualization/ThemeEditorMarkerVisualizer.h"

#include "Builders/FloorPlan/FloorPlanBuilder.h"
#include "Builders/FloorPlan/FloorPlanConfig.h"
#include "Builders/Grid/GridDungeonBuilder.h"
#include "Builders/Grid/GridDungeonConfig.h"
#include "Builders/GridFlow/GridFlowBuilder.h"
#include "Builders/GridFlow/GridFlowConfig.h"
#include "Builders/GridFlow/GridFlowModel.h"
#include "Builders/SimpleCity/SimpleCityBuilder.h"
#include "Builders/SimpleCity/SimpleCityConfig.h"
#include "Core/DungeonBuilder.h"
#include "Core/DungeonMarkerNames.h"
#include "Core/DungeonProp.h"

class FDAMarkerVisMeshBuilder {
public:
	FDAMarkerVisMeshBuilder(const FVector& InGridSize) : GridSize(InGridSize) {}
	virtual ~FDAMarkerVisMeshBuilder() {}
	virtual void Build(const FTransform& InTransform, TArray<FDAProcMeshVertex>& OutVertices, TArray<int32>& OutIndices) = 0;
	virtual UMaterialInterface* GetMaterial() { return nullptr; }

protected:
	static void BuildQuad(const FTransform& InTransform, TArray<FDAProcMeshVertex>& OutVertices, TArray<int32>& OutIndices) {
		int32 BaseIndex = OutVertices.Num();

		const FVector Normal(0, 0, 1);
		const int32 I0 = AddVertex(InTransform, FVector(-1, 1, 0), FVector2D(0, 1), Normal, OutVertices);
		const int32 I1 = AddVertex(InTransform, FVector(1, 1, 0), FVector2D(1, 1), Normal, OutVertices);
		const int32 I2 = AddVertex(InTransform, FVector(1, -1, 0), FVector2D(1, 0), Normal, OutVertices);
		const int32 I3 = AddVertex(InTransform, FVector(-1, -1, 0), FVector2D(0, 0), Normal, OutVertices);

		OutIndices.Add(I0);
		OutIndices.Add(I1);
		OutIndices.Add(I2);
		
		OutIndices.Add(I0);
		OutIndices.Add(I2);
		OutIndices.Add(I3);
	}
	
	static int32 AddVertex(const FTransform& InTransform, const FVector& InLocation, const FVector2D& UV0, const FVector& InNormal, TArray<FDAProcMeshVertex>& OutVertices) {
		const int32 Index = OutVertices.Num();
		FDAProcMeshVertex& Vertex = OutVertices.AddDefaulted_GetRef();	
		Vertex.Position = InTransform.TransformPosition(InLocation);
		Vertex.Normal = InTransform.TransformVector(InNormal);
		Vertex.UV0 = UV0;
		Vertex.Color = FColor::White;
		return Index;
	}

protected:
	FVector GridSize;
};

typedef TSharedPtr<FDAMarkerVisMeshBuilder> FDAMarkerVisMeshBuilderPtr;

class FDAMarkerVisMeshBuilder_Ground : public FDAMarkerVisMeshBuilder {
public:
	explicit FDAMarkerVisMeshBuilder_Ground(const FVector& InGridSize)
		: FDAMarkerVisMeshBuilder(InGridSize) {}

	virtual void Build(const FTransform& InTransform, TArray<FDAProcMeshVertex>& OutVertices, TArray<int32>& OutIndices) override {
		const FTransform GroundTransform = InTransform.GetScaled(GridSize * 0.5f);
		BuildQuad(GroundTransform, OutVertices, OutIndices);

		const FTransform BackFaceTransform = FTransform(FQuat(FVector(1, 0, 0), PI)) * GroundTransform;
		BuildQuad(BackFaceTransform, OutVertices, OutIndices);
	}
	
	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
				TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Ground_Inst.M_MarkerVis_Ground_Inst")));
	}
};

class FDAMarkerVisMeshBuilder_Wall : public FDAMarkerVisMeshBuilder {
public:
	explicit FDAMarkerVisMeshBuilder_Wall(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder(InGridSize) {}

	virtual void Build(const FTransform& InTransform, TArray<FDAProcMeshVertex>& OutVertices, TArray<int32>& OutIndices) override {
		const FTransform WallTransform =
			FTransform(FQuat(FVector(1, 0, 0), -PI * 0.5)) *
			InTransform.GetScaled(GridSize * 0.5f) *
			FTransform(FVector(0, 0, GridSize.X * 0.5f));
		BuildQuad(WallTransform, OutVertices, OutIndices);

		const FTransform BackFaceTransform =
            FTransform(FQuat(FVector(1, 0, 0), -PI * 0.5)) *
            FTransform(FQuat(FVector(0, 0, 1), PI)) *
            InTransform.GetScaled(GridSize * 0.5f) *
            FTransform(FVector(0, 0, GridSize.X * 0.5f));
		BuildQuad(BackFaceTransform, OutVertices, OutIndices);
	}
	
	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Wall_Inst.M_MarkerVis_Wall_Inst")));
	}
};


class FDAMarkerVisMeshBuilder_WallTile : public FDAMarkerVisMeshBuilder_Ground {
public:
	explicit FDAMarkerVisMeshBuilder_WallTile(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder_Ground(InGridSize) {}

	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Wall_Inst.M_MarkerVis_Wall_Inst")));
	}
};

class FDAMarkerVisMeshBuilder_TileEmpty : public FDAMarkerVisMeshBuilder_Ground {
public:
	explicit FDAMarkerVisMeshBuilder_TileEmpty(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder_Ground(InGridSize) {}

	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_TileEmpty_Inst.M_MarkerVis_TileEmpty_Inst")));
	}
};

class FDAMarkerVisMeshBuilder_DoorTile : public FDAMarkerVisMeshBuilder_Ground {
public:
	explicit FDAMarkerVisMeshBuilder_DoorTile(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder_Ground(InGridSize) {}

	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Door_Inst.M_MarkerVis_Door_Inst")));
	}
};

class FDAMarkerVisMeshBuilder_Door : public FDAMarkerVisMeshBuilder_Wall {
public:
	explicit FDAMarkerVisMeshBuilder_Door(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder_Wall(InGridSize) {}

	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Door_Inst.M_MarkerVis_Door_Inst")));
	}
};

class FDAMarkerVisMeshBuilder_Stairs : public FDAMarkerVisMeshBuilder {
public:
	explicit FDAMarkerVisMeshBuilder_Stairs(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder(InGridSize) {}

	virtual void Build(const FTransform& InTransform, TArray<FDAProcMeshVertex>& OutVertices, TArray<int32>& OutIndices) override {
		const FTransform GroundTransform =
			FTransform(FQuat(FVector(0, 0, 1), -PI * 0.5f)) *
			InTransform.GetScaled(GridSize * 0.5f);
		const int32 BaseIndex = OutVertices.Num();
		BuildQuad(GroundTransform, OutVertices, OutIndices);
		OutVertices[BaseIndex + 2].Position += FVector(0, 0, GridSize.Z);
		OutVertices[BaseIndex + 3].Position += FVector(0, 0, GridSize.Z);

		//const FTransform BackFaceTransform = FTransform(FQuat(FVector(1, 0, 0), PI)) * GroundTransform;
		//BuildQuad(BackFaceTransform, OutVertices, OutIndices);
	}
	
	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Stair_Inst.M_MarkerVis_Stair_Inst")));
	}
};

class FDAMarkerVisMeshBuilder_Fence : public FDAMarkerVisMeshBuilder {
public:
	explicit FDAMarkerVisMeshBuilder_Fence(const FVector& InGridSize)
        : FDAMarkerVisMeshBuilder(InGridSize) {}

	virtual void Build(const FTransform& InTransform, TArray<FDAProcMeshVertex>& OutVertices, TArray<int32>& OutIndices) override {
		const FTransform WallTransform =
            FTransform(FQuat(FVector(1, 0, 0), -PI * 0.5)) *
            InTransform.GetScaled(FVector(GridSize.X * 0.5f, GridSize.Y * 0.25f, 1.0f)) *
            FTransform(FVector(0, 0, GridSize.X * 0.25f));
		BuildQuad(WallTransform, OutVertices, OutIndices);

		const FTransform BackFaceTransform =
            FTransform(FQuat(FVector(1, 0, 0), -PI * 0.5)) *
            FTransform(FQuat(FVector(0, 0, 1), PI)) *
            InTransform.GetScaled(FVector(GridSize.X * 0.5f, GridSize.Y * 0.25f, 1.0f)) *
            FTransform(FVector(0, 0, GridSize.X * 0.25f));
		BuildQuad(BackFaceTransform, OutVertices, OutIndices);
	}
	
	virtual UMaterialInterface* GetMaterial() override {
		return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr,
                TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/MarkerVis/M_MarkerVis_Fence_Inst.M_MarkerVis_Fence_Inst")));
	}
};


class FDAMarkerVisMeshBuilderFactory {
public:
#define DA_ADD_BUILDER(MarkerName, MeshBuilderClass)											\
		if (MarkersToVisualize.Contains(*MarkerName)) {											\
			OutMeshBuilders.Add(*MarkerName, MakeShareable(new MeshBuilderClass(GridSize)));	\
		}
	
	static void CreateBuilders(UDungeonConfig* InDungeonConfig, UDungeonModel* InDungeonModel, UDungeonBuilder* InDungeonBuilder, const TSet<FName>& MarkersToVisualize, TMap<FName, FDAMarkerVisMeshBuilderPtr>& OutMeshBuilders) {
		if (!InDungeonBuilder) return;
		if (InDungeonBuilder->IsA<UGridDungeonBuilder>()) {
			if (UGridDungeonConfig* GridConfig = Cast<UGridDungeonConfig>(InDungeonConfig)) {
				const FVector GridSize = GridConfig->GridCellSize;
				DA_ADD_BUILDER(FGridBuilderMarkers::ST_GROUND, FDAMarkerVisMeshBuilder_Ground)
                DA_ADD_BUILDER(FGridBuilderMarkers::ST_WALL, FDAMarkerVisMeshBuilder_Wall)
                DA_ADD_BUILDER(FGridBuilderMarkers::ST_FENCE, FDAMarkerVisMeshBuilder_Fence)
                DA_ADD_BUILDER(FGridBuilderMarkers::ST_DOOR, FDAMarkerVisMeshBuilder_Door)
                DA_ADD_BUILDER(FGridBuilderMarkers::ST_STAIR, FDAMarkerVisMeshBuilder_Stairs)
            }
		}
		else if (InDungeonBuilder->IsA<UFloorPlanBuilder>()) {
			if (UFloorPlanConfig* FloorConfig = Cast<UFloorPlanConfig>(InDungeonConfig)) {
				const FVector GridSize = FloorConfig->GridSize;
				DA_ADD_BUILDER(FFloorPlanBuilderMarkers::MARKER_GROUND, FDAMarkerVisMeshBuilder_Ground)
				DA_ADD_BUILDER(FFloorPlanBuilderMarkers::MARKER_WALL, FDAMarkerVisMeshBuilder_Wall)
				DA_ADD_BUILDER(FFloorPlanBuilderMarkers::MARKER_DOOR, FDAMarkerVisMeshBuilder_Door)
				DA_ADD_BUILDER(FFloorPlanBuilderMarkers::MARKER_BUILDING_WALL, FDAMarkerVisMeshBuilder_Wall)
            }
		}
		else if (InDungeonBuilder->IsA<USimpleCityBuilder>()) {
			if (USimpleCityConfig* CityConfig = Cast<USimpleCityConfig>(InDungeonConfig)) {
				const FVector GridSize = FVector(CityConfig->CellSize.X, CityConfig->CellSize.Y, 1);
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerHouse, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerPark, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerRoad, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerRoadX, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerRoadT, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerRoadS, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerRoadCorner, FDAMarkerVisMeshBuilder_TileEmpty)
				DA_ADD_BUILDER(FSimpleCityBuilderMarkers::MarkerRoadEnd, FDAMarkerVisMeshBuilder_TileEmpty)
            }
		}
		else if (InDungeonBuilder->IsA<UGridFlowBuilder>()) {
			UGridFlowConfig* Config = Cast<UGridFlowConfig>(InDungeonConfig);
			UGridFlowModel* Model = Cast<UGridFlowModel>(InDungeonModel);
			if (Config && Model) {
				const FVector GridSize = Config->GridSize;
				const bool bWallsAsEdges = Model->bWallsAsEdges;
				
				DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_GROUND, FDAMarkerVisMeshBuilder_Ground)
				DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_CAVE_FENCE, FDAMarkerVisMeshBuilder_Fence)
                DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_FENCE, FDAMarkerVisMeshBuilder_Fence)

				if (bWallsAsEdges) {
					DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_WALL, FDAMarkerVisMeshBuilder_Wall)
					DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_DOOR, FDAMarkerVisMeshBuilder_Door)
                    DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_DOOR_ONEWAY, FDAMarkerVisMeshBuilder_Door)
                }
				else {
					DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_WALL, FDAMarkerVisMeshBuilder_WallTile)
                    DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_WALL_L, FDAMarkerVisMeshBuilder_WallTile)
                    DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_WALL_T, FDAMarkerVisMeshBuilder_WallTile)
                    DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_WALL_X, FDAMarkerVisMeshBuilder_WallTile)
                    DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_DOOR, FDAMarkerVisMeshBuilder_DoorTile)
                    DA_ADD_BUILDER(FGridFlowBuilderMarkers::MARKER_DOOR_ONEWAY, FDAMarkerVisMeshBuilder_DoorTile)
				}
            }
		}
	}

#undef DA_ADD_BUILDER
};


AThemeEditorMarkerVisualizer::AThemeEditorMarkerVisualizer() {
	MeshComponent = CreateDefaultSubobject<UDAProcMeshComponent>("MeshComponent");
	RootComponent = MeshComponent;
}

void AThemeEditorMarkerVisualizer::BuildVisualization(UDungeonConfig* InConfig, UDungeonModel* InModel, UDungeonBuilder* InBuilder, const TSet<FName>& MarkersToVisualize) const {
	ClearVisualization();
	
	if (!InBuilder) {
		return;
	}
	
	TMap<FName, FDAMarkerVisMeshBuilderPtr> MeshBuilders;
	FDAMarkerVisMeshBuilderFactory::CreateBuilders(InConfig, InModel, InBuilder, MarkersToVisualize, MeshBuilders);

	TArray<FDAProcMeshVertex> Vertices;
	TArray<int32> Indices;
	int32 SectionIndex = 0;
	for (const FName& MarkerToVisualize : MarkersToVisualize) {
		FDAMarkerVisMeshBuilderPtr* MeshBuilderPtr = MeshBuilders.Find(MarkerToVisualize);
		if (!MeshBuilderPtr) continue;

		FDAMarkerVisMeshBuilderPtr MeshBuilder = *MeshBuilderPtr;
		if (!MeshBuilder.IsValid()) continue;

		for (const FDAMarkerInfo& Marker : InBuilder->GetMarkers()) {
			FName MarkerName = *Marker.MarkerName;
			if (MarkerName == MarkerToVisualize) {
				MeshBuilder->Build(Marker.Transform, Vertices, Indices);
			}
		}
		if (Vertices.Num() > 0) {
			MeshComponent->CreateMeshSection(SectionIndex, Vertices, Indices, PT_TriangleList, false);

			UMaterialInterface* Material = MeshBuilder->GetMaterial();
			if (Material) {
				MeshComponent->SetMaterial(SectionIndex, Material);
			}
			
			SectionIndex++;
		}
	}
}

void AThemeEditorMarkerVisualizer::ClearVisualization() const {
	MeshComponent->ClearAllMeshSections();
}

