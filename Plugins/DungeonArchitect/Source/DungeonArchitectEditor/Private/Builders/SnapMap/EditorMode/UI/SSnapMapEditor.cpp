//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Builders/SnapMap/EditorMode/UI/SSnapMapEditor.h"

#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "SnapMapEditMode"

void SSnapMapEditor::Construct(const FArguments& InArgs) {
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    
    DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    this->ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            DetailsPanel.ToSharedRef()
        ]
    ];
}

void SSnapMapEditor::SetSettingsObject(UObject* Object, bool bForceRefresh /*= false*/) {
    if (DetailsPanel.IsValid()) {
        DetailsPanel->SetObject(Object, bForceRefresh);
    }
}

#undef LOCTEXT_NAMESPACE

