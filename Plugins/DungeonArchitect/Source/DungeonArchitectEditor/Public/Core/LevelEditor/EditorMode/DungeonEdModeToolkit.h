//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class SWidget;
class FEdMode;

class FDungeonEdModeToolkit : public FModeToolkit {
public:
    /** Initializes the dungeon mode toolkit */
    virtual void Init(const TSharedPtr<class IToolkitHost>& InitToolkitHost) override;

    /** IToolkit interface */
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FEdMode* GetEditorMode() const override;
    virtual TSharedPtr<SWidget> GetInlineContent() const override;

    
    /** Mode Toolbar Palettes **/
    virtual void GetToolPaletteNames(TArray<FName>& InPaletteName) const override;
    virtual FText GetToolPaletteDisplayName(FName PaletteName) const override; 
    virtual void BuildToolPalette(FName PaletteName, class FToolBarBuilder& ToolbarBuilder) override;

    /** Modes Panel Header Information **/
    virtual FText GetActiveToolDisplayName() const override;
    virtual FText GetActiveToolMessage() const override;

    
    void SetInlineContent(TSharedPtr<SWidget> Widget);

private:
    TSharedPtr<SWidget> DungeonEdWidget;
};

