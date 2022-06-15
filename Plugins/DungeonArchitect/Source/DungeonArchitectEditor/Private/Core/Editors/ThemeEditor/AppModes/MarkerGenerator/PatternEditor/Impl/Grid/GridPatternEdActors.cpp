//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEdActors.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEditorUtils.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/Font.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

UGridPatternEdGroundItem::UGridPatternEdGroundItem() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UGridPatternEdGroundItem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	{
		const float Diff = HoverEnergy - HoverEnergySmooth;
		const float TimeToComplete = FMath::Abs(Diff) * HoverAnimationTime;
		const float HoverAlphaDt = FMath::Clamp(DeltaTime / TimeToComplete, 0.0f, 1.0f);
		HoverEnergySmooth = FMath::Lerp(HoverEnergySmooth, HoverEnergy, HoverAlphaDt);
		
		// Set the hover energy to 0, It will be set to 1 again before this tick if the mouse is hovering over it
		HoverEnergy = 0;
	}

	const float ColorAlpha = HoverEnergySmooth;
	const float BorderAlpha = HoverAnimation ? HoverAnimation->GetFloatValue(HoverEnergySmooth) : HoverEnergySmooth;

	SetColor(FMath::Lerp(BodyColor, HoverBodyColor, ColorAlpha));
	SetBorderColor(FMath::Lerp(BorderColor, HoverBorderColor, ColorAlpha));
	SetBorderThickness(FMath::Lerp(BorderThickness, HoverBorderThickness, BorderAlpha));
}

