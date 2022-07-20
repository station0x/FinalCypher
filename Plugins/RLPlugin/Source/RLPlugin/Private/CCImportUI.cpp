// Fill out your copyright notice in the Description page of Project Settings.

#include "CCImportUI.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"

#define CC_SETTINGS TEXT( "CCImporter" )

UCCImportUI::UCCImportUI(const FObjectInitializer& ObjectInitializer)
    : Super( ObjectInitializer )
    , isCCAutoSetup( true )
    , isHQSkin( false )
    , isLWSkin( false )
    , isStandardSkin( true )
    , isCanceled( true )
    , isCanChangeAutoEnable( true )
    , hasCCShader( false )
    , isLiveLink( true )
{
    ReadConfig();
}

void UCCImportUI::ReadConfig()
{
    if ( !GConfig )
    {
        return;
    }
    GConfig->GetBool( CC_SETTINGS, TEXT( "CCAutoSetup" ), isCCAutoSetup, GEngineIni );
    GConfig->GetBool( CC_SETTINGS, TEXT( "HQSkin" ), isHQSkin, GEngineIni );
    GConfig->GetBool( CC_SETTINGS, TEXT( "LWSkin" ), isLWSkin, GEngineIni );
    GConfig->GetBool( CC_SETTINGS, TEXT( "StandardSkin" ), isStandardSkin, GEngineIni );
}

void UCCImportUI::WriteConfig()
{
    GConfig->SetBool( CC_SETTINGS, TEXT( "CCAutoSetup" ), isCCAutoSetup, GEngineIni );
    GConfig->SetBool( CC_SETTINGS, TEXT( "HQSkin" ), isHQSkin, GEngineIni );
    GConfig->SetBool( CC_SETTINGS, TEXT( "LWSkin" ), isLWSkin, GEngineIni );
    GConfig->SetBool( CC_SETTINGS, TEXT( "StandardSkin" ), isStandardSkin, GEngineIni );
}