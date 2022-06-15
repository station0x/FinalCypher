//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"

#include "Frameworks/MarkerGenerator/MarkerGenModel.h"

UDungeonThemeAsset::UDungeonThemeAsset(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    PreviewViewportProperties = ObjectInitializer.CreateDefaultSubobject<UDungeonEditorViewportProperties>( this, "PreviewProperties");
    MarkerGenerationModel = ObjectInitializer.CreateDefaultSubobject<UMarkerGenModel>(this, "MarkerGenerationModel");
}

void UDungeonThemeAsset::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector) {
    UDungeonThemeAsset* This = CastChecked<UDungeonThemeAsset>(InThis);

#if WITH_EDITORONLY_DATA
    Collector.AddReferencedObject(This->UpdateGraph, This);
#endif	// WITH_EDITORONLY_DATA

    Super::AddReferencedObjects(This, Collector);
}