void UGridPatternEdGroundItem::Initialize(const FVector& InLocation, const FVector2D& InScale)
{
	if (!IsRunningCommandlet()) {
		// Structure to hold one-time initialization
		struct FResources {
			TSoftObjectPtr<UStaticMesh> SM_GroundPlane;
			TSoftObjectPtr<UMaterialInterface> M_GroundMat;

			FResources()
				: SM_GroundPlane(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Meshes/SM_MGPlane.SM_MGPlane")))
			    , M_GroundMat(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/Ground/M_Ground_Inst.M_Ground_Inst")))
			{
			}
		};
		static FResources Resources;
		
		SetStaticMesh(Resources.SM_GroundPlane.LoadSynchronous());

		UMaterialInterface* MatBase = Resources.M_GroundMat.LoadSynchronous();
		if (MatBase) {
			MaterialInstance = UMaterialInstanceDynamic::Create(MatBase, this);
			SetMaterial(0, MaterialInstance);
		}
	}
	SetRelativeLocation(InLocation);
	SetRelativeScale3D(FVector(InScale, 1));
}

void UGridPatternEdGroundItem::SetBorderColor(const FLinearColor& InColor) const {
	if (MaterialInstance) {
		MaterialInstance->SetVectorParameterValue("BorderColor", InColor);
	}
}

void UGridPatternEdGroundItem::SetColor(const FLinearColor& InColor) const {
	if (MaterialInstance) {
		MaterialInstance->SetVectorParameterValue("Color", InColor);
	}
}

void UGridPatternEdGroundItem::SetBorderThickness(int32 InThickness) const {
	if (MaterialInstance) {
		MaterialInstance->SetScalarParameterValue("BorderThickness", InThickness);
	}
}

void UGridPatternEdGroundItem::OnHover() {
	HoverEnergy = 1.0f;
}

AGridPatternEdGroundActor::AGridPatternEdGroundActor() : Super()
{
	bIsEditorOnlyActor = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
}

void AGridPatternEdGroundActor::Build(const FMGGridPatternEdGroundSettings& InSettings) {
	Settings = InSettings;
	
	constexpr float MeshBaseSize = 100.0f;
	const float ItemTileScaleValue = InSettings.TileSize / MeshBaseSize;
	const FVector2D ItemTileScale = FVector2D(ItemTileScaleValue, ItemTileScaleValue);
	const FVector2D XEdgeScale = FVector2D(InSettings.TileSize, InSettings.EdgeSize) / MeshBaseSize;
	const FVector2D YEdgeScale = FVector2D(InSettings.EdgeSize, InSettings.TileSize) / MeshBaseSize;
	const FVector2D CornerScale = FVector2D(InSettings.EdgeSize, InSettings.EdgeSize) / MeshBaseSize;
	
	const float Offset = InSettings.TileSize + InSettings.EdgeSize;
	
	for (int X = InSettings.Start.X; X <= InSettings.End.X; X++) {
		for (int Y = InSettings.Start.Y; Y <= InSettings.End.Y; Y++) {
			const FVector TileLoc = FVector(X, Y, 0) * Offset;
			const FVector XEdgeLoc = FVector(X, Y - 0.5f, 0) * Offset;
			const FVector YEdgeLoc = FVector(X - 0.5f, Y, 0) * Offset;
			const FVector CornerLoc = FVector(X - 0.5f, Y - 0.5f, 0) * Offset;

			const FIntPoint Key(X, Y);
			
			UGridPatternEdGroundItem* TileItem = CreateItem(InSettings, InSettings.GroundColor, TileLoc, ItemTileScale);
			TileItem->Coord = Key;
			TileItem->ItemType = EMarkerGenGridPatternRuleType::Ground;
			Tiles.Add(Key, TileItem);

			UGridPatternEdGroundItem* XEdgeItem = CreateItem(InSettings, InSettings.EdgeColor, XEdgeLoc, XEdgeScale);
			XEdgeItem->Coord = Key;
			XEdgeItem->ItemType = EMarkerGenGridPatternRuleType::EdgeX;
			XEdges.Add(Key, XEdgeItem);

			UGridPatternEdGroundItem* YEdgeItem = CreateItem(InSettings, InSettings.EdgeColor, YEdgeLoc, YEdgeScale);
			YEdgeItem->Coord = Key;
			YEdgeItem->ItemType = EMarkerGenGridPatternRuleType::EdgeY;
			YEdges.Add(Key, YEdgeItem);

			UGridPatternEdGroundItem* CornerItem = CreateItem(InSettings, InSettings.CornerColor, CornerLoc, CornerScale);
			CornerItem->Coord = Key;
			CornerItem->ItemType = EMarkerGenGridPatternRuleType::Corner;
			Corners.Add(Key, CornerItem);
		}
	}
}

void AGridPatternEdGroundActor::OnMouseHover(const FVector& InWorldIntersection) {
	UGridPatternEdGroundItem* Item;
	EMarkerGenGridPatternRuleType ItemType;
	if (Deproject(InWorldIntersection, Item, ItemType)) {
		Item->OnHover();
	}
}

bool AGridPatternEdGroundActor::Deproject(const FVector& InWorldIntersection, UGridPatternEdGroundItem*& OutItem, EMarkerGenGridPatternRuleType& OutItemType) {
	FIntPoint Coord;
	FGridPatternEditorUtils::DeprojectGround(InWorldIntersection, Settings.TileSize, Settings.EdgeSize, Coord, OutItemType);

	TMap<FIntPoint, UGridPatternEdGroundItem*>* ItemMap = nullptr;
	if (OutItemType == EMarkerGenGridPatternRuleType::Corner) {
		ItemMap = &Corners;
	}
	else if (OutItemType == EMarkerGenGridPatternRuleType::EdgeY) {
		ItemMap = &YEdges;
	}
	else if (OutItemType == EMarkerGenGridPatternRuleType::EdgeX) {
		ItemMap = &XEdges;
	}
	else {	// Ground
		ItemMap = &Tiles;
	}

	if (UGridPatternEdGroundItem** ItemPtr = ItemMap->Find(Coord)) {
		UGridPatternEdGroundItem* Item = *ItemPtr;
		OutItem = Item;
		return true;
	}
	else {
		OutItem = nullptr;
		return false;
	}
}

UGridPatternEdGroundItem* AGridPatternEdGroundActor::CreateItem(const FMGGridPatternEdGroundSettings& InSettings, const FMGGridPatternGroundMeshItemColors& InColors,
                                                                const FVector& InLocation, const FVector2D& InScale)
{
	UGridPatternEdGroundItem* TileItem = NewObject<UGridPatternEdGroundItem>(this);
	TileItem->Initialize(InLocation, InScale);
	
	TileItem->BorderThickness = InSettings.BorderThickness;
	TileItem->HoverBorderThickness = InSettings.HoverBorderThickness;
	TileItem->HoverAnimationTime = InSettings.HoverAnimationTime;
	TileItem->HoverAnimation = InSettings.HoverAnimation;

	TileItem->BodyColor = InColors.BodyColor;
	TileItem->BorderColor = InColors.BorderColor;
	TileItem->HoverBodyColor = InColors.HoverBodyColor;
	TileItem->HoverBorderColor = InColors.HoverBorderColor;
	
	TileItem->SetupAttachment(RootComponent);
	TileItem->RegisterComponent();
	return TileItem;
}

////////////////////////////// AGridPatternEdRuleActor //////////////////////////////
AGridPatternEdCursorActor::AGridPatternEdCursorActor() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	CursorMesh = CreateDefaultSubobject<UStaticMeshComponent>("CursorMesh");

	if (!IsRunningCommandlet()) {
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneQuad;
			ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> M_Cursor;

			FConstructorStatics()
				: PlaneQuad(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Meshes/SM_MGPlane"))
				, M_Cursor(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/Cursor/M_Cursor_Inst"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		UStaticMesh* PlaneQuad = ConstructorStatics.PlaneQuad.Get();
		CursorMesh->SetStaticMesh(PlaneQuad);
		CursorMesh->SetMaterial(0, ConstructorStatics.M_Cursor.Get());
	}
}

void AGridPatternEdCursorActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	CursorMesh->SetRelativeLocation(CursorLocation);
}

void AGridPatternEdCursorActor::SetCursorLocation(const FVector& InLocation) {
	CursorLocation = InLocation;
}

////////////////////////////// AGridPatternEdRuleActor //////////////////////////////
AGridPatternEdRuleActor::AGridPatternEdRuleActor() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComponent->SetupAttachment(SceneRoot);
}

void AGridPatternEdRuleActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (DesiredLocation.IsTransitioning()) {
		DesiredLocation.Tick(DeltaSeconds);
		SetActorLocation(DesiredLocation());
	}
	if (DesiredScale.IsTransitioning()) {
		DesiredScale.Tick(DeltaSeconds);
		SetActorScale3D(DesiredScale());
	}

}

