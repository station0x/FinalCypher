//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Actors/DungeonMesh.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/Graph/EdGraphNode_DungeonActorBase.h"

#include "PhysicsEngine/BodyInstance.h"
#include "EdGraphNode_DungeonMesh.generated.h"

UCLASS()
class DUNGEONARCHITECTEDITOR_API UEdGraphNode_DungeonMesh : public UEdGraphNode_DungeonActorBase {
    GENERATED_UCLASS_BODY()

public:
    virtual UObject* GetNodeAssetObject(UObject* Outer) override;
    virtual UObject* GetThumbnailAssetObject() override;

    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    TArray<FMaterialOverride> MaterialOverrides;

    /** Physics scene information for this component, holds a single rigid body with multiple shapes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Collision)
    bool bUseCustomCollision = false;
    
    /** Physics scene information for this component, holds a single rigid body with multiple shapes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Collision, meta=(ShowOnlyInnerProperties, SkipUCSModifiedProperties, EditCondition="bUseCustomCollision"))
    FBodyInstance BodyInstance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Details")
    UStaticMeshComponent* AdvancedOptions;
};

