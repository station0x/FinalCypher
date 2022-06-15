//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "GridPatternEditorSettings.generated.h"

USTRUCT()
struct FMGGridPatternGroundMeshItemColors
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FLinearColor BodyColor = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FLinearColor BorderColor = FLinearColor::Black;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FLinearColor HoverBodyColor = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FLinearColor HoverBorderColor = FLinearColor::Black;
};

USTRUCT()
struct FMGGridPatternEdGroundSettings {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FIntPoint Start = FIntPoint(-5, -5);
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FIntPoint End = FIntPoint(5, 5);
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	float TileSize = 400;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	float EdgeSize = 100;

	UPROPERTY(EditAnywhere, Category="Theme Editor")
	float BorderThickness = 5;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	float HoverBorderThickness = 10;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	float HoverAnimationTime = 0.1f;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	UCurveFloat* HoverAnimation = nullptr;

	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FMGGridPatternGroundMeshItemColors GroundColor;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FMGGridPatternGroundMeshItemColors EdgeColor;
	
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FMGGridPatternGroundMeshItemColors CornerColor;
};


UCLASS()
class UMGGridPatternEditorSettings : public UDataAsset {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Theme Editor")
	FMGGridPatternEdGroundSettings GroundSettings;
};