void AGridPatternEdRuleActor::Destroyed() {
	Super::Destroyed();

	if (CanvasTexture) {
		CanvasTexture->ReleaseResource();
		CanvasTexture = nullptr;
	}
}

void AGridPatternEdRuleActor::Initialize(const FMGGridPatternEdGroundSettings& InSettings, UMarkerGenGridPatternRule* InRule) {
	Rule = InRule;
	Settings = InSettings;

	struct FSceneResources {
		TSoftObjectPtr<UStaticMesh> SM_Cube;
		TSoftObjectPtr<UMaterialInterface> M_Flat;
		TSoftObjectPtr<UFont> Font;
		TSoftObjectPtr<UFont> FontTitle;

		FSceneResources()
			: SM_Cube(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Meshes/SM_Cube.SM_Cube")))
			, M_Flat(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/Objects/M_Flat_Inst.M_Flat_Inst")))
			, Font(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Fonts/RuleText_Font.RuleText_Font")))
			, FontTitle(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Fonts/RuleTitle_Font.RuleTitle_Font")))
		{
		}
	};
	static FSceneResources Resources;

	Font = Resources.Font.LoadSynchronous();
	FontTitle = Resources.FontTitle.LoadSynchronous();
	
	MeshComponent->SetStaticMesh(Resources.SM_Cube.LoadSynchronous());
	if (UMaterialInterface* MatBase = Resources.M_Flat.LoadSynchronous()) {
		MaterialInstance = UMaterialInstanceDynamic::Create(MatBase, this);
		MeshComponent->SetMaterial(0, MaterialInstance);
	}
	UpdateTransform(false);
	UpdateColor();
	
	if (CanvasTexture) {
		CanvasTexture->ReleaseResource();
		CanvasTexture = nullptr;
	}
	constexpr int32 CanvasTexSize = 256;
	CanvasTexture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(), UCanvasRenderTarget2D::StaticClass(), CanvasTexSize, CanvasTexSize);
	CanvasTexture->RenderTargetFormat = RTF_R8;
	CanvasTexture->UpdateResource();
	
	CanvasTexture->OnCanvasRenderTargetUpdate.AddDynamic(this, &AGridPatternEdRuleActor::RenderCanvas);
	if (MaterialInstance) {
		MaterialInstance->SetTextureParameterValue("CanvasTex", CanvasTexture);
	}
	
	RefreshCanvas();
}

void AGridPatternEdRuleActor::UpdateState(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InItemType, bool bAnimate) {
	if (Rule) {
		Rule->Coord = InCoord;
		Rule->RuleType = InItemType;
	}

	UpdateTransform(bAnimate);
}

void AGridPatternEdRuleActor::UpdateTransform(bool bAnimate) {
	if (!Rule) return;
	
	FVector Location, Scale;
	FGridPatternEditorUtils::GetItemLocationScale(Rule->Coord, Settings.TileSize, Settings.EdgeSize, Rule->RuleType, Rule->bVisuallyDominant, Location, Scale);
	Scale -= FVector(0.4f, 0.4f, 0.0f);
	DesiredLocation.SetTarget(Location, bAnimate);
	DesiredScale.SetTarget(Scale, bAnimate);
	if (!bAnimate) {
		SetActorLocation(DesiredLocation());
		SetActorScale3D(DesiredScale());
	}

	auto FnShowCanvasSides = [this](float X, float Y, float Z) {
		MaterialInstance->SetScalarParameterValue("ShowCanvasX", X);
		MaterialInstance->SetScalarParameterValue("ShowCanvasY", Y);
		MaterialInstance->SetScalarParameterValue("ShowCanvasZ", Z);
	};
	
	if (MaterialInstance) {
		if (Rule->RuleType == EMarkerGenGridPatternRuleType::Ground) {
			FnShowCanvasSides(0, 0, 1);
		}
		else if (Rule->RuleType == EMarkerGenGridPatternRuleType::EdgeX) {
			if (Rule->bVisuallyDominant) {
				FnShowCanvasSides(0, 1, 0);
			}
			else {
				FnShowCanvasSides(0, 0, 0);
			}
		}
		else if (Rule->RuleType == EMarkerGenGridPatternRuleType::EdgeY) {
			if (Rule->bVisuallyDominant) {
				FnShowCanvasSides(1, 0, 0);
			}
			else {
				FnShowCanvasSides(0, 0, 0);
			}
		}
		else if (Rule->RuleType == EMarkerGenGridPatternRuleType::Corner) {
			FnShowCanvasSides(0, 0, 0);
		}
	}
}

void AGridPatternEdRuleActor::UpdateColor() const {
	if (Rule) {
		SetColor(Rule->Color, Rule->bVisuallyDominant);
	}
}

void AGridPatternEdRuleActor::SetSelected(bool bInSelected) const {
	if (MaterialInstance) {
		MaterialInstance->SetScalarParameterValue("Selected", bInSelected ? 1 : 0);
	}
}

namespace MGGridLib {
	struct FLineInfo {
		FString Text;
		int32 Height = 0;
		int32 Width = 0;
		TWeakObjectPtr<UFont> Font;
	};

	FLineInfo Create(const FString& InText, UFont* InFont) {
		check(InFont);
		FLineInfo Info;
		Info.Text = InText;
		Info.Font = InFont;
		InFont->GetStringHeightAndWidth(InText, Info.Height, Info.Width);
		return Info;
	}
	
	TArray<FLineInfo> WordWrap(const FString& InText, UFont* InFont, int32 InMaxWidth) {
		TArray<FString> WrappedLines;
		FString CurrentText;
		FString L, R;
		FString Remaining = InText;
		int32 Height, Width;
		while (Remaining.Split(" ", &L, &R)) {
			FString NextText = CurrentText + " " + L;
			InFont->GetStringHeightAndWidth(NextText, Height, Width);
			if (Width > InMaxWidth && CurrentText.Len() > 0) {
				WrappedLines.Add(CurrentText);
				CurrentText = "";
			}
			if (CurrentText.Len() > 0) {
				CurrentText += " ";
			}
			CurrentText += L;
			Remaining = R;
		}

		InFont->GetStringHeightAndWidth(CurrentText + " " + Remaining, Height, Width);
		if (Width > InMaxWidth && CurrentText.Len() > 0) {
			WrappedLines.Add(CurrentText);
			WrappedLines.Add(Remaining);
		}
		else {
			WrappedLines.Add(CurrentText + " " + Remaining);
		}

		TArray<FLineInfo> Result;
		for (const FString& WrappedLine : WrappedLines) {
			Result.Add(Create(WrappedLine, InFont));
		}
		return Result;
	}
	
	FString GenerateConditionText(const UMGPatternScriptNode_Result* InResultNode) {
		return InResultNode ? InResultNode->GenerateRuleText() : "";
	}
	
	TArray<FString> GenerateActionTextList(UMGPatternScriptNode_OnPass* InPassNode) {
		TArray<FString> Result;
		UMGPatternActionScriptNodeBase* NextNode = FMGPatternScriptUtils::GetOutgoingExecNode(InPassNode);
		TSet<UMGPatternActionScriptNodeBase*> Visited;
		while (NextNode && !Visited.Contains(NextNode)) {
			Visited.Add(NextNode);
			const FString ActionText = NextNode->GetActionText();
			if (ActionText.Len() > 0) {
				Result.Add(ActionText);
			}
			NextNode = FMGPatternScriptUtils::GetOutgoingExecNode(NextNode);
		}
		return Result;
	}
	
	// Special Comment: tasmiya
}


void AGridPatternEdRuleActor::RefreshCanvas() {
	// Update the Rule and Action List texts
	if (Rule->RuleScript) {
		ConditionText = MGGridLib::GenerateConditionText(Rule->RuleScript->ResultNode.Get());
		ActionTextList = MGGridLib::GenerateActionTextList(Rule->RuleScript->PassEventNode.Get());
	}

	if (CanvasTexture) {
		CanvasTexture->UpdateResource();
	}
}

void AGridPatternEdRuleActor::RenderCanvas(UCanvas* Canvas, int32 Width, int32 Height) {
	const FString Title = "Rule Info";
	
	TArray<MGGridLib::FLineInfo> Lines;
	if (ConditionText.Len() > 0) {
		Lines.Add(MGGridLib::Create("Condition:", FontTitle));
		Lines.Append(MGGridLib::WordWrap(ConditionText, Font, Width));
	}
	if (ActionTextList.Num() > 0) {
		Lines.Add(MGGridLib::Create("Actions:", FontTitle));
		for (const FString& Action : ActionTextList) {
			Lines.Add(MGGridLib::Create(Action, Font));
		} 
	}
	
	FIntPoint BaseLocation;
	{
		int32 TotalWidth = 0;
		int32 TotalHeight = 0;
		for (const MGGridLib::FLineInfo& Line : Lines) {
			TotalWidth = FMath::Max(TotalWidth, Line.Width);
			TotalHeight += Line.Height;
		}
		TotalWidth = FMath::Min(Width, TotalWidth);
		TotalHeight = FMath::Min(Height, TotalHeight);
		BaseLocation = (FIntPoint(Width, Height) - FIntPoint(TotalWidth, TotalHeight)) / 2;
	}
    const int32 StartX = BaseLocation.X;
    int32 StartY = BaseLocation.Y;

    for (const MGGridLib::FLineInfo& Info : Lines) {
    	if (!Info.Font.IsValid()) continue;
    	
        const FVector2D Location(StartX, StartY);
        FCanvasTextItem TextItem( Location, FText::FromString( Info.Text ), Info.Font.Get(), FLinearColor::White );
        Canvas->DrawItem( TextItem );
        StartY += Info.Height;
    }
}

void AGridPatternEdRuleActor::SetColor(const FLinearColor& InColor, bool bVisuallyDominant) const {
	FLinearColor TargetColor = InColor;
	if (!bVisuallyDominant) {
		FLinearColor HSV = InColor.LinearRGBToHSV();
		HSV.G = 0.35f;
		TargetColor = HSV.HSVToLinearRGB();
	}

	FLinearColor CanvasColor = InColor;
	{
		FLinearColor HSV = CanvasColor.LinearRGBToHSV();
		HSV.R = FMath::Frac(HSV.R + 0.5f);
		HSV.B = 1 - HSV.B;
		CanvasColor = HSV.HSVToLinearRGB();
	}

	if (MaterialInstance) {
		MaterialInstance->SetVectorParameterValue("Color", TargetColor);
		MaterialInstance->SetVectorParameterValue("CanvasColor", CanvasColor);
	}
}

