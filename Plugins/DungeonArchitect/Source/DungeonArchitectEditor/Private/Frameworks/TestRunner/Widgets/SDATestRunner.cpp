//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/TestRunner/Widgets/SDATestRunner.h"

#include "Core/LevelEditor/Customizations/DungeonArchitectStyle.h"
#include "Frameworks/TestRunner/DATestRunnerCommands.h"

#include "EditorStyleSet.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styling/ISlateStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SWidget.h"

#define LOCTEXT_NAMESPACE "SDATestRunner"


namespace {
    const FName DetailsTabId("Details");
    const FName HistogramTabId("Histogram");
}

void SDATestRunner::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderTab,
                              const TSharedPtr<SWindow>& ConstructUnderWindow) {
    TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderTab);
    const TSharedRef<FWorkspaceItem> AppMenuGroup = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("GridFlowPerfMenuGroupName", "Performance Stats"));

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.bHideSelectionTip = true;
    
    PropertyEditor = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    PropertyEditor->SetObject(GetSettingsObject());

    BindCommands();

    TabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateRaw(this, &SDATestRunner::HandleTabManagerSpawnTab, DetailsTabId))
              .SetDisplayName(LOCTEXT("DetailsTabTitle", "Perf Settings"))
              .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
              .SetGroup(AppMenuGroup);

    TabManager->RegisterTabSpawner(HistogramTabId, FOnSpawnTab::CreateRaw(this, &SDATestRunner::HandleTabManagerSpawnTab, HistogramTabId))
              .SetDisplayName(LOCTEXT("HistogramTabTitle", "Histogram"))
              .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
              .SetGroup(AppMenuGroup);


    // create tab layout
    const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("GridFlowPerfLayout_v0.2")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)
            ->Split
            (
                // Details sub-tab
                FTabManager::NewStack()
                ->AddTab(DetailsTabId, ETabState::OpenedTab)
                ->SetHideTabWell(true)
                ->SetSizeCoefficient(0.25f)
            )
            ->Split
            (
                // Histogram tab
                FTabManager::NewStack()
                ->AddTab(HistogramTabId, ETabState::OpenedTab)
                ->SetForegroundTab(HistogramTabId)
                ->SetHideTabWell(true)
            )
        );


    // create & initialize main menu
    FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());

    MenuBarBuilder.AddPullDownMenu(
        LOCTEXT("WindowMenuLabel", "Window"),
        FText::GetEmpty(),
        FNewMenuDelegate::CreateStatic(&SDATestRunner::FillWindowMenu, TabManager),
        "Window"
    );

    ChildSlot
    [
        SNew(SVerticalBox)

        // Menu
        + SVerticalBox::Slot()
          .Padding(0)
          .AutoHeight()
        [
            MenuBarBuilder.MakeWidget()
        ]

        // Toolbar
        +SVerticalBox::Slot()
        .Padding(0)
        .AutoHeight()
        [
            SNew(SBorder)
            .Padding(0)
            .BorderImage(FEditorStyle::GetBrush("NoBorder"))
            .IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
            [
                CreateToolbarWidget()
            ]
        ]
        
        +SVerticalBox::Slot()
        .Padding(2)
        .FillHeight(1.0f)
        [
            TabManager->RestoreFrom(Layout, ConstructUnderWindow).ToSharedRef()
        ]

        // StatusBar
        + SVerticalBox::Slot()
          .Padding(2)
          .AutoHeight()
        [
            SNew(STextBlock)
            .Text(this, &SDATestRunner::GetStatusText)
        ]

    ];
}

void SDATestRunner::BindCommands() {
    CommandList = MakeShareable(new FUICommandList);
    CommandList->MapAction(FDATestRunnerCommands::Get().StartStopPerfRunner,
        FExecuteAction::CreateSP(this, &SDATestRunner::OnStartStopClicked),
        FCanExecuteAction::CreateSP(this, &SDATestRunner::IsStartStopButtonEnabled));
}

void SDATestRunner::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    if (bIsRunning) {
        TickService();
        if (!IsServiceRunning()) {
            NotifyTestsComplete();
        }
    }
}

TSharedRef<SDockTab> SDATestRunner::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier) const {
    TSharedRef<SDockTab> DockTab = SNew(SDockTab)
        .TabRole(PanelTab);

    TSharedRef<SWidget> TabWidget = SNullWidget::NullWidget;
    if (TabIdentifier == DetailsTabId) {
        TabWidget = PropertyEditor.ToSharedRef();
    }
    else if (TabIdentifier == HistogramTabId) {
        TabWidget = GetHistogramWidget();
    }

    DockTab->SetContent(TabWidget);
    return DockTab;
}

void SDATestRunner::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager) {
    if (!TabManager.IsValid()) {
        return;
    }

    TabManager->PopulateLocalTabSpawnerMenu(MenuBuilder);
}

TSharedRef<SWidget> SDATestRunner::CreateToolbarWidget() const {
    FToolBarBuilder ToolbarBuilder(CommandList.ToSharedRef(), FMultiBoxCustomization::None);
    
    ToolbarBuilder.BeginSection("Perf");
    
    ToolbarBuilder.AddToolBarButton(
        FDATestRunnerCommands::Get().StartStopPerfRunner,
        NAME_None,
        TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SDATestRunner::GetStartStopButtonLabel)),
        TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SDATestRunner::GetStartStopButtonTooltip)),
        TAttribute<FSlateIcon>::Create(TAttribute<FSlateIcon>::FGetter::CreateSP(this, &SDATestRunner::GetStartStopButtonIcon)));
    
    ToolbarBuilder.EndSection();
    
    return ToolbarBuilder.MakeWidget();
}

void SDATestRunner::OnStartStopClicked() {
    if (bIsRunning) {
        HandleStopService();
    }
    else {
        HandleStartService();
    }
}

void SDATestRunner::HandleStartService() {
    if (bIsRunning) {
        StopService();
    }

    // Check if the configuration is valid
    FText ErrorMessage;
    if (!ValidateConfiguration(ErrorMessage)) {
        FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
        return;
    }
    
    StartService();
    bIsRunning = true;
}

void SDATestRunner::HandleStopService() {
    if (bIsRunning) {
        StopService();
    }
    bIsRunning = false;
}

FText SDATestRunner::GetStartStopButtonLabel() const {
    return bIsRunning
               ? LOCTEXT("PerfRunner.Buttons.Label.Stop", "Stop")
               : LOCTEXT("PerfRunner.Buttons.Label.Start", "Start");
}

FText SDATestRunner::GetStartStopButtonTooltip() const {
    return bIsRunning
            ? LOCTEXT("PerfRunner.Buttons.Tooltip.Stop", "Stop the perf runner")
            : LOCTEXT("PerfRunner.Buttons.Tooltip.Start", "Start the perf runner");
}

FSlateIcon SDATestRunner::GetStartStopButtonIcon() const {
    const bool bShowStopIcon = bIsRunning;
    return bShowStopIcon
               ? FSlateIcon(FDungeonArchitectStyle::GetStyleSetName(), "DungeonArchitect.Icons.Stop")
               : FSlateIcon(FDungeonArchitectStyle::GetStyleSetName(), "DungeonArchitect.Icons.Play");
}

void SDATestRunner::NotifyTestsComplete() {
    bIsRunning = false;
}

FText SDATestRunner::GetStatusText() const {
    return bIsRunning
               ? LOCTEXT("StatusRunningLabel", "Status: Running...")
               : LOCTEXT("StatusNotRunningLabel", "Status: Not Running");
}

TSharedRef<SWidget> SDATestRunner::GetHistogramWidget() const {
    return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE

