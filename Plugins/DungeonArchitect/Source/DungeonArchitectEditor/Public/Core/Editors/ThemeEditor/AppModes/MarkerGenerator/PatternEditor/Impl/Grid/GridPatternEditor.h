//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditor.h"
#include "Core/Utils/MathUtils.h"

class UMarkerGenGridPatternRule;
enum class EMarkerGenGridPatternRuleType : uint8;
class UMarkerGenGridPattern;
class UMGGridPatternEditorSettings;
class AGridPatternEdCursorActor;
class AGridPatternEdRuleActor;
class UGridPatternEdGroundItem;
class AGridPatternEdGroundActor;


class FMGGridPatternEditor : public IMGPatternEditor, public FGCObject {
public:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	
	virtual void Tick(FPatternEditorViewportClient* InViewportClient, float DeltaSeconds) override;
	virtual bool InputKey(FPatternEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InControllerId, FKey InKey, EInputEvent InEvent, float InAmountDepressed, bool bInGamepad) override;
	virtual void CapturedMouseMove(FPatternEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual void ProcessClick(FPatternEditorViewportClient* InViewportClient, FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void CheckHoveredHitProxy(FPatternEditorViewportClient* InViewportClient, HHitProxy* HoveredHitProxy) override;
	virtual void InvalidateRuleVisuals(const UMarkerGenPatternRule* InRule) override;
	virtual void Load(UMarkerGenPattern* InPattern) override;
	virtual void FocusCameraOnScene() const override;
	virtual void RefreshRuleActor(UMarkerGenPatternRule* InRule) override;

	
protected:
	virtual void SetupScene() override;
	void HandleGroundActorClicked(FPatternEditorViewportClient* InViewportClient, AGridPatternEdGroundActor* InGroundActor, const UGridPatternEdGroundItem* InItemComponent, const FKey& InKey, EInputEvent InEvent);
	void HandleRuleActorClicked(FPatternEditorViewportClient* InViewportClient, AGridPatternEdRuleActor* InRuleActor, const FKey& InKey, EInputEvent InEvent);
	void SetSelectedRuleActor(AGridPatternEdRuleActor* InSelected);
	void ShowContextMenu() const;
	
	virtual void HandleAddNewRule() override;
	virtual bool CanAddNewRule() const override;
	virtual void HandleDeleteRule() override;
	virtual bool CanDeleteRule() const override;
	virtual void HandleCopyRule() override;
	virtual bool CanCopyRule() const override;
	virtual void HandlePasteRule() override;
	virtual bool CanPasteRule() const override;


	void DeleteRule(AGridPatternEdRuleActor* InRuleActorToDelete, bool bShowConfirmation);
	AGridPatternEdRuleActor* AddNewRule(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InRuleType, const UMarkerGenGridPatternRule* InTemplate = nullptr);

	AGridPatternEdRuleActor* CreateRuleActor(UWorld* InWorld, UMarkerGenGridPatternRule* InRule) const;
	
private:
	AGridPatternEdGroundActor* GroundPlaneActor = nullptr;
	AGridPatternEdCursorActor* CursorActor = nullptr;
	const UMGGridPatternEditorSettings* BuildSettings = nullptr;
	TWeakObjectPtr<AActor> LastHoveredActor;
	TWeakObjectPtr<const UPrimitiveComponent> LastHoveredComponent;
	TWeakObjectPtr<AGridPatternEdRuleActor> DraggedActor;
	TWeakObjectPtr<AGridPatternEdRuleActor> SelectedActor;
	TWeakObjectPtr<UMarkerGenGridPattern> PatternModel;
	TWeakObjectPtr<UMarkerGenGridPatternRule> CopiedRule;

	struct FDragStartState {
		FDragStartState();
		void Init(UMarkerGenGridPatternRule* InRule);
		bool HasChanged(UMarkerGenGridPatternRule* InRule) const;
		
		FIntPoint Coord;
		EMarkerGenGridPatternRuleType RuleType;
		bool bStateInitialized = false;
	};
	FDragStartState DragStartState;
	
	TDASmoothValue<FVector> CursorLocation;
};

