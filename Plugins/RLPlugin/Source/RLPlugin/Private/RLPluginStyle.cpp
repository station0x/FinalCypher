// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RLPluginStyle.h"
#include "RLPlugin.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FRLPluginStyle::StyleInstance = NULL;

void FRLPluginStyle::Initialize()
{
    if ( !StyleInstance.IsValid() )
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle( *StyleInstance );
    }
}

void FRLPluginStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( *StyleInstance );
    ensure( StyleInstance.IsUnique() );
    StyleInstance.Reset();
}

FName FRLPluginStyle::GetStyleSetName()
{
    static FName StyleSetName( TEXT( "RLPluginStyle" ) );
    return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16( 16.0f, 16.0f );
const FVector2D Icon20x20( 20.0f, 20.0f );
const FVector2D Icon40x40( 40.0f, 40.0f );

TSharedRef< FSlateStyleSet > FRLPluginStyle::Create()
{
    TSharedRef< FSlateStyleSet > Style = MakeShareable( new FSlateStyleSet( "RLPluginStyle" ) );
    Style->SetContentRoot( IPluginManager::Get().FindPlugin( "RLPlugin" )->GetBaseDir() / TEXT( "Resources" ) );

    Style->Set( "RLPlugin.PluginAction", new IMAGE_BRUSH( TEXT( "ButtonIcon_40x" ), Icon40x40 ) );
    Style->Set( "RLPlugin.PluginAction.Small", new IMAGE_BRUSH( TEXT( "ButtonIcon_40x" ), Icon20x20 ) );
    return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FRLPluginStyle::ReloadTextures()
{
    if ( FSlateApplication::IsInitialized() )
    {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}

const ISlateStyle& FRLPluginStyle::Get()
{
    return *StyleInstance;
}
