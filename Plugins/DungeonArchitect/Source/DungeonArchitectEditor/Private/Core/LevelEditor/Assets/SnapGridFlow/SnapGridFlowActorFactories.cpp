//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/LevelEditor/Assets/SnapGridFlow/SnapGridFlowActorFactories.h"

#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleBounds.h"

#include "AssetRegistry/AssetData.h"

#define LOCTEXT_NAMESPACE "SnapGridFlowModuleBoundsActorFactory"

USnapGridFlowModuleBoundsActorFactory::USnapGridFlowModuleBoundsActorFactory(const FObjectInitializer& ObjectInitializer) : Super(
    ObjectInitializer) {
    DisplayName = LOCTEXT("DisplayName", "Add Snap Grid Module Bounds");
    NewActorClass = ASnapGridFlowModuleBoundsActor::StaticClass();
}

UObject* USnapGridFlowModuleBoundsActorFactory::GetAssetFromActorInstance(AActor* ActorInstance) {
    ASnapGridFlowModuleBoundsActor* ModuleBoundsActor = Cast<ASnapGridFlowModuleBoundsActor>(ActorInstance);
    return ModuleBoundsActor ? ModuleBoundsActor->BoundsComponent->ModuleBounds.LoadSynchronous() : nullptr;
}

AActor* USnapGridFlowModuleBoundsActorFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams) {
    AActor* Actor = UActorFactory::SpawnActor(InAsset, InLevel, InTransform, InSpawnParams);
    if (const ASnapGridFlowModuleBoundsActor* ModuleBoundsActor = Cast<ASnapGridFlowModuleBoundsActor>(Actor)) {
        ModuleBoundsActor->BoundsComponent->ModuleBounds = Cast<USnapGridFlowModuleBoundsAsset>(InAsset);
    }
    return Actor;
}

void USnapGridFlowModuleBoundsActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor) {
    ASnapGridFlowModuleBoundsActor* ModuleBoundsActor = Cast<ASnapGridFlowModuleBoundsActor>(NewActor);
    if (ModuleBoundsActor) {
        ModuleBoundsActor->BoundsComponent->ModuleBounds = Cast<USnapGridFlowModuleBoundsAsset>(Asset);
    }
}

void USnapGridFlowModuleBoundsActorFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO) {
    ASnapGridFlowModuleBoundsActor* ModuleBoundsActor = Cast<ASnapGridFlowModuleBoundsActor>(CDO);
    if (ModuleBoundsActor) {
        ModuleBoundsActor->BoundsComponent->ModuleBounds = Cast<USnapGridFlowModuleBoundsAsset>(Asset);
    }
}

bool USnapGridFlowModuleBoundsActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) {
    if (AssetData.IsValid() && AssetData.GetClass()->IsChildOf(USnapGridFlowModuleBoundsAsset::StaticClass())) {
        return true;
    }
    OutErrorMsg = LOCTEXT("CreationErrorMessage", "Invalid Module Bounds Asset");
    return false;
}



#undef LOCTEXT_NAMESPACE

