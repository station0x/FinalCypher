//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/LevelEditor/EditorMode/DungeonEdModeToolkit.h"

#include "Core/LevelEditor/EditorMode/DungeonEdMode.h"

#include "Classes/EditorStyleSettings.h"
#include "Editor.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "DungeonEditMode"

namespace DAEdModeImpl
{
    static const FName DAPaletteName(TEXT("Dungeon Architect")); 
    const TArray<FName> DAPaletteNames = { DAPaletteName };
}


void FDungeonEdModeToolkit::Init(const TSharedPtr<class IToolkitHost>& InitToolkitHost) {
    FModeToolkit::Init(InitToolkitHost);
}

FName FDungeonEdModeToolkit::GetToolkitFName() const {
    return FName("DungeonEditMode");
}

FText FDungeonEdModeToolkit::GetBaseToolkitName() const {
    return LOCTEXT("ToolkitName", "Dungeon Edit Mode");
}

class FEdMode* FDungeonEdModeToolkit::GetEditorMode() const {
    return GLevelEditorModeTools().GetActiveMode(FEdModeDungeon::EM_Dungeon);
}

TSharedPtr<SWidget> FDungeonEdModeToolkit::GetInlineContent() const {
    return DungeonEdWidget;
}

void FDungeonEdModeToolkit::GetToolPaletteNames(TArray<FName>& InPaletteName) const {
    InPaletteName = DAEdModeImpl::DAPaletteNames;
}

FText FDungeonEdModeToolkit::GetToolPaletteDisplayName(FName PaletteName) const {
    if (PaletteName == DAEdModeImpl::DAPaletteName)
    {
        return LOCTEXT("DAPaletteTitle", "Dungeon Architect");
    }
    return FText();
}

void FDungeonEdModeToolkit::BuildToolPalette(FName PaletteName, FToolBarBuilder& ToolbarBuilder) {
    if (PaletteName == DAEdModeImpl::DAPaletteName && DungeonEdWidget.IsValid())
    {
        //DungeonEdWidget->CustomizeToolBarPalette(ToolbarBuilder);
    }
}

FText FDungeonEdModeToolkit::GetActiveToolDisplayName() const {
    return LOCTEXT("ActiveToolDebugTitle", "Tool Name");
    //return DungeonEdWidget->GetActiveToolName();
}

FText FDungeonEdModeToolkit::GetActiveToolMessage() const {
    //return DungeonEdWidget->GetActiveToolMessage();
    return LOCTEXT("ActiveToolDebugTitle", "Tool Message");
}


void FDungeonEdModeToolkit::SetInlineContent(TSharedPtr<SWidget> Widget) {
    DungeonEdWidget = Widget;
}

#undef LOCTEXT_NAMESPACE

