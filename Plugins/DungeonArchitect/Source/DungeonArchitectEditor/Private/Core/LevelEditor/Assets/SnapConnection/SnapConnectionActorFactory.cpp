//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/LevelEditor/Assets/SnapConnection/SnapConnectionActorFactory.h"

#include "Frameworks/Snap/Lib/Connection/SnapConnectionActor.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionComponent.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionInfo.h"

#include "AssetRegistry/AssetData.h"

USnapConnectionActorFactory::USnapConnectionActorFactory(const FObjectInitializer& ObjectInitializer) : Super(
    ObjectInitializer) {
    DisplayName = NSLOCTEXT("SnapConnection", "SnapConnectionFactoryDisplayName", "Add Snap Connection");
    NewActorClass = ASnapConnectionActor::StaticClass();
}

UObject* USnapConnectionActorFactory::GetAssetFromActorInstance(AActor* ActorInstance) {
    ASnapConnectionActor* ConnectionActor = Cast<ASnapConnectionActor>(ActorInstance);
    return ConnectionActor ? ConnectionActor->ConnectionComponent->ConnectionInfo : nullptr;
}

AActor* USnapConnectionActorFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams) {
    AActor* Actor = UActorFactory::SpawnActor(InAsset, InLevel, InTransform, InSpawnParams);
    ASnapConnectionActor* ConnectionActor = Cast<ASnapConnectionActor>(Actor);
    if (ConnectionActor) {
        ConnectionActor->ConnectionComponent->ConnectionInfo = Cast<USnapConnectionInfo>(InAsset);
        //ConnectionActor->BuildConnection(InLevel->GetWorld());
    }
    return Actor;
}

void USnapConnectionActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor) {
    ASnapConnectionActor* ConnectionActor = Cast<ASnapConnectionActor>(NewActor);
    if (ConnectionActor && ConnectionActor->ConnectionComponent) {
        ConnectionActor->ConnectionComponent->ConnectionInfo = Cast<USnapConnectionInfo>(Asset);
    }
}

void USnapConnectionActorFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO) {
    ASnapConnectionActor* ConnectionActor = Cast<ASnapConnectionActor>(CDO);
    if (ConnectionActor && ConnectionActor->ConnectionComponent) {
        ConnectionActor->ConnectionComponent->ConnectionInfo = Cast<USnapConnectionInfo>(Asset);
    }
}

bool USnapConnectionActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) {
    if (AssetData.IsValid() && AssetData.GetClass()->IsChildOf(USnapConnectionInfo::StaticClass())) {
        return true;
    }
    OutErrorMsg = NSLOCTEXT("SnapConnection", "SnapConnectionFactoryDisplayName", "No connection info was specified.");
    return false;
}

