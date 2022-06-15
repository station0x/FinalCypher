//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Utils/UserData.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphConstraints.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/BaseFlowLayoutTask.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/Lib/FlowAbstractGraphPathUtils.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleDatabase.h"
#include "SnapFlowAbstractGraphSupport.generated.h"

enum class ESnapFlowAGTaskModuleCategoryOverrideMethod : uint8;
class USnapFlowNodeCategorySelectionOverride;
struct FFlowAGPathNodeGroup;
class USnapGridFlowModuleDatabase;


//////////////////////////////////////// Node Category Selector //////////////////////////////////////////////
class DUNGEONARCHITECTRUNTIME_API FSGFNodeCategorySelector {
public:
    FSGFNodeCategorySelector(const TArray<FName>& InModuleCategories, ESnapFlowAGTaskModuleCategoryOverrideMethod InModuleCategoryOverrideMethod,
        const TArray<FName>& InStartNodeCategoryOverride, const TArray<FName>& InEndNodeCategoryOverride,
        const TArray<USnapFlowNodeCategorySelectionOverride*>& InCategoryOverrideLogic)
        : ModuleCategories(InModuleCategories)
        , ModuleCategoryOverrideMethod(InModuleCategoryOverrideMethod)
        , StartNodeCategoryOverride(InStartNodeCategoryOverride)
        , EndNodeCategoryOverride(InEndNodeCategoryOverride)
        , CategoryOverrideLogic(InCategoryOverrideLogic)
    {
    }

    TArray<FName> GetCategoriesAtNode(int32 PathIndex, int32 PathLength) const;

private:
    TArray<FName> ModuleCategories;
    ESnapFlowAGTaskModuleCategoryOverrideMethod ModuleCategoryOverrideMethod;
    TArray<FName> StartNodeCategoryOverride;
    TArray<FName> EndNodeCategoryOverride;
    TArray<USnapFlowNodeCategorySelectionOverride*> CategoryOverrideLogic;
};


//////////////////////////////////////// Snap Abstract Graph Node Group Generator //////////////////////////////////////////////

struct FSGFNodeGroupGenLib {
    static void BuildNodeGroup(const FFlowAbstractGraphQuery& InGraphQuery,
                       const UFlowAbstractNode* InNode, const TArray<const UFlowAbstractNode*>& InIncomingNodes,
                       FFlowAGPathNodeGroup& OutGroup, TArray<FFAGConstraintsLink>& OutConstraintLinks);
};

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FSnapFlowAGNodeGroupSetting {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Node Group")  
    float Weight = 1.0f;
    
    UPROPERTY(EditAnywhere, Category = "Node Group")  
    FIntVector GroupSize = FIntVector(1, 1, 1);

    UPROPERTY(EditAnywhere, Category = "Node Group")
    FSGFModuleAssembly ModuleAssembly;
    
    UPROPERTY(EditAnywhere, Category = "Node Group")  
    FName Category;

    UPROPERTY(EditAnywhere, Category = "Node Group")
    TArray<FIntVector> LocalSurfaceCoords;
    
    UPROPERTY(EditAnywhere, Category = "Node Group")
    TArray<FIntVector> LocalVolumeCoords;
};

class DUNGEONARCHITECTRUNTIME_API FSnapFlowAGNodeGroupGenerator : public IFlowAGNodeGroupGenerator {
public:
    explicit FSnapFlowAGNodeGroupGenerator(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase, TSharedPtr<FSGFNodeCategorySelector> InNodeCategorySelector);
    virtual void Generate(const FFlowAbstractGraphQuery& InGraphQuery, const UFlowAbstractNode* InCurrentNode,
                int32 InPathIndex, int32 InPathLength, const FRandomStream& InRandom, const TSet<FGuid>& InVisited,
                TArray<FFlowAGPathNodeGroup>& OutGroups) const override;
    
    virtual int32 GetMinNodeGroupSize() const override;

private:
    TArray<FSnapFlowAGNodeGroupSetting> GroupSettings;
    TWeakObjectPtr<const USnapGridFlowModuleDatabase> ModuleDB;
    TSharedPtr<FSGFNodeCategorySelector> NodeCategorySelector;
};

class DUNGEONARCHITECTRUNTIME_API FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator : public IFlowAGNodeGroupGenerator {
public:
    explicit FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase, TSharedPtr<FSGFNodeCategorySelector> InNodeCategorySelector);
    virtual void Generate(const FFlowAbstractGraphQuery& InGraphQuery, const UFlowAbstractNode* InCurrentNode,
                int32 InPathIndex, int32 InPathLength, const FRandomStream& InRandom, const TSet<FGuid>& InVisited,
                TArray<FFlowAGPathNodeGroup>& OutGroups) const override;
    
    virtual int32 GetMinNodeGroupSize() const override;

private:
    TArray<FSnapFlowAGNodeGroupSetting> GroupSettings;
    TWeakObjectPtr<const USnapGridFlowModuleDatabase> ModuleDB;
    TSharedPtr<FSGFNodeCategorySelector> NodeCategorySelector;
};


