// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CompilerBooster.h"
#include "CompilerBoosterStyle.h"
#include "CompilerBoosterCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Blutility/Public/EditorUtilitySubsystem.h"
#include "Editor/Blutility/Public/IBlutilityModule.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"
#include "UMGEditor/Public/WidgetBlueprint.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectBaseUtility.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

static const FName CompilerBoosterTabName("CompilerBooster");

#define LOCTEXT_NAMESPACE "FCompilerBoosterModule"

void FCompilerBoosterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FCompilerBoosterStyle::Initialize();
	FCompilerBoosterStyle::ReloadTextures();

	FCompilerBoosterCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCompilerBoosterCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FCompilerBoosterModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FCompilerBoosterModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FCompilerBoosterModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FCompilerBoosterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FCompilerBoosterStyle::Shutdown();

	FCompilerBoosterCommands::Unregister();
}

void FCompilerBoosterModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	UObject* WidgetObj = LoadObject<UObject>(nullptr, TEXT("/CompilerBooster/CompilerBooster.CompilerBooster"));
	UEditorUtilityWidgetBlueprint* WidgetBP = (UEditorUtilityWidgetBlueprint*)WidgetObj;
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
}

void FCompilerBoosterModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FCompilerBoosterCommands::Get().PluginAction);
}

void FCompilerBoosterModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FCompilerBoosterCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCompilerBoosterModule, CompilerBooster)