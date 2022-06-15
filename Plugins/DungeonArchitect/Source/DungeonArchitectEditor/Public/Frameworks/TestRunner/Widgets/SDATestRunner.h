//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;
class FSpawnTabArgs;
class SDockTab;

////////////////////////// Widget ////////////////////////// 
class SDATestRunner
    : public SCompoundWidget {
public:
    SLATE_BEGIN_ARGS(SDATestRunner) {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab,
                   const TSharedPtr<SWindow>& ConstructUnderWindow);
    virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

protected:
    TSharedRef<SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier) const;
    static void FillWindowMenu(FMenuBuilder& MenuBuilder, TSharedPtr<FTabManager> TabManager);
    TSharedRef<SWidget> CreateToolbarWidget() const;

    void OnStartStopClicked();
    bool IsStartStopButtonEnabled() const { return true; }
    virtual void NotifyTestsComplete();
    virtual bool ValidateConfiguration(FText& OutErrorMessage) { return true; }
    virtual FText GetStatusText() const;
    virtual UObject* GetSettingsObject() = 0;

    virtual void StartService() = 0;
    virtual void StopService() = 0;
    virtual bool IsServiceRunning() const = 0;

    virtual void TickService() {
    }

    virtual TSharedRef<SWidget> GetHistogramWidget() const;

private:
    void HandleStartService();
    void HandleStopService();
    void BindCommands();
    FText GetStartStopButtonLabel() const;
    FText GetStartStopButtonTooltip() const;
    FSlateIcon GetStartStopButtonIcon() const;
    
protected:
    TSharedPtr<FTabManager> TabManager;
    TSharedPtr<IDetailsView> PropertyEditor;
    TSharedPtr<FUICommandList> CommandList;

private:
    bool bIsRunning = false;
};