struct FSGFNodeGroupUserData : IDAUserData {
    FSGFModuleAssembly ModuleAssembly;

    void CopyFrom(const class USGFNodeGroupUserData* Other);
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API USGFNodeGroupUserData : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY()
    FSGFModuleAssembly ModuleAssembly;

    void CopyFrom(const FSGFNodeGroupUserData& Other);
};


//////////////////////////////////////// Snap Abstract Graph Constraint System //////////////////////////////////////////////

class DUNGEONARCHITECTRUNTIME_API FSnapGridFlowAbstractGraphConstraints : public FFlowAbstractGraphConstraints {
public:
    FSnapGridFlowAbstractGraphConstraints(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase,
            TSharedPtr<FSGFNodeCategorySelector> InNodeCategorySelector, bool bInSupportsDoorCategories);
    
    virtual bool IsValid(const FFlowAbstractGraphQuery& InGraphQuery, const UFlowAbstractNode* Node, const TArray<const UFlowAbstractNode*>& IncomingNodes) override;
    virtual bool IsValid(const FFlowAbstractGraphQuery& InGraphQuery, const FFlowAGPathNodeGroup& Group, int32 PathIndex, int32 PathLength, const TArray<FFAGConstraintsLink>& IncomingNodes) override;

private:
    bool IsValid(const FFlowAbstractGraphQuery& InGraphQuery, const FFlowAGPathNodeGroup& Group, const TArray<FFAGConstraintsLink>& IncomingNodes, const TArray<FName>& InAllowedCategories) const;

private:
    TWeakObjectPtr<USnapGridFlowModuleDatabase> ModuleDatabase;
    TSharedPtr<FSGFNodeCategorySelector> NodeCategorySelector;
    bool bSupportsDoorCategories;
};


//////////////////////////////////////// Snap Abstract Graph Domain Data //////////////////////////////////////////////

/**
    Tilemap domain specific data that is attached to the abstract graph nodes
*/
UCLASS()
class DUNGEONARCHITECTRUNTIME_API UFANodeSnapDomainData : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY()
    TArray<FName> ModuleCategories;
};

//////////////////////////////////////// Snap Abstract Graph Node Selection /////////////////////////////////////////////

UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType, Blueprintable, HideDropdown)
class USnapFlowNodeCategorySelectionOverride : public UObject {
    GENERATED_BODY()
public:
    // Change the category list if needed. Return true if the new list should be used, false to ignore this override blueprint 
    UFUNCTION(BlueprintNativeEvent, Category = "Dungeon")
    bool TryOverrideCategories(int32 PathIndex, int32 PathLength, const TArray<FName>& ExistingCategories, TArray<FName>& OutNewCategories);
    virtual bool TryOverrideCategories_Implementation(int32 PathIndex, int32 PathLength, const TArray<FName>& ExistingCategories, TArray<FName>& OutNewCategories) { return false; }
};

UENUM()
enum class ESnapFlowAGTaskModuleCategoryOverrideMethod : uint8 {
    None        UMETA(DisplayName = "None"),
    StartEnd   UMETA(DisplayName = "Start / End Nodes"),
    Blueprint   UMETA(DisplayName = "Blueprint"),
};



//////////////////////////////////////// DEPRECATED: USnapFlowAGTaskExtender /////////////////////////////////////////////
UCLASS()
class DUNGEONARCHITECTRUNTIME_API USnapFlowAGTaskExtender : public UFlowExecTaskExtender {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category="Snap")
    TArray<FName> ModuleCategories = { "Room" };

    UPROPERTY(EditAnywhere, Category="Snap")
    ESnapFlowAGTaskModuleCategoryOverrideMethod ModuleCategoryOverrideMethod = ESnapFlowAGTaskModuleCategoryOverrideMethod::None;
    
    UPROPERTY(EditAnywhere, Category="Snap", Meta=(EditCondition="ModuleCategoryOverrideMethod == ESnapFlowAGTaskModuleCategoryOverrideMethod::StartEnd"))
    TArray<FName> StartNodeCategoryOverride;
    
    UPROPERTY(EditAnywhere, Category="Snap", Meta=(EditCondition="ModuleCategoryOverrideMethod == ESnapFlowAGTaskModuleCategoryOverrideMethod::StartEnd"))
    TArray<FName> EndNodeCategoryOverride;
    
    /**
     * Use a blueprint logic to change the category list on a per-node basis, if needed.
     * Create a blueprint inherited from "Snap Flow Node Category Selection Override". Then override the "TryOverrideCategories" function
     */
    UPROPERTY(EditAnywhere, Instanced, SimpleDisplay, Category="Snap", Meta=(EditCondition="ModuleCategoryOverrideMethod == ESnapFlowAGTaskModuleCategoryOverrideMethod::Blueprint"))
    TArray<USnapFlowNodeCategorySelectionOverride*> CategoryOverrideLogic;
};

