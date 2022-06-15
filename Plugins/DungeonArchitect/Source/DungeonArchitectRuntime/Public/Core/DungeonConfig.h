//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "DungeonConfig.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(DungeonConfigLog, Log, All);

DECLARE_DELEGATE_OneParam(ConfigPropertyChangedDelegate, struct FPropertyChangedEvent&);

UCLASS(Blueprintable)
class DUNGEONARCHITECTRUNTIME_API UDungeonConfig : public UObject {
    GENERATED_UCLASS_BODY()

public:
    /** 
      Changing this number would completely change the layout of the dungeon. 
      This is the base random number seed that is used to build the dungeon 
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    int32 Seed;

    /**
    Generate the dungeon with instanced meshes to improve performance
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    bool Instanced;

    /**
    The dungeon is built asynchronously over multiple frames if this value is > 0.
    If value is 0, the entire dungeon is built in a single frame (might face lag)
    Set it to a value in milli seconds (e.g. 16-33) to have it built over multiple frames,
    while maintaining a smooth frame rate
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    float MaxBuildTimePerFrameMs;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
    ConfigPropertyChangedDelegate ConfigPropertyChanged;
#endif

};

