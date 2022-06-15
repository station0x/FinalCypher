//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Meshing/Geometry/DungeonProceduralMesh.h"

#include "GameFramework/Actor.h"
#include "ThemeEditorMarkerVisualizer.generated.h"

class UDungeonConfig;
class UDungeonBuilder;
class UDungeonModel;

UCLASS()
class DUNGEONARCHITECTEDITOR_API AThemeEditorMarkerVisualizer : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AThemeEditorMarkerVisualizer();

	void BuildVisualization(UDungeonConfig* InConfig, UDungeonModel* InModel, UDungeonBuilder* InBuilder, const TSet<FName>& MarkersToVisualize) const;
	void ClearVisualization() const;
	
private:
	UPROPERTY()
	UDAProcMeshComponent* MeshComponent = nullptr;
};

