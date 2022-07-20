// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
//#define UE418
//#define UE419
//#define UE420
//#define UE421
//#define UE422
//#define UE423
//#define UE424
#include "RLPlugin.h"
#include "RLPluginStyle.h"
#include "RLPluginCommands.h"
#include "CCImportWindow.h"
#include "CCImportUI.h"
#include "RLString.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/coreUObject/Public/UObject/Class.h"
#include "LevelEditor.h"
#include <Materials/MaterialExpressionFunctionInput.h>
#include <Materials/MaterialExpressionAdd.h>
#include "Materials/Material.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "RenderUtils.h"
#include "Engine/Texture2D.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "FbxMeshUtils.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/UnrealEd/Classes/Factories/MaterialFactoryNew.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Animation/Rig.h"
#include "Runtime/Engine/Classes/Animation/Skeleton.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "Runtime/Engine/Classes/Animation/AnimBlueprint.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Runtime/Engine/Classes/Engine/SimpleConstructionScript.h"
#include "Runtime/Engine/Classes/Engine/SCS_Node.h"
#include "Engine/InheritableComponentHandler.h"
#include "Editor/UnrealEd/Public/Kismet2/KismetEditorUtilities.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Editor/BlueprintGraph/Classes/K2Node.h"
#include "Editor/BlueprintGraph/Classes/K2Node_Variable.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableGet.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UObject/UnrealType.h"
#include "Editor/UnrealEd/Public/EdMode.h"
#include "Editor/UnrealEd/Public/EditorModeManager.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsAsset.h"
#include "Editor/ClothingSystemEditorInterface/Public/ClothingSystemEditorInterfaceModule.h"
#include "Editor/ClothingSystemEditorInterface/Public/ClothingAssetFactoryInterface.h"
#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION <= 23
#include "Runtime/ClothingSystemRuntime/Public/Assets/ClothingAsset.h"
#else
#include "ClothingAsset.h"
#endif

#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 24
#include "Runtime/ClothingSystemRuntimeNv/Public/Assets/ClothingAssetNv.h"
#include "Runtime/ClothingSystemRuntimeCommon/Public/Utils/ClothingMeshUtils.h"
#endif

#if ( ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION > 24 ) || ENGINE_MAJOR_VERSION >= 5
#include "Runtime/ClothingSystemRuntimeCommon/Public/ClothingAsset.h"
#include "Runtime/ClothingSystemRuntimeNv/Public/ClothConfigNv.h"

#endif

#include "Editor/SkeletalMeshEditor/Public/ISkeletalMeshEditor.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Editor/UnrealEd/Classes/Factories/PhysicsAssetFactory.h"

#include <exception>
#include <algorithm>
#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 18
#include "Runtime/Engine/Classes/Materials/MaterialExpressionTextureSample.h"
#else
#include "Rendering/SkeletalMeshModel.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshModel.h"
#endif

#include "Runtime/Engine/Classes/Materials/MaterialExpressionConstant.h"
#include "Runtime/Engine/Classes/Materials/MaterialExpressionMultiply.h"
#include "Runtime/Engine/Classes/Materials/MaterialExpressionVertexNormalWS.h"
#include "Runtime/Engine/Classes/Materials/MaterialExpressionSubtract.h"
#include "Runtime/Engine/Classes/Materials/MaterialExpressionScalarParameter.h"
#include "Editor/UnrealEd/Classes/Factories/MaterialInstanceConstantFactoryNew.h"
#include "Editor/UnrealEd/Classes/Factories/SubsurfaceProfileFactory.h"
#include "Developer/AssetTools/Public/AssetTypeActions_Base.h"

#include "Runtime/Slate/Public/Widgets/Notifications/SNotificationList.h"
#include "Runtime/Slate/Public/Framework/Notifications/NotificationManager.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "Runtime/Core/Public/Internationalization/Text.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "Editor/UnrealEd/Public/AnimationEditorUtils.h"
#include "Editor/UnrealEd/Public/EditorReimportHandler.h"
#include "Editor/UnrealEd/Classes/Factories/AnimBlueprintFactory.h"
#if ENGINE_MAJOR_VERSION < 5
#include "Editor/UnrealEd/Public/PhysicsAssetUtils.h"
#else
#include "Developer/PhysicsUtilities/Public/PhysicsAssetUtils.h"
#include "ChaosCloth/ChaosClothConfig.h"
#endif
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintTemplate.h"
#include "Runtime/Engine/Classes/Materials/MaterialInterface.h"
#include "Runtime/Engine/Classes/Engine/SubsurfaceProfile.h"
#include "Editor/SkeletalMeshEditor/Public/ISkeletalMeshEditorModule.h"
#include "Editor/UnrealEd/Public/AssetEditorModeManager.h"
static const FName RLPluginTabName( "CC_Auto_Setup" );

#define DEC(x,n) ((x/(int)1E##n)%10)
#define LOCTEXT_NAMESPACE "FRLPluginModule"
#define DEFAULT_MATERIAL_EXP_X 380
#define DEFAULT_MATERIAL_EXP_Y 220
#define LODGROUP "_LODGroup"
#define AUTOSETUP_BIG_VERSION 1
#define AUTOSETUP_MID_VERSION 10

//Digital Hunam Shader Name
#define DigitalTongue L"RLTongue"
#define DigitalTeeth L"RLTeethGum"
#define DigitalEye L"RLEye"
#define DigitalHead L"RLHead"
#define DigitalSkin L"RLSkin"
#define DigitalTearline L"RLEyeTearline"
#define DigitalOcculsion L"RLEyeOcclusion"
#define RLHair L"RLHair"
#define RLGeneralSSS L"RLSSS"

//Material Keyword
#define UpperTeethKeyWord           "std_upper_teeth"
#define LowerTeethKeyWord           "std_lower_teeth"
#define TougueKeyWord               "tongue"
#define TransparencyKeyWord         "_transparency"
#define ScalpKeyWord                "scalp"
#define EyelashKeyWord              "eyelash"
#define CorneaKeyWord               "std_cornea_"

#define HeadKeyWord                 "head"
#define TearKeyWord                 "tear"
#define EyeOcclusionKeyWord         "eye_occlusion"

//Character UID
#define CC_G1_UID L"RL_G6_Standard_Series"
#define CC_G3_UID L"RL_CharacterCreator_Base_Std_G3"
#define CC_G3PLUS_UID L"RL_CC3_Plus"
#define CC_GAME_BASE_MULTI_UID L"RL_CharacterCreator_Base_Game_G1_Multi_UV"
#define CC_GAME_BASE_ONE_UID L"RL_CharacterCreator_Base_Game_G1_One_UV"
#define CC_GAME_BASE_DIVIDE_EYELASH_UID L"RL_CharacterCreator_Base_Game_G1_Divide_Eyelash_UV"
#define TYPE_CD_IC7_UID L"NonStdLookAtDataCopyFromCCBase"
#define TYPE_C_UID L"ActorBuild"
#define TYPE_D_UID L"ActorScan"

#define BONE_TYPE_G1          "G1"
#define BONE_TYPE_G3          "G3"
#define BONE_TYPE_G3PLUS      "G3Plus"
#define BONE_TYPE_GAME_MULTI  "GameBase_Multi"
#define BONE_TYPE_GAME_SINGLE "GameBase_Single"

#if ENGINE_MAJOR_VERSION < 5
#define MeshGetMaterial StaticMaterials
#else
#define MeshGetMaterial GetStaticMaterials()
#endif
TMap<FString, FString> CharacterGenerationBoneMap =
{
    { CC_G1_UID,                        BONE_TYPE_G1 },
    { CC_G3_UID,                        BONE_TYPE_G3 },
    { CC_G3PLUS_UID,                    BONE_TYPE_G3PLUS },
    { TYPE_CD_IC7_UID,                  BONE_TYPE_G3PLUS },
    { TYPE_C_UID,                       BONE_TYPE_G3PLUS },
    { TYPE_D_UID,                       BONE_TYPE_G3PLUS },
    { CC_GAME_BASE_MULTI_UID,           BONE_TYPE_GAME_MULTI },
    { CC_GAME_BASE_ONE_UID,             BONE_TYPE_GAME_SINGLE },
    { CC_GAME_BASE_DIVIDE_EYELASH_UID,  BONE_TYPE_GAME_SINGLE }
};

#define HeadKeyWordG1                 "skin_head"

TMap<FString, FString> BoneTypeSkinKeyWord =
{
    { BONE_TYPE_G1,          "skin_body" }, // skin_head
    { BONE_TYPE_G3,          "std_skin_" },
    { BONE_TYPE_G3PLUS,      "std_skin_" },
    { BONE_TYPE_GAME_MULTI,  "ga_skin_" },
    { BONE_TYPE_GAME_SINGLE, "ga_skin_" }
};

static const FString MATERIAL_FOLDER_NAME = "Materials";
static const FString SUBSURFACE_PROFILE_FOLDER_NAME = "SSSProfile";

static const FString TearPackageName = "/Game/CC_Shaders/TearLineShader/RL_TearLine";
static const FString EyeOcclusionPackageName = "/Game/CC_Shaders/EyeOcclusionShader/RL_EyeOcclusion";
static const FString HairPackageName = "/Game/CC_Shaders/HairShader/RL_Hair";
static const FString HairSpecularPackageName = "/Game/CC_Shaders/HairShader/RL_Hair_Specular";
static const FString EyelashPackageName = "/Game/CC_Shaders/EyelashShader/RL_Eyelash";
static const FString EyePackageName = "/Game/CC_Shaders/EyeShader/RL_EyeRefractive";
static const FString HQSkinPackageName = "/Game/CC_Shaders/SkinShader/RL_HQSkin";
static const FString LWSkinPackageName = "/Game/CC_Shaders/SkinShader/RL_LWSkin";
static const FString GumsTongueTeethPackageName = "/Game/CC_Shaders/GumsTongueTeethShader/RL_GumsTongueTeeth";
static const FString StandardORMPackage = "/Game/CC_Shaders/StandardShader/RL_Standard";
static const FString StandardORMOpacityPackage = "/Game/CC_Shaders/StandardShader/RL_Standard_Opacity"; 
static const FString GeneralSSSPackage = "/Game/CC_Shaders/RL_SSS/RL_SSS";
                                                                                                      //    radius, distrib, IOR,   extinct, normal scale,  R0,R2, lerp
static const RLScatter g_kHeadSss( FLinearColor( 0.827451, 0.678431, 0.458824 ), FLinearColor( 1, 0.368627, 0.29804 ), 2, 0.93, 1.55, 1, 0.08, 0.75, 1.3, 0.85 );
static const RLScatter g_kSkinSss( FLinearColor( 0.827451, 0.678431, 0.458824 ), FLinearColor( 1, 0.368627, 0.29804 ), 2, 0.4, 3, 1, 0.08, 0.75, 1.3, 0.85 );
static const RLScatter g_kTeethSss( FLinearColor( 1, 0.9, 0.8 ), FLinearColor( 1, 0.9, 0.8 ), 1, 0.93, 1.55, 1, 0.3646, 0.75, 1.3, 0.9 );
static const RLScatter g_kEyeSss( FLinearColor( 1, 1, 1 ), FLinearColor( 1, 1, 1 ), 5, 0.93, 1.55, 1, 0.08, 0.75, 1.3, 0.85 );
static const RLScatter g_kGeneralSss( FLinearColor( 1, 1, 1 ), FLinearColor( 1, 1, 1 ), 13, 0.85, 1.55, 0.5, 0.25, 0.3, 1.8, 0.5 );
URig* FRLPluginModule::m_pEngineHumanoidRig = nullptr;

void RemovePostfix( FString& strString, const char& cChar )
{
    int nPosition = 0;
    if( strString.FindLastChar( cChar, nPosition ) )
    {
        int nCount = strString.Len() - nPosition;
        strString.RemoveAt( nPosition, nCount, true );
    }
}

void RemoveMaterialPostfix( FString& strMaterialName )
{
    if( strMaterialName.Contains( "_Inst" ) )
    {
        RemovePostfix( strMaterialName, '_' );
    }

    if( strMaterialName.Contains( "_LWHQ" ) || strMaterialName.Contains( "_HQ" ) || strMaterialName.Contains( "_LW" ) )
    {
        RemovePostfix( strMaterialName, '_' );
    }

    // 這樣結果也是錯的,還是讓 material 錯的明顯一點
    //if ( strMaterialName.Contains( "_ncl1_" ) )
    //{
    //    RemovePostfix( strMaterialName, '_' );
    //    RemovePostfix( strMaterialName, '_' );
    //}
}

FString RemoveInvalidChar( FString s )
{
    s = s.Replace( TEXT( " " ), TEXT( "_" ), ESearchCase::IgnoreCase );
    s = s.Replace( TEXT( "(" ), TEXT( "_" ), ESearchCase::IgnoreCase );
    s = s.Replace( TEXT( ")" ), TEXT( "_" ), ESearchCase::IgnoreCase );
    return s;
}

void FRLPluginModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    FRLPluginStyle::Initialize();
    FRLPluginStyle::ReloadTextures();

    FRLPluginCommands::Register();

    m_kPluginCommands = MakeShareable( new FUICommandList );
    m_kPluginCommands->MapAction( FRLPluginCommands::Get().PluginAction,
                                  FExecuteAction::CreateRaw( this, &FRLPluginModule::PluginButtonClicked ),
                                  FCanExecuteAction() );

    FLevelEditorModule& kLevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );

    // init menu
    TSharedPtr<FExtender> spMenuExtender = MakeShareable( new FExtender );
    spMenuExtender->AddMenuExtension( "WindowLayout",
                                      EExtensionHook::After,
                                      m_kPluginCommands,
                                      FMenuExtensionDelegate::CreateRaw( this, &FRLPluginModule::AddMenuExtension ) );
    kLevelEditorModule.GetMenuExtensibilityManager()->AddExtender( spMenuExtender );

    // init toolbar
    FName kExtensionHook = "";
    EExtensionHook::Position eHookPosition;
    if ( ENGINE_MAJOR_VERSION < 5 )
    {
        kExtensionHook = "Settings";
        eHookPosition = EExtensionHook::After;
    }
    else
    {
        kExtensionHook = "Play";
        eHookPosition = EExtensionHook::Before;
    }
    TSharedPtr<FExtender> spToolbarExtender = MakeShareable( new FExtender );
    spToolbarExtender->AddToolBarExtension( kExtensionHook,
                                            eHookPosition,
                                            m_kPluginCommands,
                                            FToolBarExtensionDelegate::CreateRaw( this, &FRLPluginModule::AddToolbarExtension ) );
    kLevelEditorModule.GetToolBarExtensibilityManager()->AddExtender( spToolbarExtender );
}

void FRLPluginModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    FRLPluginStyle::Shutdown();
    FRLPluginCommands::Unregister();
}

void FRLPluginModule::PluginButtonClicked()
{
    TArray<FAssetData> kAssetDatas;
    GEditor->GetContentBrowserSelections( kAssetDatas ); //取得選擇的檔案

    TArray<FString> kLODPathList;
    AutoSetup( kAssetDatas, "Standard", kLODPathList, false );
}

void FRLPluginModule::AutoSetup( TArray<FAssetData>& kAssetDatas,
                                 FString strShaderType,
                                 TArray<FString>& kLODPathList,
                                 bool bIsDragFbx )
{
    IPlatformFile& kPlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    // shader path
    FString strShaderPath = FPaths::ProjectContentDir() + "CC_Shaders/";
    if ( !bIsDragFbx )
    {
        UCCImportUI* pImportUI = NewObject<UCCImportUI>();
        if ( !pImportUI )
        {
            return;
        }
        pImportUI->isCCAutoSetup = true;
        pImportUI->isCanChangeAutoEnable = false;
        if ( !kPlatformFile.DirectoryExists( *strShaderPath ) )
        {
            pImportUI->hasCCShader = false;
            pImportUI->isHQSkin = false;
            pImportUI->isLWSkin = false;
            pImportUI->isStandardSkin = true;
            pImportUI->WriteConfig();
            pImportUI->SaveConfig();
        }
        else
        {
            pImportUI->hasCCShader = true;
        }

        if ( !IsRunningCommandlet() )
        {
            TSharedPtr<SWindow> spParentWindow = nullptr;
            if ( FModuleManager::Get().IsModuleLoaded( "MainFrame" ) )
            {
                IMainFrameModule& kMainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>( "MainFrame" );
                spParentWindow = kMainFrame.GetParentWindow();
            }
            if ( !spParentWindow )
            {
                return;
            }
            TSharedPtr<SCCImportWindow> spImportWindow = nullptr;
            TSharedRef<SWindow> spWindow = SNew( SWindow )
                .IsTopmostWindow( true )
                .Title( LOCTEXT( "CCAutoSetup_ImportWindowTitle", "Character Creator & iClone Auto Setup Import Options" ) )
                .SizingRule( ESizingRule::Autosized );
            spWindow->SetContent( SAssignNew( spImportWindow, SCCImportWindow )
                                  .ImportUI( pImportUI )
                                  .WidgetWindow( spWindow ) );
            FSlateApplication::Get().AddModalWindow( spWindow, spParentWindow, false );

            if ( pImportUI->isCanceled )
            {
                return;
            }
            pImportUI->WriteConfig();
            pImportUI->SaveConfig();
        }
        else
        {
            // For LiveLink Import FBX
            pImportUI->WriteConfig();
            pImportUI->SaveConfig();
        }

        strShaderType = "Standard";
        if ( pImportUI->isStandardSkin )
        {
            strShaderType = "Standard";
        }
        else if ( pImportUI->isHQSkin )
        {
            strShaderType = "HQ";
        }
        else if ( pImportUI->isLWSkin )
        {
            strShaderType = "LW";
        }
    }

    for( auto kAssetData : kAssetDatas )
    {
        FScopedSlowTask kSlowTask( 100.f, NSLOCTEXT( "AutoProcessing", "Auto_Processing", "Auto-Processing..." ) );
        kSlowTask.MakeDialog();
        kSlowTask.EnterProgressFrame( 15.f );

        // root path
        FString strRootGamePath = FPaths::GetPath( kAssetData.ObjectPath.ToString() );
        FString strRootPath = strRootGamePath + "/";
        strRootPath.RemoveFromStart( TEXT( "/Game/" ) );
        strRootPath = FPaths::ProjectContentDir() + strRootPath;

        // texture path
        FString strTextureGamePath = strRootGamePath + "/textures";
        FString strTexturePath = strTextureGamePath + "/";
        strTexturePath.RemoveFromStart( TEXT( "/Game/" ) );
        strTexturePath = FPaths::ProjectContentDir() + strTexturePath;

        // fbm texture path
        FString strFbmTextureGamePath = strRootGamePath + "/" + kAssetData.AssetName.ToString() + "_fbm";
        FString strFbmTexturePath = strFbmTextureGamePath + "/";
        strFbmTexturePath.RemoveFromStart( TEXT( "/Game/" ) );
        strFbmTexturePath = FPaths::ProjectContentDir() + strFbmTexturePath;

        // material path
        FString strMaterialGamePath = strRootGamePath + "/" + MATERIAL_FOLDER_NAME + "/" + FPaths::GetBaseFilename( kAssetData.AssetName.ToString() );
        FString strMaterialPath = strRootPath + MATERIAL_FOLDER_NAME + "/" + FPaths::GetBaseFilename( kAssetData.AssetName.ToString() );

        // json file path
        FString strJsonFilePath = FPaths::ProjectSavedDir() + "JsonData/" + kAssetData.AssetName.ToString() + ".json";
        if( !FPaths::FileExists( strJsonFilePath ) )
        {
            return;
        }

        // subsurface profile path
        FString strSubsurfaceProfilePath = strRootGamePath + "/" + SUBSURFACE_PROFILE_FOLDER_NAME + "/" + kAssetData.AssetName.ToString();
        CreateFolder( strSubsurfaceProfilePath );

        if( !bIsDragFbx )
        {
            if( kAssetData.GetClass() != USkeletalMesh::StaticClass()
                && kAssetData.GetClass() != UStaticMesh::StaticClass() )
            {
                FText strMessage = FText::FromString( TEXT( "Please select a Skeletal Mesh or Static Mesh" ) );
                FMessageDialog::Open( EAppMsgType::Ok, strMessage );
                return;
            }

            // 版本檢查
            if( !CheckAutoSetupVersionPass( strJsonFilePath ) )
            {
                return;
            }

            if( kPlatformFile.DirectoryExists( *strTexturePath ) )
            {
                FString strTargetFolderPath = strTexturePath + kAssetData.AssetName.ToString() + "/";
                MoveTextureFile( strTargetFolderPath );
            }
        }

        // Check material instance
        m_bIsMaterialInstance = kPlatformFile.DirectoryExists( *strShaderPath );
        kSlowTask.EnterProgressFrame( 15.f );

        FString strGeneration = "";
        FString strBoneType = "NULL";
        bool bSupportShaderSelect = true;
        TMap< FString, RLMaterialData > kMaterialMap;
        TMap< FString, TArray<RLPhysicsCollisionShapeData> > kCollisionShapeMap;
        ParseJson( strJsonFilePath, strGeneration, strBoneType, bSupportShaderSelect, kMaterialMap, kCollisionShapeMap );

        TArray<FString> kTexturePathList;
        CreateTexturesPathList( strRootGamePath, kTexturePathList );
        RemoveInvalidTexture( kMaterialMap, strTextureGamePath, strFbmTextureGamePath, kTexturePathList );

        FString strTextureFilesGamePath = strTextureGamePath + "/" + kAssetData.AssetName.ToString();
        FString strTextureFilesPath = strTexturePath + kAssetData.AssetName.ToString();
        if( kAssetData.GetClass() == USkeletalMesh::StaticClass() )
        {
            // for舊版json
            if( strGeneration.IsEmpty() )
            {
                strBoneType = GetBoneType( kAssetData );
            }

            ShowInfo( "Start Auto-Processing...", 7.f );
            USkeletalMesh* pMesh = dynamic_cast< USkeletalMesh* >( kAssetData.GetAsset() );
            if( !pMesh )
            {
                return;
            }

            int nInvalidMaterialNum = 0;
            TArray<FString> kOutFiles;
            if( !bIsDragFbx )
            {
                if( kAssetData.AssetName.ToString().Contains( "_LOD0" ) )
                {
                    const void* pParentWindow = FSlateApplication::Get().FindBestParentWindowHandleForDialogs( nullptr );
                    IDesktopPlatform* pDesktopPlatform = FDesktopPlatformModule::Get();
                    if( pDesktopPlatform )
                    {
                        pDesktopPlatform->OpenFileDialog( pParentWindow,
                                                          TEXT( "Import LOD1~N.fbx" ),
                                                          TEXT( "C:\\" ),
                                                          TEXT( "" ),
                                                          TEXT( "FBX|*.fbx;*.Fbx" ),
                                                          EFileDialogFlags::Multiple,
                                                          kOutFiles );
                    }
                    kOutFiles.Sort();
                    for( int i = 0; i < kOutFiles.Num(); ++i )
                    {
                        FString strCheckLOD = "_LOD" + FString::FromInt( i + 1 );
                        if( !kOutFiles[ i ].Contains( strCheckLOD ) )
                        {
                            FText strMessage = FText::FromString( TEXT( "Make sure to select the LOD files in proper sequential order." ) );
                            FMessageDialog::Open( EAppMsgType::Ok, strMessage );
                            return;
                        }
                    }
                }
            }
            else
            {
                kOutFiles = kLODPathList;
                kOutFiles.Sort();
            }


            //PhysicsAsset & Skeleton

            FString strPhysicsAssetPath = strRootGamePath + "/" + kAssetData.AssetName.ToString() + "_PhysicsAsset." + kAssetData.AssetName.ToString() + "_PhysicsAsset";
            UPhysicsAsset* pPhysicsAsset = Cast<UPhysicsAsset>( StaticLoadObject( UPhysicsAsset::StaticClass(), NULL, *( strPhysicsAssetPath ) ) );
            if ( !pPhysicsAsset )
            {
                UObject* pPhysicsAssetObject = RLCreatePhysicsAsset( NAME_None, nullptr, pMesh );
                pPhysicsAsset = Cast<UPhysicsAsset>( pPhysicsAssetObject );
            }
            USkeleton* pSkeleton = GetAssetSkeleton( kAssetData );
            if ( !pSkeleton )
            {
                return;
            }

            CreatePhysicCollisionShape( pPhysicsAsset, pMesh, pSkeleton, kCollisionShapeMap );

            FString strSrcMaterialFolderPath = strRootPath;
            FString strTgtMaterialFolderPath = strRootPath + "Materials/";
            FString strFbxName = kAssetData.AssetName.ToString();

            // 為了LODGROUP重新配置而找出LOD材質球
            if( kAssetData.AssetName.ToString().Contains( LODGROUP ) )
            {
                if( kMaterialMap.Num() != 0 )
                {
                    findLODGroupMaterialJson( kMaterialMap, strFbxName, strRootGamePath, pMesh, m_bIsMaterialInstance );
                }
                else
                {
                    findLODGroupMaterial( strRootGamePath, pMesh );
                }
            }

            CreateFolder( strMaterialPath );
            DeleteDefaultSkeletalMaterial( strSrcMaterialFolderPath,
                                           strTgtMaterialFolderPath,
                                           strFbxName,
                                           strRootGamePath,
                                           pMesh,
                                           m_bIsMaterialInstance );

            const auto& nMaterialCount = pMesh->Materials.Num();
            for( int i = 0; i < nMaterialCount; ++i )
            {
                auto pSkeletalMaterial = pMesh->Materials[ i ];
                FString strSlowTask = "Auto-Processing : " + pSkeletalMaterial.MaterialSlotName.ToString();
                if( kAssetData.AssetName.ToString().Contains( "_LOD0" ) )
                {
                    kSlowTask.EnterProgressFrame( 60 / nMaterialCount, FText::FromString( strSlowTask ) );
                }
                else
                {
                    kSlowTask.EnterProgressFrame( 70 / nMaterialCount, FText::FromString( strSlowTask ) );
                }

                bool bSame = false;
                if ( pSkeletalMaterial.MaterialInterface )
                {
                    FString strMaterialName = pSkeletalMaterial.MaterialInterface->GetName();
                    RemoveMaterialPostfix( strMaterialName );
                    bSame = pSkeletalMaterial.MaterialSlotName.ToString() == strMaterialName;
                }

                if( pSkeletalMaterial.MaterialInterface && bSame )
                {
                    UMaterial* pMaterial = pSkeletalMaterial.MaterialInterface->GetMaterial();
                    if( !pMaterial )
                    {
                        continue;
                    }

                    FString strMaterialName = pSkeletalMaterial.MaterialInterface->GetName();
                    RemoveMaterialPostfix( strMaterialName );

                    RLMaterialData* pMaterialData = kMaterialMap.Find( strMaterialName );
                    if ( !pMaterialData )
                    {
                        continue;
                    }
                    if ( pMaterialData->GetShaderData() == nullptr && strShaderType == "Standard" )
                    {
                        ConvertEyeMaterialName( strMaterialName );
                        pMaterialData = kMaterialMap.Find( strMaterialName );
                    }

                    CheckShaderTypeChange( strFbxName,
                                           strShaderType,
                                           pMesh,
                                           pMaterial,
                                           pSkeletalMaterial.MaterialInterface,
                                           pMaterialData,
                                           i,
                                           strRootGamePath,
                                           m_bIsMaterialInstance,
                                           strBoneType );

                    //要重取一次，material有可能被上面func.改掉
                    pMaterial = pMesh->Materials[ i ].MaterialInterface->GetMaterial();
                    UMaterialInterface* pMaterialInterface = pMesh->Materials[ i ].MaterialInterface;

                    UMaterialInstanceConstant* pInstUMaterialInterface = nullptr;
                    AssignMaterialInstanceJson( pInstUMaterialInterface,
                                                strMaterialName,
                                                pMaterialData,
                                                kTexturePathList,
                                                pMaterial,
                                                pMaterialInterface,
                                                strTextureFilesGamePath,
                                                strFbmTextureGamePath,
                                                strMaterialGamePath,
                                                strSubsurfaceProfilePath,
                                                strBoneType,
                                                strShaderType );
                    if( pInstUMaterialInterface )
                    {
                        FSkeletalMaterial* InstFSMaterial = new FSkeletalMaterial( pInstUMaterialInterface,
                                                                                   true,
                                                                                   false,
                                                                                   pMesh->Materials[ i ].MaterialSlotName,
                                                                                   pMesh->Materials[ i ].ImportedMaterialSlotName );
                        pMesh->Materials.RemoveAt( i );
                        pMesh->Materials.Insert( *InstFSMaterial, i );
                    }
                    pMesh->MarkPackageDirty();
                    pMesh->PostEditChange();
                }
                else
                {
                    ++nInvalidMaterialNum;
                    FString strPackageName = "";
                    UMaterial* pUnrealMaterial = nullptr;
                    if( pMesh->Materials[ i ].MaterialSlotName.ToString().Contains( " " ) )
                    {
                        FString materialSlotName = pMesh->Materials[ i ].MaterialSlotName.ToString().Replace( TEXT( " " ), TEXT( "_" ), ESearchCase::IgnoreCase );
                        pMesh->Materials[ i ].MaterialSlotName = FName( *materialSlotName );
                    }
                    if( !m_bIsMaterialInstance )
                    {
                        strPackageName = strRootGamePath + "/Materials/";
                        strPackageName += pMesh->Materials[ i ].MaterialSlotName.ToString();
                        UPackage* pPackage = CreatePackage( NULL, *strPackageName );
                        UMaterialFactoryNew* pMaterialFactory = NewObject<UMaterialFactoryNew>();
                        pUnrealMaterial = ( UMaterial* )pMaterialFactory->FactoryCreateNew( UMaterial::StaticClass(),
                                                                                            pPackage,
                                                                                            *pMesh->Materials[ i ].MaterialSlotName.ToString(),
                                                                                            RF_Standalone | RF_Public,
                                                                                            NULL,
                                                                                            GWarn );
                        FAssetRegistryModule::AssetCreated( pUnrealMaterial );
                    }
                    else
                    {
                        FString strPackage = StandardORMPackage;
                        UMaterialInterface* pStandardMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strPackage );
                        if( pStandardMaterialInterface )
                        {
                            FString strMaterialName = pMesh->Materials[ i ].MaterialSlotName.ToString();
                            strPackageName = strMaterialGamePath + "/" + strMaterialName + "_Inst";

                            UMaterialInstanceConstantFactoryNew* pFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
                            pFactory->InitialParent = pStandardMaterialInterface;

                            UPackage* pPackage = CreatePackage( NULL, *strPackageName );
                            FString strMaterialInstanceName = strMaterialName + "_Inst";
                            UMaterialInstanceConstant* pMaterialInstance = ( UMaterialInstanceConstant* )pFactory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(),
                                                                                                                                     pPackage,
                                                                                                                                     FName( *strMaterialInstanceName ),
                                                                                                                                     RF_Standalone | RF_Public, NULL,
                                                                                                                                     GWarn );
                            FAssetRegistryModule::AssetCreated( pMaterialInstance );
                        }
                    }
                    UMaterialInterface* pMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strPackageName );
                    if( !pMaterialInterface )
                    {
                        UE_LOG( LogTemp, Warning, TEXT( "NO MaterialInterface" ) );
                    }
                    FSkeletalMaterial* pFSMaterial = new FSkeletalMaterial( pMaterialInterface,
                                                                            true,
                                                                            false,
                                                                            *pMesh->Materials[ i ].MaterialSlotName.ToString(),
                                                                            *pMesh->Materials[ i ].ImportedMaterialSlotName.ToString() );
                    pMesh->Materials.RemoveAt( i );
                    pMesh->Materials.Insert( *pFSMaterial, i );
                    pMesh->MarkPackageDirty();
                    pMesh->PostEditChange();

                    FString strMaterialName = pMesh->Materials[ i ].MaterialInterface->GetName();
                    RemoveMaterialPostfix( strMaterialName );

                    RLMaterialData* pMaterialData = kMaterialMap.Find( strMaterialName );
                    if ( !pMaterialData )
                    {
                        continue;
                    }
                    if ( pMaterialData->GetShaderData() == nullptr && strShaderType == "Standard" )
                    {
                        ConvertEyeMaterialName( strMaterialName );
                        pMaterialData = kMaterialMap.Find( strMaterialName );
                    }

                    UMaterial* pMaterial = pMesh->Materials[ i ].MaterialInterface->GetMaterial();
                    CheckShaderTypeChange( strFbxName,
                                           strShaderType,
                                           pMesh,
                                           pMaterial,
                                           pMesh->Materials[ i ].MaterialInterface,
                                           pMaterialData,
                                           i,
                                           strRootGamePath,
                                           m_bIsMaterialInstance,
                                           strBoneType );

                    //要重取一次，material有可能被上面func.改掉
                    pMaterial = pMesh->Materials[ i ].MaterialInterface->GetMaterial();
                    pMaterialInterface = pMesh->Materials[ i ].MaterialInterface;

                    UMaterialInstanceConstant* pInstUMaterialInterface = nullptr;
                    AssignMaterialInstanceJson( pInstUMaterialInterface,
                                                strMaterialName, 
                                                pMaterialData,
                                                kTexturePathList,
                                                pMaterial,
                                                pMaterialInterface,
                                                strTextureFilesGamePath,
                                                strFbmTextureGamePath,
                                                strMaterialGamePath,
                                                strSubsurfaceProfilePath,
                                                strBoneType,
                                                strShaderType );
                    if( pInstUMaterialInterface )
                    {
                        FSkeletalMaterial* pInstFSMaterial = new FSkeletalMaterial( pInstUMaterialInterface,
                                                                                    true,
                                                                                    false,
                                                                                    *pMesh->Materials[ i ].MaterialSlotName.ToString(),
                                                                                    *pMesh->Materials[ i ].ImportedMaterialSlotName.ToString() );
                        pMesh->Materials.RemoveAt( i );
                        pMesh->Materials.Insert( *pInstFSMaterial, i );
                    }
                    pMesh->MarkPackageDirty();
                    pMesh->PostEditChange();
                }
            }
            if( nInvalidMaterialNum > 0 )
            {
#ifdef UE418
                for( int j = ( pMesh->GetSourceModel().Sections.Num() - nInvalidMaterialNum - 1 );
                     j < pMesh->GetSourceModel().Sections.Num(); ++j )
#else
                for( int j = ( pMesh->GetImportedModel()->LODModels[ 0 ].Sections.Num() - nInvalidMaterialNum - 1 );
                     j < pMesh->GetImportedModel()->LODModels[ 0 ].Sections.Num(); ++j )
#endif
                {
                    if( j < 0 )
                    {
                        j = 0;
                    }
#ifdef UE418
                    if( ( j + 1 ) < pMesh->GetSourceModel().Sections.Num() )
                    {
                        if( pMesh->GetSourceModel().Sections[ j + 1 ].MaterialIndex
                            <= pMesh->GetSourceModel().Sections[ j ].MaterialIndex )
                        {
                            if( ( pMesh->GetSourceModel().Sections[ j ].MaterialIndex + 1 ) < pMesh->Materials.Num() )
                            {
                                //showInfo( "An incorrect material slot setting was detected. Try to fix it. Please check the material slot after the CC Auto-Setup.", 7.f );
                                FText strMessage = FText::FromString( TEXT( "An incorrect material slot setting was detected. Try to fix it. Please check the material slot after the CC Auto-Setup." ) );
                                FMessageDialog::Open( EAppMsgType::Ok, strMessage );
                                pMesh->GetSourceModel().Sections[ j + 1 ].MaterialIndex = pMesh->GetSourceModel().Sections[ j ].MaterialIndex + 1;
                            }
                        }
                    }
#else
                    if( ( j + 1 ) < pMesh->GetImportedModel()->LODModels[ 0 ].Sections.Num() )
                    {
                        if( pMesh->GetImportedModel()->LODModels[ 0 ].Sections[ j + 1 ].MaterialIndex
                            <= pMesh->GetImportedModel()->LODModels[ 0 ].Sections[ j ].MaterialIndex )
                        {
                            if( ( pMesh->GetImportedModel()->LODModels[ 0 ].Sections[ j ].MaterialIndex + 1 ) < pMesh->Materials.Num() )
                            {
                                //showInfo( "An incorrect material slot setting was detected. Try to fix it. Please check the material slot after the CC Auto-Setup.", 7.f );
                                FText strMessage = FText::FromString( TEXT( "An incorrect material slot setting was detected. Try to fix it. Please check the material slot after the CC Setup." ) );
                                FMessageDialog::Open( EAppMsgType::Ok, strMessage );
                                pMesh->GetImportedModel()->LODModels[ 0 ].Sections[ j + 1 ].MaterialIndex = pMesh->GetImportedModel()->LODModels[ 0 ].Sections[ j ].MaterialIndex + 1;
                            }
                        }
                    }
#endif
                }
                pMesh->MarkPackageDirty();
                pMesh->PostEditChange();
            }
            if( pMesh && bIsDragFbx )
            {
                if( !IsRunningCommandlet() )
                {
                    FReimportManager::Instance()->Reimport( pMesh, false );
                    pMesh->MarkPackageDirty();
                    pMesh->PostEditChange();
                }
            }
            
            CreatePhysicSoftCloth( pPhysicsAsset, pMesh, strBoneType, kMaterialMap );

            if( kAssetData.AssetName.ToString().Contains( "_LOD0" ) )
            {
                UE_LOG( LogTemp, Warning, TEXT( "Old Process, No enhance" ) );
            }

            if( strBoneType != "NULL" )
            {
                if( !m_pEngineHumanoidRig )
                {
                    m_pEngineHumanoidRig = LoadObject<URig>( nullptr,
                                                             TEXT( "/Engine/EngineMeshes/Humanoid.Humanoid" ),
                                                             nullptr,
                                                             LOAD_None,
                                                             nullptr );

                }

                pSkeleton->SetRigConfig( m_pEngineHumanoidRig );
                pSkeleton->SetRigBoneMapping( "Custom_1", "cc_base_jawroot" );

                if ( CharacterGenerationBoneMap.Contains( strGeneration ) )
                {
                    pSkeleton->SetBoneTranslationRetargetingMode( 0, EBoneTranslationRetargetingMode::Skeleton, true );
                    pSkeleton->SetBoneTranslationRetargetingMode( 0, EBoneTranslationRetargetingMode::AnimationScaled );
                    pSkeleton->SetBoneTranslationRetargetingMode( 1, EBoneTranslationRetargetingMode::AnimationScaled );
                }
                else //NonStd
                {
                    pSkeleton->SetBoneTranslationRetargetingMode( 0, EBoneTranslationRetargetingMode::Animation, true );
                    pSkeleton->SetBoneTranslationRetargetingMode( 0, EBoneTranslationRetargetingMode::Animation );
                    pSkeleton->SetBoneTranslationRetargetingMode( 1, EBoneTranslationRetargetingMode::Animation );
                }

                FName kIKFootRootBone = "ik_foot_root";
                int nIKFootRootBoneID = pSkeleton->GetReferenceSkeleton().FindBoneIndex( kIKFootRootBone );
                if( nIKFootRootBoneID != -1 )
                {
                    pSkeleton->SetBoneTranslationRetargetingMode( nIKFootRootBoneID, EBoneTranslationRetargetingMode::Animation, true );
                }
                FName kIKHandRootBone = "ik_hand_root";
                int nIKHandRootBoneID = pSkeleton->GetReferenceSkeleton().FindBoneIndex( kIKHandRootBone );
                if( nIKHandRootBoneID != -1 )
                {
                    pSkeleton->SetBoneTranslationRetargetingMode( nIKHandRootBoneID, EBoneTranslationRetargetingMode::Animation, true );
                }

                pSkeleton->SetPreviewMesh( pMesh, true );
                pSkeleton->MarkPackageDirty();
                pSkeleton->PostEditChange();
            }
            ShowInfo( "Auto-Processing Complete", 7.f );
        }
        else if( kAssetData.GetClass() == UStaticMesh::StaticClass() )
        {
            ShowInfo( "Start Auto-Processing...", 7.f );
            UStaticMesh* pMesh = dynamic_cast< UStaticMesh* >( kAssetData.GetAsset() );
            if( !pMesh )
            {
                return;
            }
            FString strTargetFolderPath = strTexturePath + kAssetData.AssetName.ToString() + "/";
            MoveTextureFile( strTargetFolderPath );

            FString strSrcMaterialFolderPath = strRootPath;
            FString strTgtMaterialFolderPath = strRootPath + "Materials/";
            FString strFbxName = kAssetData.AssetName.ToString();
            DeleteDefaultStaticMaterial( strSrcMaterialFolderPath,
                                         strTgtMaterialFolderPath,
                                         strFbxName,
                                         strRootGamePath,
                                         pMesh,
                                         m_bIsMaterialInstance );
            CheckWorldGridMaterial( strFbxName, strRootGamePath, strMaterialGamePath, pMesh, m_bIsMaterialInstance );


            int nInvalidMaterialNum = 0;
            for( int j = 0; j < pMesh->MeshGetMaterial.Num(); ++j )
            {
                FString strSlowTask = "Auto-Processing : " + pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString();
                kSlowTask.EnterProgressFrame( 70 / pMesh->MeshGetMaterial.Num(), FText::FromString( strSlowTask ) );
                if( pMesh->MeshGetMaterial[ j ].MaterialInterface )
                {
                    UMaterial* pMaterial = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetMaterial();

                    FString strMaterialName = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetName();
                    RemoveMaterialPostfix( strMaterialName );

                    RLMaterialData* pMaterialData = kMaterialMap.Find( strMaterialName );
                    if ( !pMaterialData )
                    {
                        continue;
                    }
                    if ( pMaterialData->GetShaderData() == nullptr && strShaderType == "Standard" )
                    {
                        ConvertEyeMaterialName( strMaterialName );
                        pMaterialData = kMaterialMap.Find( strMaterialName );
                    }

                    //要重取一次，material有可能被上面func.改掉
                    pMaterial = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetMaterial();

                    UMaterialInstanceConstant* pInstUMaterialInterface = nullptr;
                    AssignMaterialInstanceJson( pInstUMaterialInterface,
                                                strMaterialName,
                                                pMaterialData,
                                                kTexturePathList,
                                                pMaterial,
                                                pMesh->MeshGetMaterial[ j ].MaterialInterface,
                                                strTextureFilesGamePath,
                                                strFbmTextureGamePath,
                                                strMaterialGamePath,
                                                strSubsurfaceProfilePath,
                                                strBoneType,
                                                strShaderType );
                    if( pInstUMaterialInterface )
                    {
                        FStaticMaterial* pFSMaterial = new FStaticMaterial( pInstUMaterialInterface,
                                                                            *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                                            *pMesh->MeshGetMaterial[ j ].ImportedMaterialSlotName.ToString() );
                        pMesh->MeshGetMaterial.RemoveAt( j );
                        pMesh->MeshGetMaterial.Insert( *pFSMaterial, j );
                        pMesh->MarkPackageDirty();
                        pMesh->PostEditChange();
                    }
                    if( pMesh->MeshGetMaterial[ j ].MaterialInterface->GetName() == "WorldGridMaterial" )
                    {
                        ++nInvalidMaterialNum;
                        FString strPackageName = strRootGamePath + "/" + pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString();
                        UPackage* pPackage = CreatePackage( NULL, *strPackageName );
                        UMaterialFactoryNew* pMaterialFactory = NewObject<UMaterialFactoryNew>();
                        UMaterial* pUnrealMaterial = ( UMaterial* )pMaterialFactory->FactoryCreateNew( UMaterial::StaticClass(),
                                                                                                       pPackage,
                                                                                                       *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                                                                       RF_Standalone | RF_Public,
                                                                                                       NULL,
                                                                                                       GWarn );
                        FAssetRegistryModule::AssetCreated( pUnrealMaterial );
                        UMaterialInterface* pMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strPackageName );
                        if( !pMaterialInterface )
                        {
                            UE_LOG( LogTemp, Warning, TEXT( "NO MaterialInterface" ) );
                        }
                        else
                        {
                            FStaticMaterial* pFSMaterial = new FStaticMaterial( pMaterialInterface,
                                                                                *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                                                *pMesh->MeshGetMaterial[ j ].ImportedMaterialSlotName.ToString() );
                            pMesh->MeshGetMaterial.RemoveAt( j );
                            pMesh->MeshGetMaterial.Insert( *pFSMaterial, j );
                            pMesh->MarkPackageDirty();
                            pMesh->PostEditChange();

                            //FString strMaterialName = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetName();
                            RemoveMaterialPostfix( strMaterialName );

                            //RLMaterialData* pMaterialData = kMaterialMap.Find( strMaterialName );
                            if ( !pMaterialData )
                            {
                                continue;
                            }
                            if ( pMaterialData->GetShaderData() == nullptr && strShaderType == "Standard" )
                            {
                                ConvertEyeMaterialName( strMaterialName );
                                pMaterialData = kMaterialMap.Find( strMaterialName );
                            }
                            //要重取一次，material有可能被上面func.改掉
                            pUnrealMaterial = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetMaterial();
                            pMaterialInterface = pMesh->MeshGetMaterial[ j ].MaterialInterface;

                            UMaterialInstanceConstant* pNewInstUMaterialInterface = nullptr;
                            AssignMaterialInstanceJson( pNewInstUMaterialInterface,
                                                        strMaterialName,
                                                        pMaterialData,
                                                        kTexturePathList,
                                                        pUnrealMaterial,
                                                        pMaterialInterface,
                                                        strTextureFilesGamePath,
                                                        strFbmTextureGamePath,
                                                        strMaterialGamePath,
                                                        strSubsurfaceProfilePath,
                                                        strBoneType,
                                                        strShaderType );
                            if( pNewInstUMaterialInterface )
                            {
                                pFSMaterial = new FStaticMaterial( pNewInstUMaterialInterface,
                                                                   *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                                   *pMesh->MeshGetMaterial[ j ].ImportedMaterialSlotName.ToString() );
                                pMesh->MeshGetMaterial.RemoveAt( j );
                                pMesh->MeshGetMaterial.Insert( *pFSMaterial, j );
                            }
                        }
                    }
                }
                else
                {
                    ++nInvalidMaterialNum;
                    FString strPackageName = strRootGamePath + "/Materials/" + pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString();
                    UPackage* pPackage = CreatePackage( NULL, *strPackageName );
                    UMaterialFactoryNew* pMaterialFactory = NewObject<UMaterialFactoryNew>();
                    UMaterial* pUnrealMaterial = ( UMaterial* )pMaterialFactory->FactoryCreateNew( UMaterial::StaticClass(),
                                                                                                   pPackage,
                                                                                                   *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                                                                   RF_Standalone | RF_Public,
                                                                                                   NULL,
                                                                                                   GWarn );
                    FAssetRegistryModule::AssetCreated( pUnrealMaterial );
                    UMaterialInterface* pMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strPackageName );
                    if( !pMaterialInterface )
                    {
                        UE_LOG( LogTemp, Warning, TEXT( "NO MaterialInterface" ) );
                    }
                    else
                    {
                        FStaticMaterial* pFSMaterial = new FStaticMaterial( pMaterialInterface,
                                                                            *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                                            *pMesh->MeshGetMaterial[ j ].ImportedMaterialSlotName.ToString() );
                        pMesh->MeshGetMaterial.RemoveAt( j );
                        pMesh->MeshGetMaterial.Insert( *pFSMaterial, j );
                        pMesh->MarkPackageDirty();
                        pMesh->PostEditChange();

                        FString strMaterialName = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetName();
                        RemoveMaterialPostfix( strMaterialName );

                        RLMaterialData* pMaterialData = kMaterialMap.Find( strMaterialName );
                        if ( !pMaterialData )
                        {
                            continue;
                        }
                        if ( pMaterialData->GetShaderData() == nullptr && strShaderType == "Standard" )
                        {
                            ConvertEyeMaterialName( strMaterialName );
                            pMaterialData = kMaterialMap.Find( strMaterialName );
                        }
                        //要重取一次，material有可能被上面func.改掉
                        pUnrealMaterial = pMesh->MeshGetMaterial[ j ].MaterialInterface->GetMaterial();
                        pMaterialInterface = pMesh->MeshGetMaterial[ j ].MaterialInterface;

                        UMaterialInstanceConstant* pInstUMaterialInterface = nullptr;
                        AssignMaterialInstanceJson( pInstUMaterialInterface,
                                                    strMaterialName,
                                                    pMaterialData,
                                                    kTexturePathList,
                                                    pUnrealMaterial,
                                                    pMaterialInterface,
                                                    strTextureFilesGamePath,
                                                    strFbmTextureGamePath,
                                                    strMaterialGamePath,
                                                    strSubsurfaceProfilePath,
                                                    strBoneType,
                                                    strShaderType );

                        if( pInstUMaterialInterface )
                        {
                            //UE_LOG(LogTemp, Warning, TEXT("InstUMaterialInterface"));
                            pFSMaterial = new FStaticMaterial( pInstUMaterialInterface,
                                                               *pMesh->MeshGetMaterial[ j ].MaterialSlotName.ToString(),
                                                               *pMesh->MeshGetMaterial[ j ].ImportedMaterialSlotName.ToString() );
                            pMesh->MeshGetMaterial.RemoveAt( j );
                            pMesh->MeshGetMaterial.Insert( *pFSMaterial, j );
                            pMesh->MarkPackageDirty();
                            pMesh->PostEditChange();
                        }
                    }
                }
            }
            ShowInfo( "Auto-Processing Complete", 7.f );
        }
        else
        {
            FText strMessage = FText::FromString( TEXT( "Please select a Skeletal Mesh or Static Mesh" ) );
            FMessageDialog::Open( EAppMsgType::Ok, strMessage );
        }
        FString fbmpath = strRootGamePath + "/" + kAssetData.AssetName.ToString() + "_fbm";
        DeleteTextureFile( strRootGamePath, fbmpath );
    }
}

bool FRLPluginModule::CheckAutoSetupVersionPass( FString strJsonFilePath )
{
    FString strJsonConfig;
    FFileHelper::LoadFileToString( strJsonConfig, *strJsonFilePath );
    TSharedPtr<FJsonObject> kJsonObject;
    TSharedRef<TJsonReader<>> kReader = TJsonReaderFactory<>::Create( strJsonConfig );
    FString strFbxName = FPaths::GetBaseFilename( strJsonFilePath );
    if( FJsonSerializer::Deserialize( kReader, kJsonObject ) )
    {
        FString strJsonVersion = "";
        strJsonVersion = kJsonObject->GetObjectField( strFbxName )->GetStringField( "Version" );
        if( !strJsonVersion.IsEmpty() )
        {
            int nPosition = 0;
            int nBigJsonVersion = -1;
            int nMidJsonVersion = -1;
            if( strJsonVersion == "1" )
            {
                nBigJsonVersion = 1;
                nMidJsonVersion = 0;
            }
            else
            {
                TArray< FString > kVersionList;
                strJsonVersion.ParseIntoArray( kVersionList, TEXT( "." ) );

                if( kVersionList.Num() == 4 )
                {
                    nBigJsonVersion = FCString::Atoi( *kVersionList[ 0 ] );
                    nMidJsonVersion = FCString::Atoi( *kVersionList[ 1 ] );
                }
            }

            auto fnShowMessage = [ & ]( const CHAR* strMessage )
            {
                if( !IsRunningCommandlet() )
                {
                    FText strTextMessage = FText::FromString( strMessage );
                    FMessageDialog::Open( EAppMsgType::Ok, strTextMessage );
                }
            };

            if( AUTOSETUP_BIG_VERSION != nBigJsonVersion )
            {
                fnShowMessage( Version_Error_Message );
                UE_LOG( LogTemp, Warning, TEXT( Version_Error_Message ) );
                return false;
            }

            if( AUTOSETUP_MID_VERSION < nMidJsonVersion )
            {
                fnShowMessage( Version_Not_Suitable );
                UE_LOG( LogTemp, Warning, TEXT( Version_Not_Suitable ) );
            }
        }
    }
    return true;
}

bool FRLPluginModule::CreateFolder( FString &strPath )
{
    IPlatformFile& kPlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if( kPlatformFile.DirectoryExists( *strPath ) )
    {
        return true;
    }
    return kPlatformFile.CreateDirectory( *strPath );
}

void FRLPluginModule::CreateTexturesPathList( const FString &strRootGamePath, TArray< FString >& kTexturesPathList )
{
    kTexturesPathList.Empty();
    FAssetRegistryModule& kAssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" );
    TArray<FAssetData> kAssetData;
    TArray<UObject*> kAssetObjectsInPath;
    FARFilter kFilter;
    kFilter.ClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    kFilter.PackagePaths.Add( *strRootGamePath );
    kFilter.bRecursivePaths = true;
    kAssetRegistryModule.Get().GetAssets( kFilter, kAssetData );
    if( kAssetData.Num() == 0 )
    {
        return;
    }

    for( int32 i = 0; i < kAssetData.Num(); ++i )
    {
        FString strAssetDataGamePath = kAssetData[ i ].ObjectPath.ToString();
        kTexturesPathList.Add( strAssetDataGamePath );
    }
}

FString FRLPluginModule::GetMaterialPackagePath( const FString &strRootGamePath, const FString& strMaterialName, const FString &strFbxName, bool bIsMaterialInstance )
{
    FString strPackagePath;
    if( bIsMaterialInstance )
    {
        strPackagePath = strRootGamePath + "/" + MATERIAL_FOLDER_NAME + "/" + strFbxName + "/" + strMaterialName + "_Inst";
    }
    else
    {
        strPackagePath = strRootGamePath + "/Materials/" + strMaterialName;
    }
    return strPackagePath;
}

void FRLPluginModule::ReplaceMaterial( UMaterialInterface* pMaterialInterface, const FString &strPackagePath, std::function< void( UMaterialInterface* ) > fnReplaceMaterial )
{
    if( pMaterialInterface->GetMaterial()->GetFName().ToString() == pMaterialInterface->GetFName().ToString() )
    {
        UMaterialInterface* pPackageMaterial = LoadObject<UMaterialInterface>( nullptr, *strPackagePath );
        if( pPackageMaterial )
        {
            fnReplaceMaterial( pPackageMaterial );
        }
        else
        {
            UE_LOG( LogTemp, Warning, TEXT( "NO MaterialInterface" ) );
        }
    }
}

void FRLPluginModule::DeleteUnrealMaterial( const FString &strSourceFolderPath, const FString &strTargetFolderPath, const FString &strFbxName, const FString &strRootGamePath, bool bIsMaterialInstance, std::function< void() > fnReplaceMeshMaterial )
{
    IPlatformFile& kPlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if( !bIsMaterialInstance && !kPlatformFile.DirectoryExists( *strTargetFolderPath ) )
    {
        kPlatformFile.CreateDirectory( *strTargetFolderPath );
    }

    FAssetRegistryModule& kAssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" );
    TArray<FAssetData> kAssetDatas;
    FARFilter Filter;
    Filter.ClassNames.Add( UMaterial::StaticClass()->GetFName() );
    Filter.PackagePaths.Add( *strRootGamePath );
    kAssetRegistryModule.Get().GetAssets( Filter, kAssetDatas );

    if( kAssetDatas.Num() == 0 )
    {
        return;
    }

    TArray< UObject* > kAssetObjectsInPath;
    kAssetObjectsInPath.Empty();
    TArray< FString > kDeleteFilePaths;
    kDeleteFilePaths.Empty();

    for( auto& kAssetData : kAssetDatas )
    {
        FString kMaterialName = kAssetData.AssetName.ToString();
        FString strTargetFilePath = strTargetFolderPath + kMaterialName + ".uasset";
        FString strSourceFilePath = strSourceFolderPath + kMaterialName + ".uasset";
        kDeleteFilePaths.Add( strSourceFilePath );

        UObject *kAsset = kAssetData.GetAsset();
        if( kAsset )
        {
            kAssetObjectsInPath.Add( kAsset );
            kAssetRegistryModule.Get().AssetDeleted( kAsset );
        }

        FString strPackagePath = GetMaterialPackagePath( strRootGamePath, kMaterialName, strFbxName, bIsMaterialInstance );
        if( !bIsMaterialInstance )
        {
            UPackage* Package = CreatePackage( NULL, *strPackagePath );
            auto MaterialFactory = NewObject<UMaterialFactoryNew>();
            UMaterial* UnrealMaterial = ( UMaterial* )MaterialFactory->FactoryCreateNew( UMaterial::StaticClass(), Package, *kMaterialName, RF_Standalone | RF_Public, NULL, GWarn );
            FAssetRegistryModule::AssetCreated( UnrealMaterial );
            Package->FullyLoad();
            Package->SetDirtyFlag( true );
            UnrealMaterial->PreEditChange( NULL );
            UnrealMaterial->PostEditChange();
        }
        else
        {
            UMaterialInterface* pStandardMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *StandardORMPackage );
            if( pStandardMaterialInterface )
            {
                UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
                Factory->InitialParent = pStandardMaterialInterface;

                UPackage* Package = CreatePackage( NULL, *strPackagePath );

                FString strMaterialInstanceName = kMaterialName + "_Inst";
                UMaterialInstanceConstant* pMaterialInstance = ( UMaterialInstanceConstant* )Factory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, FName( *strMaterialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
                FAssetRegistryModule::AssetCreated( pMaterialInstance );
            }
        }
    }

    fnReplaceMeshMaterial();

    if( !IsRunningCommandlet() )
    {
        ObjectTools::ForceDeleteObjects( kAssetObjectsInPath, false );
    }
    else
    {
        for( auto& strDeletePath : kDeleteFilePaths )
        {
            FString strFullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead( *strDeletePath );
            kPlatformFile.DeleteFile( *strFullPath );
        }
    }
}

void FRLPluginModule::DeleteDefaultSkeletalMaterial( const FString &strSourceFolderPath, const FString &strTargetFolderPath, const FString &strFbxName, const FString &strRootGamePath, USkeletalMesh *pMesh, bool bIsMaterialInstance )
{
    auto fnReplaceMeshSkeletalMaterial = [ & ]()
    {
        for( int i = 0; i < pMesh->Materials.Num(); ++i )
        {
            auto& kMaterial = pMesh->Materials[ i ];
            auto pMaterialInterface = kMaterial.MaterialInterface;
            if( !pMaterialInterface )
            {
                continue;
            }

            auto fnReplaceMaterial = [ & ]( UMaterialInterface* pMaterialInterface )
            {
                FSkeletalMaterial *FSMaterial = new FSkeletalMaterial( pMaterialInterface, true, false, *kMaterial.MaterialSlotName.ToString(), *kMaterial.ImportedMaterialSlotName.ToString() );
                pMesh->Materials.RemoveAt( i );
                pMesh->Materials.Insert( *FSMaterial, i );
                pMesh->MarkPackageDirty();
                pMesh->PostEditChange();
                pMaterialInterface->MarkPackageDirty();
                pMaterialInterface->PostEditChange();
            };

            auto strPackagePath = GetMaterialPackagePath( strRootGamePath, pMaterialInterface->GetFName().ToString(), strFbxName, bIsMaterialInstance );
            ReplaceMaterial( pMaterialInterface, strPackagePath, fnReplaceMaterial );
        }
    };

    DeleteUnrealMaterial( strSourceFolderPath, strTargetFolderPath, strFbxName, strRootGamePath, bIsMaterialInstance, fnReplaceMeshSkeletalMaterial );
}

void FRLPluginModule::DeleteDefaultStaticMaterial( const FString &strSourceFolderPath, const FString &strTargetFolderPath, const FString &strFbxName, const FString &strRootGamePath, UStaticMesh *pMesh, bool bIsMaterialInstance )
{
    auto fnReplaceMeshStaticMaterial = [ & ]()
    {
        for( int i = 0; i < pMesh->MeshGetMaterial.Num(); ++i )
        {
            auto& kMaterial = pMesh->MeshGetMaterial[ i ];
            auto pMaterialInterface = kMaterial.MaterialInterface;
            if( pMaterialInterface == NULL )
            {
                continue;
            }

            auto fnReplaceMaterial = [ & ]( UMaterialInterface* pMaterialInterface )
            {
                FStaticMaterial *FSMaterial = new FStaticMaterial( pMaterialInterface, *kMaterial.MaterialSlotName.ToString(), *kMaterial.ImportedMaterialSlotName.ToString() );
                pMesh->MeshGetMaterial.RemoveAt( i );
                pMesh->MeshGetMaterial.Insert( *FSMaterial, i );
                pMesh->MarkPackageDirty();
                pMesh->PostEditChange();
                pMaterialInterface->MarkPackageDirty();
                pMaterialInterface->PostEditChange();
            };

            auto strPackagePath = GetMaterialPackagePath( strRootGamePath, pMaterialInterface->GetFName().ToString(), strFbxName, bIsMaterialInstance );
            ReplaceMaterial( pMaterialInterface, strPackagePath, fnReplaceMaterial );
        }
    };

    DeleteUnrealMaterial( strSourceFolderPath, strTargetFolderPath, strFbxName, strRootGamePath, bIsMaterialInstance, fnReplaceMeshStaticMaterial );
}

void FRLPluginModule::MoveTextureFile( FString &strTargetFolderPath )
{
    TArray<FString> kFilePathList;
    kFilePathList.Empty();
    const TCHAR* strExtension = _T( "*.uasset" );
    FFileManagerGeneric::Get().FindFilesRecursive( kFilePathList, *strTargetFolderPath, strExtension, true, false );
    for( FString strFilePath : kFilePathList )
    {
        FString strTargetPath = strTargetFolderPath + FPaths::GetCleanFilename( strFilePath );
        if( strTargetPath != strFilePath )
        {
            FFileManagerGeneric::Get().Move( *strTargetPath, *strFilePath );
        }
    }
}

void FRLPluginModule::DeleteTextureFile( const FString& strDeleteFolderPath, const FString& strCheckFolderPath )
{
    FAssetRegistryModule& kAssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" );

    FARFilter kDeleteFilter;
    kDeleteFilter.ClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    kDeleteFilter.PackagePaths.Add( *strDeleteFolderPath );

    TArray<FAssetData> kDeleteAssetData;
    kAssetRegistryModule.Get().GetAssets( kDeleteFilter, kDeleteAssetData );

    FARFilter kCheckFilter;
    kCheckFilter.ClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    kCheckFilter.PackagePaths.Add( *strCheckFolderPath );

    TArray<FAssetData> kCheckAssetData;
    kAssetRegistryModule.Get().GetAssets( kCheckFilter, kCheckAssetData );

    TArray<UObject*> kAssetObjectsInPath;
    for( auto& kDeleteData : kDeleteAssetData )
    {
        auto fnDeleteAsset = [ & ]()
        {
            UObject* pAsset = kDeleteData.GetAsset();
            if( pAsset )
            {
                kAssetObjectsInPath.Add( pAsset );
                kAssetRegistryModule.Get().AssetDeleted( pAsset );
            }
        };

        for( auto& kCheckData : kCheckAssetData )
        {
            if( kDeleteData.AssetName.ToString() == kCheckData.AssetName.ToString() )
            {
                fnDeleteAsset();
                break;
            }
        }

        if( kDeleteData.AssetName.ToString().ToLower().Contains( "_specular" ) )
        {
            fnDeleteAsset();
        }
    }

    if( !IsRunningCommandlet() )
    {
        ObjectTools::ForceDeleteObjects( kAssetObjectsInPath, false );
    }
    else
    {
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        for( int i = 0; i < kAssetObjectsInPath.Num(); i++ )
        {
            FString assetGamePath = FPaths::GetPath( kAssetObjectsInPath[ i ]->GetPathName() );
            FString assetPath = assetGamePath + "/";
            assetPath.RemoveFromStart( TEXT( "/Game/" ) );
            assetPath = FPaths::ProjectContentDir() + assetPath + FPaths::GetBaseFilename( kAssetObjectsInPath[ i ]->GetPathName() ) + ".uasset";
            assetPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead( *assetPath );
            PlatformFile.DeleteFile( *assetPath );
        }
    }
}

void FRLPluginModule::CheckWorldGridMaterial( FString &strFbxName, FString &rootGamePath, FString ccMaterialFolderGamePath, UStaticMesh *pMesh, bool isMaterialInstance )
{

    for( int i = 0; i < pMesh->MeshGetMaterial.Num(); ++i )
    {
        //FString PackageName = rootGamePath + "/Materials/";
        FString materialName = RemoveInvalidChar( pMesh->MeshGetMaterial[ i ].MaterialSlotName.ToString() );
        if( pMesh->MeshGetMaterial[ i ].MaterialInterface->GetMaterial()->GetName() == "WorldGridMaterial" )
        {
            if( isMaterialInstance )
            {
                UMaterialInterface* standardMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *StandardORMPackage );

                if( standardMaterialInterface != nullptr )
                {
                    FString PackageName = ccMaterialFolderGamePath + "/" + materialName + "_Inst";
                    UMaterialInstanceConstant* materialInstance = nullptr;
                    FString materialInstancePath = ccMaterialFolderGamePath + "/" + materialName + "_Inst." + materialName + "_Inst";
                    materialInstance = Cast<UMaterialInstanceConstant>( StaticLoadObject( UMaterialInstanceConstant::StaticClass(), NULL, *( materialInstancePath ) ) );

                    if( materialInstance == NULL )
                    {
                        FString materialInstanceName = materialName + "_Inst";
                        UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
                        Factory->InitialParent = standardMaterialInterface;
                        UPackage* Package = CreatePackage( NULL, *PackageName );
                        materialInstance = ( UMaterialInstanceConstant* )Factory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, FName( *materialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
                        FAssetRegistryModule::AssetCreated( materialInstance );
                    }
                    if( materialInstance )
                    {
                        FStaticMaterial* pFSMaterial = new FStaticMaterial( materialInstance,
                                                                            *pMesh->MeshGetMaterial[ i ].MaterialSlotName.ToString(),
                                                                            *pMesh->MeshGetMaterial[ i ].ImportedMaterialSlotName.ToString() );
                        pMesh->MeshGetMaterial.RemoveAt( i );
                        pMesh->MeshGetMaterial.Insert( *pFSMaterial, i );
                        pMesh->MarkPackageDirty();
                        pMesh->PostEditChange();
                    }
                }
            }
        }
    }
}

bool IsHairOrBrow( EShaderType eShaderType, ENodeType eNodeType )
{
    bool bIsBrow = ( eShaderType == EShaderType::Hair && eNodeType == ENodeType::Brow );
    bool bIsHair = ( eShaderType == EShaderType::Hair && eNodeType == ENodeType::Hair );

    return ( bIsHair || bIsBrow );
}

bool FRLPluginModule::CheckShaderTypeChange( FString &strFbxName, FString strShaderType, USkeletalMesh *pMesh, UMaterial* pMaterial, UMaterialInterface* MaterialInterface, RLMaterialData* pMaterialData, int nSlotID, FString &strRootGamePath, bool bIsMaterialInstance, FString strBoneType )
{
    bool bChangeType = false;
    if( !MaterialInterface )
    {
        return bChangeType;
    }

    TMap< EShaderType, FString > kPackagePreset{ { EShaderType::Teeth, GumsTongueTeethPackageName },
                                                 { EShaderType::Eyelash, EyelashPackageName },
                                                 { EShaderType::Eye, EyePackageName },
                                                 { EShaderType::Tearline, TearPackageName },
                                                 { EShaderType::Occulsion, EyeOcclusionPackageName },
                                                 { EShaderType::Hair, HairPackageName },
                                                 { EShaderType::GeneralSSS, GeneralSSSPackage }
    };

    TMap< ENodeType, FString > kNodePreset{ { ENodeType::Hair, HairPackageName },
                                            { ENodeType::Brow, HairPackageName },
    };

    UMaterialInstanceConstant* pMaterialInstance = nullptr;
    FString strMaterialName = MaterialInterface->GetName();
    RemoveMaterialPostfix( strMaterialName );

    auto eShaderType = GetShaderType( pMaterialData, strMaterialName.ToLower(), strBoneType );

    FString strOldShaderType;
    if( eShaderType == EShaderType::Teeth ||
        kNodePreset.Contains( pMaterialData->m_eNodeType ) &&
        eShaderType != EShaderType::Scalp ||
        eShaderType == EShaderType::Eyelash ||
        eShaderType == EShaderType::Eye ||
        eShaderType == EShaderType::Head ||
        eShaderType == EShaderType::Skin ||
        eShaderType == EShaderType::Tearline ||
        eShaderType == EShaderType::Occulsion ||
        eShaderType == EShaderType::Hair ||
        eShaderType == EShaderType::GeneralSSS )
    {
        if( pMaterial->GetPathName().Contains( "CC_Shaders" ) && !pMaterial->GetPathName().Contains( "StandardShader" ) )
        {
            strOldShaderType = "CCShader";
            if( pMaterial->GetPathName().Contains( "LWHQ" ) )
            {
                strOldShaderType = "CCShader";
            }
            else if( pMaterial->GetPathName().Contains( "HQ" ) )
            {
                strOldShaderType = "HQ";
            }
            else if( pMaterial->GetPathName().Contains( "LW" ) )
            {
                strOldShaderType = "LW";
            }
        }
        else
        {
            strOldShaderType = "Standard";
        }

        if( strOldShaderType == "CCShader" )
        {
            if( strShaderType == "Standard" )
            {
                bChangeType = true;
            }
            else
            {
                return bChangeType;
            }
        }
        else
        {
            if( strShaderType == strOldShaderType )
            {
                return bChangeType;
            }
            else
            {
                bChangeType = true;
            }
        }

        if( bChangeType && strShaderType != "Standard" )
        {
            UMaterialInterface* pCCShaderMaterialInterface = nullptr;
            auto it = kPackagePreset.Find( eShaderType );
            
            if ( it )
            {
                FString strShaderPackageName = *( *it );
                if ( IsHairOrBrow( eShaderType, pMaterialData->m_eNodeType ) )
                {
                    strShaderPackageName = HairSpecularPackageName;
                }

                pCCShaderMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strShaderPackageName );
                if ( eShaderType != EShaderType::TRA && eShaderType != EShaderType::PBR )
                {
                    strMaterialName = strMaterialName + "_LWHQ";
                }
            }
            else
            {
                it = kNodePreset.Find( pMaterialData->m_eNodeType );
                if ( it )
                {
                    FString strShaderPackageName = *( *it );
                    if ( IsHairOrBrow( eShaderType, pMaterialData->m_eNodeType ) )
                    {
                        strShaderPackageName = HairSpecularPackageName;
                    }
                    pCCShaderMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strShaderPackageName );
                    if ( eShaderType != EShaderType::TRA && eShaderType != EShaderType::PBR )
                    {
                        strMaterialName = strMaterialName + "_LWHQ";
                    }
                }
            }

            if( !pCCShaderMaterialInterface && ( eShaderType == EShaderType::Skin || eShaderType == EShaderType::Head ) )
            {
                if( strShaderType == "HQ" )
                {
                    pCCShaderMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *HQSkinPackageName );
                    strMaterialName = strMaterialName + "_HQ";
                }
                else if( strShaderType == "LW" )
                {
                    pCCShaderMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *LWSkinPackageName );
                    strMaterialName = strMaterialName + "_LW";
                }
            }

            if( pCCShaderMaterialInterface )
            {
                UMaterialInstanceConstantFactoryNew* pFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
                pFactory->InitialParent = pCCShaderMaterialInterface;

                FString strPackageName = GetMaterialPackagePath( strRootGamePath, strMaterialName, strFbxName, true );
                UPackage* pPackage = CreatePackage( NULL, *strPackageName );

                FString strMaterialInstanceName = strMaterialName + "_Inst";
                pMaterialInstance = ( UMaterialInstanceConstant* )pFactory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), pPackage, FName( *strMaterialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
                FAssetRegistryModule::AssetCreated( pMaterialInstance );
                pMaterialInstance->MarkPackageDirty();
                pMaterialInstance->PostEditChange();
            }

            if( pMaterialInstance )
            {
                FSkeletalMaterial *InstFSMaterial = new FSkeletalMaterial( pMaterialInstance, true, false, *pMesh->Materials[ nSlotID ].MaterialSlotName.ToString(), *pMesh->Materials[ nSlotID ].ImportedMaterialSlotName.ToString() );
                pMesh->Materials.RemoveAt( nSlotID );
                pMesh->Materials.Insert( *InstFSMaterial, nSlotID );
            }
        }
        else if( bChangeType && strShaderType == "Standard" )
        {
            FString strPackageName = GetMaterialPackagePath( strRootGamePath, strMaterialName, strFbxName, bIsMaterialInstance );
            if( pMesh->Materials[ nSlotID ].MaterialInterface )
            {
                UMaterialInterface* pStandardMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strPackageName );
                if( pStandardMaterialInterface )
                {
                    FSkeletalMaterial *FSMaterial = new FSkeletalMaterial( pStandardMaterialInterface, true, false, *strMaterialName, *strMaterialName );
                    pMesh->Materials.RemoveAt( nSlotID );
                    pMesh->Materials.Insert( *FSMaterial, nSlotID );
                    pMesh->MarkPackageDirty();
                    pMesh->PostEditChange();
                    pStandardMaterialInterface->MarkPackageDirty();
                    pStandardMaterialInterface->PostEditChange();
                }
            }
        }
    }
    else
    {
        bChangeType = false;
    }

    return bChangeType;
}

void FRLPluginModule::ShowInfo( const FString& strMessage, const float& fExpireDuration )
{
    if( IsRunningCommandlet() )
    {
        return;
    }

    FNotificationInfo kInfo( FText::FromString( strMessage ) );
    kInfo.FadeInDuration = 0.1f;
    kInfo.FadeOutDuration = 0.5f;
    kInfo.ExpireDuration = fExpireDuration;
    kInfo.bUseThrobber = false;
    kInfo.bUseLargeFont = true;
    kInfo.bFireAndForget = false;
    kInfo.bAllowThrottleWhenFrameRateIsLow = false;

    auto kNotificationItem = FSlateNotificationManager::Get().AddNotification( kInfo );
    kNotificationItem->ExpireAndFadeout();
}

FString FRLPluginModule::GetBoneType( const FAssetData& kAssetData )
{
    FString strBoneType = "NULL";
    bool bIsMulti = false;
    USkeleton* pSkeleton = GetAssetSkeleton( kAssetData );
    if( !pSkeleton )
    {
        return strBoneType;
    }

    USkeletalMesh *pMesh = dynamic_cast< USkeletalMesh* >( kAssetData.GetAsset() );
    if( !pMesh )
    {
        return strBoneType;
    }
    else
    {
        for( auto& kMaterial : pMesh->Materials )
        {
            if( kMaterial.MaterialSlotName.ToString().Contains( HeadKeyWord ) )
            {
                bIsMulti = true;
                break;
            }
        }
    }

    FName G1Bone = "cc_base_r_toe00";
    if( pSkeleton->GetReferenceSkeleton().FindBoneIndex( G1Bone ) != -1 )
    {
        strBoneType = BONE_TYPE_G1;
    }
    else
    {
        FName G3Bone = "cc_base_pelvis";
        if( pSkeleton->GetReferenceSkeleton().FindBoneIndex( G3Bone ) != -1 )
        {
            strBoneType = BONE_TYPE_G3;
            for( auto& kMaterial : pMesh->Materials )
            {
                if( kMaterial.MaterialSlotName.ToString().ToLower().Contains( TearKeyWord ) || kMaterial.MaterialSlotName.ToString().ToLower().Contains( EyeOcclusionKeyWord ) )
                {
                    strBoneType = BONE_TYPE_G3PLUS;  //未來應改成讀檔判
                    break;
                }
            }
        }
        else
        {
            FName GameBone = "cc_base_facialbone";
            if( pSkeleton->GetReferenceSkeleton().FindBoneIndex( GameBone ) != -1 )
            {
                if( bIsMulti )
                {
                    strBoneType = BONE_TYPE_GAME_MULTI;
                }
                else
                {
                    strBoneType = BONE_TYPE_GAME_SINGLE;
                }
            }
            else
            {
                strBoneType = "NULL";
            }
        }
    }
    return strBoneType;
}

void FRLPluginModule::findLODGroupMaterialJson( TMap< FString, RLMaterialData > &kMaterialMap, FString &strFbxName, FString &strRootGamePath, USkeletalMesh *kMesh, bool bIsMaterialInstance )
{
    if( !bIsMaterialInstance || kMaterialMap.Num() == 0 )
    {
        return;
    }

    //Lod資料為空
    if ( kMesh->GetLODNum() == 1 && kMesh->GetLODInfo( 0 )->LODMaterialMap.Num() == 0 )
    {
        return;
    }

    TArray<FString> LODMaterialList;
    for ( int i = 1; i <= 5; i++ )
    {
        for( auto& Elem : kMaterialMap )
        {
            FString strNum = "LOD" + FString::FromInt( i );
            if( Elem.Key.Contains( strNum ) )
            {
                LODMaterialList.Add( Elem.Key );
            }
        }
    }

    int nOffset = 0;
    //用來判斷export fbx有沒有keep origin avatar
    if( kMesh->GetLODNum() == LODMaterialList.Num() )
    {
        nOffset = -1;
    }

    for( int i = 0; i < LODMaterialList.Num(); i++ )
    {
        int nLodIndex = i + 1 + nOffset;
#if defined UE418 || defined UE419
        if( kMesh->LODInfo[ nLodIndex ].LODMaterialMap.Num() == 1 )
#else
        if( kMesh->GetLODInfo( nLodIndex )->LODMaterialMap.Num() == 1 )
#endif
        {
            FString PackageName = strRootGamePath + "/" + MATERIAL_FOLDER_NAME + "/" + strFbxName + "/" + LODMaterialList[ i ] + "_Inst";

            UMaterialInterface* LODMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *PackageName );

            if( !LODMaterialInterface )
            {
                UMaterialInterface* standardMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *StandardORMPackage );
                if( standardMaterialInterface )
                {
                    UMaterialInstanceConstant* materialInstance = nullptr;
                    FString materialInstanceName = LODMaterialList[ i ] + "_Inst";
                    UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
                    Factory->InitialParent = standardMaterialInterface;
                    UPackage* Package = CreatePackage( NULL, *PackageName );
                    materialInstance = ( UMaterialInstanceConstant* )Factory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, FName( *materialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
                    FAssetRegistryModule::AssetCreated( materialInstance );
                }
                LODMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *PackageName );
            }

            if( LODMaterialInterface )
            {
                int numberOfMaterial = -1;
                for( int y = kMesh->Materials.Num() - 1; y >= 0; y-- )
                {
                    if( kMesh->Materials[ y ].MaterialSlotName.ToString().Contains( LODMaterialList[ i ] ) )
                    {
                        numberOfMaterial = y;
                        break;
                    }
                }
                if( numberOfMaterial == -1 )
                {
                    FSkeletalMaterial *LODFSMaterial = new FSkeletalMaterial( LODMaterialInterface, true, false, *LODMaterialList[ i ], *LODMaterialList[ i ] );
                    kMesh->Materials.Add( *LODFSMaterial );
#if defined UE418 || defined UE419
                    kMesh->LODInfo[ nLodIndex ].LODMaterialMap[ 0 ] = ( kMesh->Materials.Num() - 1 );
#else
                    kMesh->GetLODInfo( nLodIndex )->LODMaterialMap[ 0 ] = ( kMesh->Materials.Num() - 1 );
#endif
                }
                else
                {
#if defined UE418 || defined UE419
                    kMesh->LODInfo[ nLodIndex ].LODMaterialMap[ 0 ] = numberOfMaterial;
#else
                    kMesh->GetLODInfo( nLodIndex )->LODMaterialMap[ 0 ] = numberOfMaterial;
#endif				
                }
            }
        }
    }
}
void FRLPluginModule::findLODGroupMaterial( FString &rootGamePath, USkeletalMesh *mesh )
{
    TArray<FString> LODMaterialList;
    LODMaterialList.Empty();
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" );
    TArray<FAssetData> AssetData;
    FARFilter Filter;
    Filter.ClassNames.Add( UMaterial::StaticClass()->GetFName() );
    Filter.PackagePaths.Add( *rootGamePath );
    AssetRegistryModule.Get().GetAssets( Filter, AssetData );

    for( int32 i = 0; i < AssetData.Num(); i++ )
    {
        if( AssetData[ i ].AssetName.ToString().Contains( "LOD" ) )
        {
            if( i == 0 && AssetData[ 0 ].AssetName.ToString().Contains( "LOD1" ) )
            {

            }
            else
            {
                LODMaterialList.Add( AssetData[ i ].ObjectPath.ToString() );
            }
        }
    }
    LODMaterialList.Sort();
    for( int i = 0; i < LODMaterialList.Num(); i++ )
    {
#if defined UE418 || defined UE419
        if( mesh->LODInfo[ i + 1 ].LODMaterialMap.Num() == 1 )
#else
        if( mesh->GetLODInfo( i + 1 )->LODMaterialMap.Num() == 1 )
#endif
        {
            UMaterialInterface* LODMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *LODMaterialList[ i ] );
            if( LODMaterialInterface != nullptr )
            {
                int numberOfMaterial = -1;
                for( int y = mesh->Materials.Num() - 1; y >= 0; y-- )
                {
                    if( mesh->Materials[ y ].MaterialSlotName.ToString().Contains( FPaths::GetBaseFilename( LODMaterialList[ i ] ) ) )
                    {
                        numberOfMaterial = y;
                        break;
                    }
                }
                if( numberOfMaterial == -1 )
                {
                    FSkeletalMaterial *LODFSMaterial = new FSkeletalMaterial( LODMaterialInterface, true, false, *FPaths::GetBaseFilename( LODMaterialList[ i ] ), *FPaths::GetBaseFilename( LODMaterialList[ i ] ) );
                    mesh->Materials.Add( *LODFSMaterial );
#if defined UE418 || defined UE419
                    mesh->LODInfo[ i + 1 ].LODMaterialMap[ 0 ] = ( mesh->Materials.Num() - 1 );
#else
                    mesh->GetLODInfo( i + 1 )->LODMaterialMap[ 0 ] = ( mesh->Materials.Num() - 1 );
#endif
                }
                else
                {
#if defined UE418 || defined UE419
                    mesh->LODInfo[ i + 1 ].LODMaterialMap[ 0 ] = numberOfMaterial;
#else
                    mesh->GetLODInfo( i + 1 )->LODMaterialMap[ 0 ] = numberOfMaterial;
#endif				
                }
            }
        }
    }
}

void FRLPluginModule::AddMenuExtension( FMenuBuilder& kBuilder )
{
    kBuilder.AddMenuEntry( FRLPluginCommands::Get().PluginAction );
}

void FRLPluginModule::AddToolbarExtension( FToolBarBuilder& kBuilder )
{
    kBuilder.AddToolBarButton( FRLPluginCommands::Get().PluginAction );
}

void FRLPluginModule::PhysicIniPaser( FString iniPath )
{
    iniPath = "C:/Users/AllenTang/Desktop/base.ini";
    FString Shape_Names = "";
    GConfig->GetString( TEXT( "CC_Base_R_Forearm" ), TEXT( "Shape_Names" ), Shape_Names, iniPath );
    UE_LOG( LogTemp, Warning, TEXT( "Shape_Names = %s" ), *Shape_Names );
}

void FRLPluginModule::SetOrmTextureSetting( UMaterialInstanceConstant* pMaterialInstance, const FString& strTexturePath, const bool bSkin )
{
    UTexture2D* pOrmTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *strTexturePath ) );
    if( pOrmTexture->SRGB )
    {
        pOrmTexture->SRGB = false;
        if( bSkin )
        {
            pOrmTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
        }
        pOrmTexture->MarkPackageDirty();
        pOrmTexture->PostEditChange();
    }
    pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "ORM Map" ) ), pOrmTexture );
}

void FRLPluginModule::ProcessMaterialInstanceConstant( UMaterialInstanceConstant*& pMaterialInstance, UMaterial* pMaterial, UMaterialInterface* pMeshMaterialInterface, UMaterialInterface* pParentInterface, const FString& strCCMaterialFolderPath, std::function< void( UMaterialInstanceConstant* ) > fnMaterialSetting )
{
    FString strPackageName = strCCMaterialFolderPath + "/" + pMaterial->GetName() + "_Inst";
    FString strPackageNamePath = strPackageName;
    strPackageNamePath.RemoveFromStart( TEXT( "/Game/" ) );
    strPackageNamePath = FPaths::ProjectContentDir() + strPackageNamePath + ".uasset";

    if( FPaths::GetPath( pMaterial->GetPathName() ).Contains( "CC_Shaders" ) )
    {
        FString strMaterialInstancePath = pMeshMaterialInterface->GetPathName();
        pMaterialInstance = Cast<UMaterialInstanceConstant>( StaticLoadObject( UMaterialInstanceConstant::StaticClass(), NULL, *( strMaterialInstancePath ) ) );
    }
    else if( !FPaths::FileExists( strPackageNamePath ) )
    {
        UPackage* Package = CreatePackage( NULL, *strPackageName );

        UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
        Factory->InitialParent = pParentInterface;

        FString strMaterialName = pMeshMaterialInterface->GetName();
        RemoveMaterialPostfix( strMaterialName );

        FString strMaterialInstanceName = strMaterialName + "_LWHQ_Inst";
        pMaterialInstance = ( UMaterialInstanceConstant* )Factory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, FName( *strMaterialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
        FAssetRegistryModule::AssetCreated( pMaterialInstance );
    }

    fnMaterialSetting( pMaterialInstance );

    pMaterialInstance->MarkPackageDirty();
    pMaterialInstance->PostEditChange();
}

void FRLPluginModule::AssignMaterialInstanceJson( UMaterialInstanceConstant*& pInstUMaterialInterface,
                                                  const FString& strMaterialName,
                                                  RLMaterialData* pMaterialData,
                                                  TArray <FString> & kTexturesPathList,
                                                  UMaterial* pMaterial,
                                                  UMaterialInterface* pMaterialInterface,
                                                  FString texturesFilesGamePath,
                                                  FString strTexturesFilesGamePathFbm,
                                                  FString strCCMaterialFolderGamePath,
                                                  const FString& strSubsurfaceProfilePath,
                                                  FString strBoneType,
                                                  FString shaderType )
{
    if ( !m_bIsMaterialInstance )
    {
        FMessageDialog::Open( EAppMsgType::Ok, FText::FromString( TEXT( "Miss RLShader Data" ) ) );
        return;
    }

    FString strTexturesFilesGamePathsFbm[ 2 ] ={ strTexturesFilesGamePathFbm, texturesFilesGamePath };
    FString texturesFilesGamePaths[ 2 ] ={ texturesFilesGamePath, strTexturesFilesGamePathFbm };

    FString strTexturePathToLoad = "";

    bool bIsStandardSkin = shaderType == "Standard";
    bool bIsHQSkin = shaderType == "HQ";
    bool bIsLWSkin = shaderType == "LW";
    bool bIsHair = pMaterialData->m_eNodeType == ENodeType::Hair;
    EShaderType eShaderType = GetShaderType( pMaterialData, pMaterialInterface->GetName().ToLower(), strBoneType );
    if ( !bIsStandardSkin && ( bIsLWSkin || bIsHQSkin ) )
    {
        if ( eShaderType == EShaderType::Teeth )
        {
            UMaterialInterface* pGumsTongueTeethMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *GumsTongueTeethPackageName );
            if ( pGumsTongueTeethMaterialInterface )
            {
                auto fnTougueMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
                {
                    if ( pMaterialInterface->GetName().ToLower().Contains( UpperTeethKeyWord ) )
                    {
                        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Is Upper Teeth" ) ), 1.0f );
                    }

                    SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetNormal( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                    SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );

                    if ( bIsHQSkin )
                    {
                        SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );

                        const RLScatter* pScatter = pMaterialData->GetScatter();
                        SetScatter( pScatter, pMaterialInstance, strMaterialName, strSubsurfaceProfilePath, eShaderType );
                    }

                    strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
                    if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
                    {
                        SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad );
                        SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                        SetTextureParameter( pMaterialData, "Metallic", pMaterialInstance );
                    }
                    else
                    {
                        SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                        SetMetallic( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                        UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
                    }

                    if ( pMaterialInterface->GetName().ToLower().Contains( TougueKeyWord ) )
                    {
                        UpdateStaticParameter( pMaterialInstance, "Is Tongue", true, true );
                        strTexturePathToLoad = "/Game/CC_Shaders/SkinShader/Textures/Share/Black_sRGB.Black_sRGB";
                        UTexture2D* pBlackTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
                        if ( pBlackTexture )
                        {
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Teeth Mask Map" ) ), pBlackTexture );
                        }
                    }
                };

                ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pGumsTongueTeethMaterialInterface, strCCMaterialFolderGamePath, fnTougueMaterialSetting );
            }
        }
        else if ( ( bIsHair && eShaderType == EShaderType::PBR ) ||
                  eShaderType == EShaderType::Hair )
        {
            
            FString strHairPackagePath = IsHairOrBrow( eShaderType, pMaterialData->m_eNodeType ) ? HairSpecularPackageName : HairPackageName;
            UMaterialInterface* pHairMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strHairPackagePath );
            if ( pHairMaterialInterface )
            {
                auto fnHairMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
                {
                    pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), pMaterialData->m_fOpacity );

                    SetGlow( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetBlendToHairDepthMap( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetNormal( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                    SetSpecular( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName, true );
                    SetOpacity( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                    SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );

                    if ( bIsHQSkin )
                    {
                        SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );
                        SetOpacityAdv( pMaterialData, pMaterialInstance );
                    }

                    strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
                    if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
                    {
                        SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad );
                        SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                        SetTextureParameter( pMaterialData, "Roughness", pMaterialInstance );
                    }
                    else
                    {
                        SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                        SetRoughness( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                        UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
                    }
                    SetMultiUvIndex( pMaterialData, pMaterialInstance );
                };

                ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pHairMaterialInterface, strCCMaterialFolderGamePath, fnHairMaterialSetting );
            }
            else
            {
                pMaterial->BlendMode = EBlendMode::BLEND_Masked;
                pMaterial->TwoSided = true;
            }
        }
        else if ( eShaderType == EShaderType::Eyelash )
        {
            UMaterialInterface* pEyelashMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *EyelashPackageName );
            if ( pEyelashMaterialInterface )
            {
                auto fnEyelashMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
                {
                    pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), pMaterialData->m_fOpacity );

                    SetGlow( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetNormal( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                    SetSpecular( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName, true );
                    SetOpacity( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                    SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );

                    if ( bIsHQSkin )
                    {
                        SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );
                    }

                    strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
                    if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
                    {
                        SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad );
                        SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                        SetTextureParameter( pMaterialData, "Roughness", pMaterialInstance );
                        SetTextureParameter( pMaterialData, "Metallic", pMaterialInstance );
                    }
                    else
                    {
                        SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                        SetRoughness( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                        SetMetallic( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                        UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
                    }
                };

                ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pEyelashMaterialInterface, strCCMaterialFolderGamePath, fnEyelashMaterialSetting );
            }
            else
            {
                pMaterial->BlendMode = EBlendMode::BLEND_Translucent;
                pMaterial->TwoSided = true;
            }
        }
        else if ( eShaderType == EShaderType::Eye )
        {
            UMaterialInterface* pEyeMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *EyePackageName );
            if ( pEyeMaterialInterface )
            {
                auto fnEyeMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
                {
                    if ( pMaterialInterface->GetName().ToLower().Contains( "std_cornea_r" ) )
                    {
                        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Is Left Eye" ) ), 0.0f );
                    }
                    pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Limbus UV Width Shading" ) ), 0.2f );

                    SetGlow( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetSpecular( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName, true );
                    SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );

                    if ( bIsHQSkin )
                    {
                        SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );

                        const RLScatter*pScatter = pMaterialData->GetScatter();
                        SetScatter( pScatter, pMaterialInstance, strMaterialName, strSubsurfaceProfilePath, eShaderType );
                    }

                    strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
                    if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
                    {
                        SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad );
                        SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                        SetTextureParameter( pMaterialData, "Metallic", pMaterialInstance );
                    }
                    else
                    {
                        SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                        SetMetallic( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                        UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
                    }
                };

                ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pEyeMaterialInterface, strCCMaterialFolderGamePath, fnEyeMaterialSetting );
            }
        }
        else if ( eShaderType == EShaderType::Tearline )
        {
            UMaterialInterface* pTearMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *TearPackageName );
            if ( pTearMaterialInterface )
            {
                auto fnTearMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
                {
                    if ( bIsHQSkin )
                    {
                        SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );
                    }
                };
                ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pTearMaterialInterface, strCCMaterialFolderGamePath, fnTearMaterialSetting );
            }
        }
        else if ( eShaderType == EShaderType::Occulsion )
        {
            UMaterialInterface* pEyeOcclusionMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *EyeOcclusionPackageName );
            if ( pEyeOcclusionMaterialInterface )
            {
                auto fnEyeOcclusionMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
                {
                    if ( bIsHQSkin )
                    {
                        SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );
                    }
                };
                ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pEyeOcclusionMaterialInterface, strCCMaterialFolderGamePath, fnEyeOcclusionMaterialSetting );
            }
        }
        else if ( eShaderType == EShaderType::Skin || eShaderType == EShaderType::Head )
        {
            FString strSkinPackageName = HQSkinPackageName;
            if ( bIsLWSkin )
            {
                strSkinPackageName = LWSkinPackageName;
            }

            UMaterialInterface* pSkinMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strSkinPackageName );
            if ( pSkinMaterialInterface )
            {
                FString strPackageName = strCCMaterialFolderGamePath + "/" + pMaterial->GetName() + "_Inst";
                FString strPackageNamePath = strPackageName;
                strPackageNamePath.RemoveFromStart( TEXT( "/Game/" ) );
                strPackageNamePath = FPaths::ProjectContentDir() + strPackageNamePath + ".uasset";

                UMaterialInstanceConstant* pMaterialInstance = nullptr;
                if ( FPaths::GetPath( pMaterial->GetPathName() ).Contains( "CC_Shaders" ) )
                {
                    FString strMaterialInstancePath = pMaterialInterface->GetPathName();
                    pMaterialInstance = Cast<UMaterialInstanceConstant>( StaticLoadObject( UMaterialInstanceConstant::StaticClass(), NULL, *( strMaterialInstancePath ) ) );
                }
                else if ( !FPaths::FileExists( strPackageNamePath ) )
                {
                    FString strMaterialInstanceName = "";
                    if ( bIsHQSkin )
                    {
                        strMaterialInstanceName = strMaterialName + "_HQ_Inst";
                    }
                    else if ( bIsLWSkin )
                    {
                        strMaterialInstanceName = strMaterialName + "_LW_Inst";
                    }
                    UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
                    Factory->InitialParent = pSkinMaterialInterface;
                    UPackage* Package = CreatePackage( NULL, *strPackageName );
                    pMaterialInstance = ( UMaterialInstanceConstant* )Factory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, FName( *strMaterialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
                    FAssetRegistryModule::AssetCreated( pMaterialInstance );
                }

                SetNormal( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                SetGlow( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                SetSpecular( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName, true );
                SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
                SetDisplacement( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );

                strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
                if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
                {
                    SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad, true );
                    SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                    SetTextureParameter( pMaterialData, "Roughness", pMaterialInstance );
                    SetTextureParameter( pMaterialData, "Metallic", pMaterialInstance );
                }
                else
                {
                    SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                    SetRoughness( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                    SetMetallic( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, true );
                    UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
                }

                auto strCCTexturePath = "/Game/CC_Shaders/SkinShader/Textures/" + strBoneType + "/";
                auto fnSetTextureParameter = [ &pMaterialInstance, &strCCTexturePath ]( const FString strMaterialName, const FName strParameterName )
                {
                    FString strTexturePath = strCCTexturePath + strMaterialName + "." + strMaterialName;
                    UTexture2D* pTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *strTexturePath ) );
                    if ( pTexture )
                    {
                        pMaterialInstance->SetTextureParameterValueEditorOnly( strParameterName, pTexture );
                    }
                };

                if ( bIsLWSkin )
                {
                    if ( eShaderType == EShaderType::Head || strBoneType == "GameBase_Single" )
                    {
                        if ( strBoneType == "GameBase_Single" )
                        {
                            fnSetTextureParameter( "Body_Mask_SSS_Cavity", UTF8_TO_TCHAR( "SSS Mask" ) );
                        }
                        else
                        {
                            fnSetTextureParameter( "Head_Mask_SSS_Cavity", UTF8_TO_TCHAR( "SSS Mask" ) );
                            fnSetTextureParameter( "Head_Tessellation_Mask", UTF8_TO_TCHAR( "Tessellation Mask Map" ) );
                        }
                        fnSetTextureParameter( "Mask_MicroNormal", UTF8_TO_TCHAR( "MicroNormal Mask Map" ) );

                        if ( strBoneType == "GameBase_Single" )
                        {
                            UpdateStaticParameter( pMaterialInstance, "Enable Eyelash Mask for One Mesh Character", true, false );
                        }
                    }
                    else
                    {
                        strTexturePathToLoad = "/Game/CC_Shaders/SkinShader/Textures/Share/White.White";

                        UTexture2D* pWhiteTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *strTexturePathToLoad ) );
                        if ( pWhiteTexture )
                        {
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "SSS Mask" ) ), pWhiteTexture );
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "MicroNormal Mask Map" ) ), pWhiteTexture );
                        }

                        CMaterialType kType( strMaterialName );
                        FString strMaterialType = kType.GetType();
                        if ( !strMaterialType.IsEmpty() )
                        {
                            fnSetTextureParameter( strMaterialType + "_Mask_SSS_Cavity", UTF8_TO_TCHAR( "SSS Mask" ) );
                            fnSetTextureParameter( strMaterialType + "_Tessellation_Mask", UTF8_TO_TCHAR( "Tessellation Mask Map" ) );
                        }
                    }
                }

                if ( bIsHQSkin )
                {

                    if ( eShaderType == EShaderType::Head || strBoneType == "GameBase_Single" )
                    {
                        fnSetTextureParameter( "SpecularCavity", UTF8_TO_TCHAR( "Specular Cavity Map" ) );
                        fnSetTextureParameter( "MouthCavity_MouthAO", UTF8_TO_TCHAR( "Mouth Cavity and AO Map" ) );
                        fnSetTextureParameter( "HQ_Mask_Cheek_Fore_UpperLid_Chin", UTF8_TO_TCHAR( "CFUC Mask Map" ) );
                        fnSetTextureParameter( "HQ_Mask_Ear_Neck", UTF8_TO_TCHAR( "EN Mask Map" ) );
                        fnSetTextureParameter( "HQ_Mask_Roughness", UTF8_TO_TCHAR( "NLML Mask Map" ) );
                        fnSetTextureParameter( "Mask_Mouth_Cavity", UTF8_TO_TCHAR( "Mouth Cavity Mask Map" ) );
                        fnSetTextureParameter( "Mask_MicroNormal", UTF8_TO_TCHAR( "MicroNormal Mask Map" ) );

                        if ( strBoneType == "GameBase_Single" )
                        {
                            fnSetTextureParameter( "Body_Mask_SSS_Cavity", UTF8_TO_TCHAR( "SSS Map" ) );
                            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "MicroNormal Tiling Value" ) ), 72 );
                        }
                        else
                        {
                            fnSetTextureParameter( "Head_Mask_SSS_Cavity", UTF8_TO_TCHAR( "SSS Map" ) );
                            fnSetTextureParameter( "Head_Tessellation_Mask", UTF8_TO_TCHAR( "Tessellation Mask Map" ) );
                        }

                        UpdateStaticParameter( pMaterialInstance, "Is Head", true, true );
                        if ( strBoneType == "GameBase_Single" )
                        {
                            UpdateStaticParameter( pMaterialInstance, "Enable Eyelash Mask for One Mesh Character", true, true );
                        }


                    }
                    else
                    {
                        strTexturePathToLoad = "/Game/CC_Shaders/SkinShader/Textures/Share/Black.Black";
                        UTexture2D* pBlackTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
                        if ( pBlackTexture )
                        {
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "NLML Mask Map" ) ), pBlackTexture );
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "EN Mask Map" ) ), pBlackTexture );
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "CFUC Mask Map" ) ), pBlackTexture );
                        }

                        FString strArmMicroNormalTexturePath = strCCTexturePath + "Arm_Mask_MicroNormal" + "." + "Arm_Mask_MicroNormal";
                        UTexture2D* pArmMicroNormalTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strArmMicroNormalTexturePath ) ) );
                        if ( pArmMicroNormalTexture )
                        {
                            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "MicroNormal Mask Map" ) ), pArmMicroNormalTexture );
                        }
                        else
                        {
                            strTexturePathToLoad = "/Game/CC_Shaders/SkinShader/Textures/Share/White.White";
                            UTexture2D* pWhiteTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
                            if ( pWhiteTexture )
                            {
                                pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "MicroNormal Mask Map" ) ), pWhiteTexture );
                            }
                        }

                        CMaterialType kType( strMaterialName, strBoneType );
                        float fTilingValue = kType.GetTilingValue();
                        if ( fTilingValue != 0.0f )
                        {
                            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "MicroNormal Tiling Value" ) ), fTilingValue );
                        }

                        FString strMaterialType = kType.GetType();
                        if ( !strMaterialType.IsEmpty() )
                        {
                            fnSetTextureParameter( strMaterialType + "_Mask_SSS_Cavity", UTF8_TO_TCHAR( "SSS Map" ) );
                            fnSetTextureParameter( strMaterialType + "_Tessellation_Mask", UTF8_TO_TCHAR( "Tessellation Mask Map" ) );
                        }
                    }

                    SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );

                    const RLScatter*pScatter = pMaterialData->GetScatter();
                    SetScatter( pScatter, pMaterialInstance, strMaterialName, strSubsurfaceProfilePath, eShaderType );
                }

                pInstUMaterialInterface = pMaterialInstance;
            }
        }
        else if ( eShaderType == EShaderType::GeneralSSS )
        {
            AssignGeneralSss( pMaterialData, kTexturesPathList, strTexturesFilesGamePathFbm, strMaterialName, texturesFilesGamePath, bIsHQSkin, strSubsurfaceProfilePath, strTexturePathToLoad, pInstUMaterialInterface, pMaterial, pMaterialInterface, strCCMaterialFolderGamePath );
        }
    }
    else
    {
        if ( eShaderType == EShaderType::GeneralSSS )
        {
            AssignGeneralSss( pMaterialData, kTexturesPathList, strTexturesFilesGamePathFbm, strMaterialName, texturesFilesGamePath, bIsHQSkin, strSubsurfaceProfilePath, strTexturePathToLoad, pInstUMaterialInterface, pMaterial, pMaterialInterface, strCCMaterialFolderGamePath );
        }
    }

    bool bIsPBR = true;
    if( m_bIsMaterialInstance )
    {
        FString strStandardPackagePath = ( pMaterialData && pMaterialData->m_kTextureDatas.Contains( "Opacity" ) ) ? StandardORMOpacityPackage : StandardORMPackage;
        UMaterialInterface* pStandardMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *strStandardPackagePath );

        if( pStandardMaterialInterface )
        {
            FString PackageName = strCCMaterialFolderGamePath + "/" + strMaterialName + "_Inst";
            FString materialInstancePath = strCCMaterialFolderGamePath + "/" + strMaterialName + "_Inst." + strMaterialName + "_Inst";

            UMaterialInstanceConstant* pMaterialInstance = Cast<UMaterialInstanceConstant>( StaticLoadObject( UMaterialInstanceConstant::StaticClass(), NULL, *( materialInstancePath ) ) );
            
            if( !pMaterialInstance || pMaterialInstance->Parent->GetName() != pStandardMaterialInterface->GetName() )
            {
                FString materialInstanceName = strMaterialName + "_Inst";
                UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
                Factory->InitialParent = pStandardMaterialInterface;
                UPackage* Package = CreatePackage( NULL, *PackageName );
                pMaterialInstance = ( UMaterialInstanceConstant* )Factory->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, FName( *materialInstanceName ), RF_Standalone | RF_Public, NULL, GWarn );
                FAssetRegistryModule::AssetCreated( pMaterialInstance );
            }
            bIsPBR = pMaterialData->m_bIsPbr;

            if( eShaderType == EShaderType::Eyelash || eShaderType == EShaderType::Scalp )
            {
                pMaterialInstance->BasePropertyOverrides.bOverride_BlendMode = true;
                pMaterialInstance->BasePropertyOverrides.BlendMode = EBlendMode::BLEND_Translucent;
                pMaterialInstance->UpdateOverridableBaseProperties();
                if( eShaderType == EShaderType::Scalp )
                {
                    pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Tessellation Level" ) ), 1.0 );
                }
            }

            if( bIsStandardSkin && strMaterialName.ToLower().Contains( "std_eye_" ) )
            {
                pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), 1.0 );
            }

            if( pMaterialData )
            {
                pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), pMaterialData->m_fOpacity );
                if( pMaterialData->m_bTwoSide )
                {
                    pMaterialInstance->BasePropertyOverrides.bOverride_TwoSided = true;
                    pMaterialInstance->BasePropertyOverrides.TwoSided = pMaterialData->m_bTwoSide;
                }
            }

            SetSpecular( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName, bIsPBR );
            SetOpacity( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
            SetGlow( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
            SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
            SetDisplacement( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
            SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );

            strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
            if( kTexturesPathList.Contains( strTexturePathToLoad ) && bIsPBR )
            {
                SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad );
                SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                SetTextureParameter( pMaterialData, "Roughness", pMaterialInstance );
                SetTextureParameter( pMaterialData, "Metallic", pMaterialInstance );
            }
            else
            {
                SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                SetRoughness( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, bIsPBR );
                SetMetallic( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, bIsPBR );
                UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
            }
            SetMultiUvIndex( pMaterialData, pMaterialInstance );

            if( ( ( bIsLWSkin || bIsHQSkin ) && !bIsPBR && strMaterialName.ToLower().Contains( "std_eye_" ) )
                || ( !bIsPBR && strMaterialName.ToLower().Contains( "cornea" ) ) || strMaterialName.ToLower().Contains( "eyemoisture" ) )
            {
                pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), 0.0f );
            }
            if( eShaderType == EShaderType::Tearline || eShaderType == EShaderType::Occulsion )
            {
                pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), 0.0f );
            }

            SetNormal( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
            pMaterialInstance->MarkPackageDirty();
            pMaterialInstance->PostEditChange();
            if( !pInstUMaterialInterface )
            {
                pInstUMaterialInterface = pMaterialInstance;
            }
        }
    }

    return;
}

void FRLPluginModule::AssignGeneralSss( RLMaterialData* pMaterialData, TArray<FString> &kTexturesPathList, 
                                        FString strTexturesFilesGamePathFbm, const FString& strMaterialName, 
                                        FString texturesFilesGamePath, bool bIsHQSkin, const FString& strSubsurfaceProfilePath, 
                                        FString strTexturePathToLoad, UMaterialInstanceConstant*& pInstUMaterialInterface, 
                                        UMaterial* pMaterial, UMaterialInterface* pMaterialInterface, FString strCCMaterialFolderGamePath )
{
    UMaterialInterface* pSSSMaterialInterface = LoadObject<UMaterialInterface>( nullptr, *GeneralSSSPackage );
    if ( pSSSMaterialInterface )
    {
        FString strTexturesFilesGamePathsFbm[ 2 ] ={strTexturesFilesGamePathFbm, texturesFilesGamePath };
        FString texturesFilesGamePaths[ 2 ] ={ texturesFilesGamePath, strTexturesFilesGamePathFbm };

        auto fnSSSMaterialSetting = [ & ]( UMaterialInstanceConstant* pMaterialInstance )
        {
            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity" ) ), pMaterialData->m_fOpacity );
            if ( pMaterialData->m_bTwoSide )
            {
                pMaterialInstance->BasePropertyOverrides.bOverride_TwoSided = true;
                pMaterialInstance->BasePropertyOverrides.TwoSided = pMaterialData->m_bTwoSide;
            }

            bool bIsPBR = pMaterialData->m_bIsPbr;
            SetSpecular( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName, bIsPBR );
            SetOpacity( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
            SetGlow( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
            SetBlend( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
            SetDisplacement( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
            SetBaseColor( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );

            if ( bIsHQSkin )
            {
                SetShaderData( pMaterialData->GetShaderData(), pMaterialInstance, texturesFilesGamePath );

                const RLScatter*pScatter = pMaterialData->GetScatter();
                SetScatter( pScatter, pMaterialInstance, strMaterialName, strSubsurfaceProfilePath, EShaderType::GeneralSSS );
            }

            strTexturePathToLoad = GetTexturePath( pMaterialData, "ORM", texturesFilesGamePath, strMaterialName );
            if ( kTexturesPathList.Contains( strTexturePathToLoad ) && bIsPBR )
            {
                SetOrmTextureSetting( pMaterialInstance, strTexturePathToLoad );
                SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
                SetTextureParameter( pMaterialData, "Roughness", pMaterialInstance );
                SetTextureParameter( pMaterialData, "Metallic", pMaterialInstance );
            }
            else
            {
                SetAO( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName );
                SetRoughness( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, bIsPBR );
                SetMetallic( pMaterialData, pMaterialInstance, kTexturesPathList, texturesFilesGamePaths, strMaterialName, bIsPBR );
                UpdateStaticParameter( pMaterialInstance, "Use ORM Map", false, true );
            }
            SetNormal( pMaterialData, pMaterialInstance, kTexturesPathList, strTexturesFilesGamePathsFbm, strMaterialName );
            SetMultiUvIndex( pMaterialData, pMaterialInstance );
        };
        ProcessMaterialInstanceConstant( pInstUMaterialInterface, pMaterial, pMaterialInterface, pSSSMaterialInterface, strCCMaterialFolderGamePath, fnSSSMaterialSetting );
    }
}

FString FRLPluginModule::GetTexturePath( RLMaterialData *pMaterialData, const FString& strKey, const FString& strTexturesFolderPath, const FString& strMaterialName )
{
    FString strTextureName;
    if( pMaterialData && pMaterialData->m_kTextureDatas.Contains( strKey ) )
    {
        strTextureName = FPaths::GetBaseFilename( pMaterialData->m_kTextureDatas.Find( strKey )->m_strTexturePath );
    }
    else
    {
        static const TMap< FString, FString > kTextureMap{ { "BaseColor", "_Diffuse" },
                                                           { "Normal", "_Normal" },
                                                           { "Bump", "_Normal" },
                                                           { "Specular", "_Specular" },
                                                           { "ORM", "_ORM" },
                                                           { "Roughness", "_roughness" },
                                                           { "Opacity", "_Opacity" },
                                                           { "Metallic", "_metallic" },
                                                           { "AO", "_ao" },
                                                           { "Glow", "_Glow" },
                                                           { "Displacement", "_Displacement" }
        };

        if( kTextureMap.Contains( strKey ) )
        {
            strTextureName = strMaterialName + kTextureMap[ strKey ];
        }
    }
    return strTexturesFolderPath + "/" + strTextureName + "." + strTextureName;
}

void FRLPluginModule::SetTextureParameter( RLMaterialData *pMaterialData, const FString& strKey, UMaterialInstanceConstant* pMaterialInstance )
{
    FString strKeyName = strKey;
    if( strKeyName == "Normal" )
    {
        strKeyName = "Normal/Bump";
    }

    if( pMaterialData && pMaterialData->m_kTextureDatas.Contains( strKey ) )
    {
        RLTextureData *pTextureData = pMaterialData->m_kTextureDatas.Find( strKey );
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( strKeyName + " UOffset" ) ), pTextureData->m_kOffset[ 0 ] );
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( strKeyName + " VOffset" ) ), pTextureData->m_kOffset[ 1 ] );
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( strKeyName + " UTiling" ) ), pTextureData->m_kTiling[ 0 ] );
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( strKeyName + " VTiling" ) ), pTextureData->m_kTiling[ 1 ] );

        if( strKey != "ORM" || strKey != "Metallic" || strKey != "Roughness" )
        {
            float fValue = pTextureData->m_fStrength / 100.0f;
            if( pMaterialInstance->GetFName().ToString().Contains( "_LOD" ) || pMaterialInstance->GetFName().ToString().Contains( "_Merge" ) )
            {
                fValue = strKey == "Displacement" ? 0.0f : 1.0f;
            }
            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( strKeyName + " Strength" ) ), fValue );
        }

        if( strKey == "Displacement" )
        {
            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( "Tessellation Level" ), pTextureData->m_fTessellationLevel );
            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( "Displacement Multiplier" ), pTextureData->m_fTessellationMulitiplier );
            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( "Gray-scale Base Value" ), pTextureData->m_fGrayScaleBaseValue );
        }
    }
}

void FRLPluginModule::UpdateStaticParameter( UMaterialInstanceConstant* pMaterialInstance, const FString& strParameter, bool bEnable, bool bMarkChanged )
{
    FStaticParameterSet kOutStaticParameters;
    pMaterialInstance->GetStaticParameterValues( kOutStaticParameters );
    for( auto& kParameter : kOutStaticParameters.StaticSwitchParameters )
    {
#ifdef UE418
        if( kParameter.ParameterName.ToString().Contains( strParameter ) )
#else
        if( kParameter.ParameterInfo.Name.ToString().Contains( strParameter ) )
#endif
        {
            kParameter.bOverride = true;
            kParameter.Value = bEnable;
        }
    }

    if( bMarkChanged )
    {
        pMaterialInstance->MarkPackageDirty();
        pMaterialInstance->PostEditChange();
    }
    pMaterialInstance->UpdateStaticPermutation( kOutStaticParameters );
}

void FRLPluginModule::SetMultiUvIndex( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance )
{
    if ( pMaterialData->m_iUvChannelIndex != 0 )
    {
        int iIndex = pMaterialData->m_iUvChannelIndex % 4; //最多支援 4 組
        switch ( iIndex )
        {
            case 1:
                UpdateStaticParameter( pMaterialInstance, "Use 2nd UV", true, true );
                break;
            case 2:
                UpdateStaticParameter( pMaterialInstance, "Use 3rd UV", true, true );
                break;
            case 3:
                UpdateStaticParameter( pMaterialInstance, "Use 4th UV", true, true );
                break;
            default:
                UpdateStaticParameter( pMaterialInstance, "Use 1st UV", true, true );
                break;
        }
    }
}

void FRLPluginModule::SetBaseColor( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString strTexturesFilesGamePathsFbm[2], FString strMaterialName )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, "Base Color", strTexturesFilesGamePathsFbm[ i ], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            UTexture* pDiffuseTexture = Cast<UTexture>( StaticLoadObject( UTexture::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
            pDiffuseTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
            pDiffuseTexture->SRGB = true;
            pDiffuseTexture->MarkPackageDirty();
            pDiffuseTexture->PostEditChange();
            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Base Color Map" ) ), pDiffuseTexture );
            SetTextureParameter( pMaterialData, "Base Color", pMaterialInstance );
            break;
        }
    }

    if( pMaterialData )
    {
        FLinearColor kBaseColorTint = FLinearColor( 1.0, 1.0, 1.0, 1.0 );
        kBaseColorTint.R = pMaterialData->m_kDiffuseColor[ 0 ] / 255;
        kBaseColorTint.G = pMaterialData->m_kDiffuseColor[ 1 ] / 255;
        kBaseColorTint.B = pMaterialData->m_kDiffuseColor[ 2 ] / 255;
        pMaterialInstance->SetVectorParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Base Color Tint" ) ), kBaseColorTint );
    }
}

void FRLPluginModule::SetNormal( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString kTexturesFilesGamePathsFbm[2], FString strMaterialName )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, "Normal", kTexturesFilesGamePathsFbm[ i ], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            if ( strTexturePathToLoad.ToLower().Contains( "bump" ) )
            {
                UTexture2D* pBumpTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
                pBumpTexture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
                pBumpTexture->SRGB = false;
                pBumpTexture->MarkPackageDirty();
                pBumpTexture->PostEditChange();
                pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Bump Map" ) ), pBumpTexture );
                SetTextureParameter( pMaterialData, "Normal", pMaterialInstance );
                UpdateStaticParameter( pMaterialInstance, "Use Bump Map", true, true );
            }
            else
            {
                UTexture2D* pNormalTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
                pNormalTexture->CompressionSettings = TextureCompressionSettings::TC_Normalmap;
                pNormalTexture->SRGB = false;
                pNormalTexture->MarkPackageDirty();
                pNormalTexture->PostEditChange();
                pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Normal Map" ) ), pNormalTexture );
                SetTextureParameter( pMaterialData, "Normal", pMaterialInstance );
            }
            break;
        }
    }
}

void FRLPluginModule::SetSpecular( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString strTexturesFilesGamePathsFbm[2], FString strMaterialName, bool bIsPBR )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, "Specular", strTexturesFilesGamePathsFbm[ i ], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) && !bIsPBR )
        {
            UTexture2D* pSpecularTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
            pSpecularTexture->SRGB = false;
            pSpecularTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
            pSpecularTexture->MarkPackageDirty();
            pSpecularTexture->PostEditChange();
            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Specular Map" ) ), pSpecularTexture );
            SetTextureParameter( pMaterialData, "Specular", pMaterialInstance );
            break;
        }
    }
}

void FRLPluginModule::SetOpacity( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString strTexturesFilesGamePathsFbm[2], FString strMaterialName )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, "Opacity", strTexturesFilesGamePathsFbm[ i ], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            UTexture2D* pOpacityTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
            pOpacityTexture->SRGB = false;
            pOpacityTexture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
            pOpacityTexture->MarkPackageDirty();
            pOpacityTexture->PostEditChange();
            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Opacity Map" ) ), pOpacityTexture );
            SetTextureParameter( pMaterialData, "Opacity", pMaterialInstance );
            break;
        }
    }
}

void FRLPluginModule::SetOpacityAdv( RLMaterialData* pMaterialData, UMaterialInstanceConstant* pMaterialInstance )
{
    if ( pMaterialData 
         && pMaterialData->m_kTextureDatas.Contains( "Opacity" )
         && pMaterialInstance )
    {
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( "Alpha Multiplier" ) ), 1.2f );
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( "Alpha Power" ) ), 1.f );
    }
}

void FRLPluginModule::SetGlow( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString strTexturesFilesGamePaths[2], FString strMaterialName )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, "Glow", strTexturesFilesGamePaths[ i ], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            UTexture2D* pEmissiveTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Glow Map" ) ), pEmissiveTexture );
            SetTextureParameter( pMaterialData, "Glow", pMaterialInstance );
            break;
        }
    }

    if( pMaterialData )
    {
        FLinearColor kAmbientColorTint = FLinearColor( 1.0, 1.0, 1.0, 1.0 );
        kAmbientColorTint.R = pMaterialData->m_kAmbientColor[ 0 ] / 255;
        kAmbientColorTint.G = pMaterialData->m_kAmbientColor[ 1 ] / 255;
        kAmbientColorTint.B = pMaterialData->m_kAmbientColor[ 2 ] / 255;
        pMaterialInstance->SetVectorParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Glow Color Tint" ) ), kAmbientColorTint );
        pMaterialInstance->SetScalarParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Self Illumination" ) ), pMaterialData->m_fSelfIllumination );
    }
}

void FRLPluginModule::SetBlendToHairDepthMap( RLMaterialData* pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString kTexturesFilesGamePaths[ 2 ], FString strMaterialName )
{
    FString strKey = "Blend";
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, strKey, kTexturesFilesGamePaths[ i ], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            UTexture* blendTexture = Cast<UTexture>( StaticLoadObject( UTexture::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
            FName mapBlendName( UTF8_TO_TCHAR( "Depth Map" ) );
            pMaterialInstance->SetTextureParameterValueEditorOnly( mapBlendName, blendTexture );
            break;
        }
    }
}

void FRLPluginModule::SetBlend( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString kTexturesFilesGamePaths[2], FString strMaterialName )
{
    FString strKey = "Blend";
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, strKey, kTexturesFilesGamePaths[ i ], strMaterialName );
        float fBlendMode = -1.0;
        if ( pMaterialData  && pMaterialData->m_kTextureDatas.Contains( strKey ) )
        {
            RLTextureData *pTextureData = pMaterialData->m_kTextureDatas.Find( strKey );
            if ( pTextureData->m_strBlendMode == "Multiply" )
            {
                fBlendMode = 0;
            }
            else if ( pTextureData->m_strBlendMode == "Addition" )
            {
                fBlendMode = 1;
            }
            else if ( pTextureData->m_strBlendMode == "Overlay" )
            {
                fBlendMode = 2;
            }
        }
        else
        {
            TMap< const char*, float > kBlendMode{ { "_blend_multiply", 0.0f },
                                                   { "_blend_addition", 1.0f },
                                                   { "_blend_overlay" , 2.0f } };

            for ( auto& kPair : kBlendMode )
            {
                strTexturePathToLoad = kTexturesFilesGamePaths[i] + "/" + strMaterialName + kPair.Key + "." + strMaterialName + kPair.Key;
                if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
                {
                    fBlendMode = kPair.Value;
                    break;
                }
            }
        }

        if ( fBlendMode >= 0 )
        {
            if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
            {
                UTexture* blendTexture = Cast<UTexture>( StaticLoadObject( UTexture::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
                FName mapBlendName( UTF8_TO_TCHAR( "Blend Map" ) );
                pMaterialInstance->SetTextureParameterValueEditorOnly( mapBlendName, blendTexture );
                FName blendModeName( UTF8_TO_TCHAR( "Blend Mode" ) );
                pMaterialInstance->SetScalarParameterValueEditorOnly( blendModeName, fBlendMode );
                SetTextureParameter( pMaterialData, "Blend", pMaterialInstance );
                break;
            }
        }
    }
}

void FRLPluginModule::SetDisplacement( RLMaterialData* pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString strTexturesFilesGamePaths[2], FString strMaterialName )
{
    FString strKey = "Displacement";
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, strKey, strTexturesFilesGamePaths[i], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            UTexture2D* pDisplacementTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );

            if ( pDisplacementTexture->SRGB )
            {
                pDisplacementTexture->SRGB = false;
                pDisplacementTexture->MarkPackageDirty();
                pDisplacementTexture->PostEditChange();
            }

            if ( pMaterialData && pMaterialData->m_kTextureDatas.Contains( strKey ) )
            {
                RLTextureData* pTextureData = pMaterialData->m_kTextureDatas.Find( strKey );
                if ( pTextureData->m_strDisplacementMapType == "Gray Scale Displacement" )
                {
                    pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Gray Scale Displacement Map" ) ), pDisplacementTexture );
                    SetTextureParameter( pMaterialData, strKey, pMaterialInstance );
                }
            }
            break;
        }
    }
}

void FRLPluginModule::SetAO( RLMaterialData *pMaterialData, UMaterialInstanceConstant* pMaterialInstance, TArray<FString> kTexturesPathList, FString strTexturesFilesGamePaths[2], FString strMaterialName )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString strTexturePathToLoad = GetTexturePath( pMaterialData, "AO", strTexturesFilesGamePaths[i], strMaterialName );
        if ( kTexturesPathList.Contains( strTexturePathToLoad ) )
        {
            UTexture2D* pAOTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTexturePathToLoad ) ) );
            if ( pAOTexture->SRGB )
            {
                pAOTexture->SRGB = false;
                pAOTexture->MarkPackageDirty();
                pAOTexture->PostEditChange();
            }
            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "AO Map" ) ), pAOTexture );
            SetTextureParameter( pMaterialData, "AO", pMaterialInstance );
            break;
        }
    }
}

void FRLPluginModule::SetRoughness( RLMaterialData *kMaterialData, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePaths[2], FString materialName, bool isPBR )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString texturePathToLoad = GetTexturePath( kMaterialData, "Roughness", texturesFilesGamePaths[i], materialName );
        if ( texturesPathList.Contains( texturePathToLoad ) && isPBR )
        {
            UTexture2D* roughnessTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( texturePathToLoad ) ) );
            if ( roughnessTexture->SRGB )
            {
                roughnessTexture->SRGB = false;
                roughnessTexture->MarkPackageDirty();
                roughnessTexture->PostEditChange();
            }
            kMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Roughness Map" ) ), roughnessTexture );
            SetTextureParameter( kMaterialData, "Roughness", kMaterialInstance );
            break;
        }
    }
}

void FRLPluginModule::SetMetallic( RLMaterialData *kMaterialData, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePaths[2], FString materialName, bool isPBR )
{
    for ( int i = 0; i < 2; ++i )
    {
        FString texturePathToLoad = GetTexturePath( kMaterialData, "Metallic", texturesFilesGamePaths[ i ], materialName );
        if ( texturesPathList.Contains( texturePathToLoad ) && isPBR )
        {
            UTexture2D* metallicTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( texturePathToLoad ) ) );
            if ( metallicTexture->SRGB )
            {
                metallicTexture->SRGB = false;
                metallicTexture->MarkPackageDirty();
                metallicTexture->PostEditChange();
            }
            kMaterialInstance->SetTextureParameterValueEditorOnly( FName( UTF8_TO_TCHAR( "Metallic Map" ) ), metallicTexture );
            SetTextureParameter( kMaterialData, "Metallic", kMaterialInstance );
            break;
        }
    }
}

void FRLPluginModule::CreateCollisionShapeFromData( RLPhysicsCollisionShapeData* pCSD /*pCollisionShapeData*/, UBodySetup* pBodySetup, const FTransform* kBoneParentWorldTransform )
{
    int32 nNewPrimIndex;
    FTransform CSWorldTransform( FTransform::Identity );
    CSWorldTransform.SetTranslation( FVector( pCSD->m_kWorldTranslate[ 0 ], pCSD->m_kWorldTranslate[ 1 ], pCSD->m_kWorldTranslate[ 2 ] ) );
    CSWorldTransform.SetRotation( FQuat( pCSD->m_kWorldRotation[ 0 ], pCSD->m_kWorldRotation[ 1 ], pCSD->m_kWorldRotation[ 2 ], pCSD->m_kWorldRotation[ 3 ] ) );
    FMatrix LR = CSWorldTransform.ToMatrixNoScale();
    FMatrix mFlipY = { { 1,  0, 0, 0 },
                       { 0, -1, 0, 0 },
                       { 0,  0, 1, 0 },
                       { 0,  0, 0, 1 }
                      };
    LR = mFlipY * LR * mFlipY;
    FTransform LRTransform( LR );
    if ( pCSD->m_strBoundType == "Capsule" )
    {
        if ( pCSD->m_strBoundAxis == "X" )
        {
            FRotator kRotator( -90, 0, 0 );
            FQuat kRotate( kRotator );
            LRTransform.SetRotation( LRTransform.GetRotation() * kRotate );
        }
        if ( pCSD->m_strBoundAxis == "Y" )
        {
            FRotator kRotator( 0, 0, 90 );
            FQuat kRotate( kRotator );
            LRTransform.SetRotation( LRTransform.GetRotation() * kRotate );
        }
        if ( pCSD->m_strBoundAxis == "Z" )
        {
           //
        }
    }
    LR = LRTransform.ToMatrixNoScale();

    FTransform BoneParentWorldTransformInverse = kBoneParentWorldTransform->Inverse();
    FMatrix CSLocalMatrix = LR * BoneParentWorldTransformInverse.ToMatrixNoScale();
    FTransform CSLocal( CSLocalMatrix );
    if ( pCSD->m_strBoundType == "Box" )
    {
        nNewPrimIndex = pBodySetup->AggGeom.BoxElems.Add( FKBoxElem() );
        FKBoxElem* pBoxElem = &pBodySetup->AggGeom.BoxElems[ nNewPrimIndex ];
        pBoxElem->SetName( *pCSD->m_strName );
        pBoxElem->SetTransform( CSLocal );
        pBoxElem->X = pCSD->m_kExtent[ 0 ];
        pBoxElem->Y = pCSD->m_kExtent[ 1 ];
        pBoxElem->Z = pCSD->m_kExtent[ 2 ];
    }
    else if ( pCSD->m_strBoundType == "Sphere" )
    {
        nNewPrimIndex = pBodySetup->AggGeom.SphereElems.Add( FKSphereElem() );
        FKSphereElem* pSphereElem = &pBodySetup->AggGeom.SphereElems[ nNewPrimIndex ];
        pSphereElem->SetTransform( CSLocal );
        pSphereElem->SetName( *pCSD->m_strName );
        pSphereElem->Radius = pCSD->m_fRadius;
    }
    else if ( pCSD->m_strBoundType == "Capsule" )
    {
        nNewPrimIndex = pBodySetup->AggGeom.SphylElems.Add( FKSphylElem() );
        FKSphylElem* pSphylElem = &pBodySetup->AggGeom.SphylElems[ nNewPrimIndex ];
        pSphylElem->SetName( *pCSD->m_strName );
        pSphylElem->SetTransform( CSLocal );
        pSphylElem->Radius = pCSD->m_fRadius;
        pSphylElem->Length = pCSD->m_fCapsuleLength;

    }
}

void FRLPluginModule::CreateConstraint( FName strBoneName, int nBoneID, USkeletalMesh* pMesh, UPhysicsAsset* pPhysicsAsset )
{

    FTransform RelTM = FTransform::Identity;

    int32 nParentIndex = pPhysicsAsset->FindBodyIndex( strBoneName );
    if( nParentIndex == INDEX_NONE )
    {
        return;
    }
    nParentIndex = nBoneID;
    int32 nParentBodyIndex = INDEX_NONE;
    FName strParentName;
    TArray<FTransform> LocalPose = pMesh->RefSkeleton.GetRefBonePose();
    do
    {
        // Transform of child from parent is just child ref-pose entry.
        RelTM = RelTM * LocalPose[ nParentIndex ];

        //Travel up the hierarchy to find a parent which has a valid body
        nParentIndex = pMesh->RefSkeleton.GetParentIndex( nParentIndex );
        if( nParentIndex != INDEX_NONE )
        {
            strParentName = pMesh->RefSkeleton.GetBoneName( nParentIndex );
            nParentBodyIndex = pPhysicsAsset->FindBodyIndex( strParentName );
        }
        else
        {
            //no more parents so just stop
            break;
        }

    } while( nParentBodyIndex == INDEX_NONE );

    if( nParentBodyIndex != INDEX_NONE )
    {
        //found valid parent body so create joint
        int32 NewConstraintIndex = FPhysicsAssetUtils::CreateNewConstraint( pPhysicsAsset, strBoneName );
        UPhysicsConstraintTemplate* CS = pPhysicsAsset->ConstraintSetup[ NewConstraintIndex ];

        // set angular constraint mode
        CS->DefaultInstance.SetAngularSwing1Motion( ACM_Limited );
        CS->DefaultInstance.SetAngularSwing2Motion( ACM_Limited );
        CS->DefaultInstance.SetAngularTwistMotion( ACM_Limited );

        // Place joint at origin of child
        CS->DefaultInstance.ConstraintBone1 = strBoneName;
        CS->DefaultInstance.Pos1 = FVector::ZeroVector;
        CS->DefaultInstance.PriAxis1 = FVector( 1, 0, 0 );
        CS->DefaultInstance.SecAxis1 = FVector( 0, 1, 0 );

        CS->DefaultInstance.ConstraintBone2 = strParentName;
        CS->DefaultInstance.Pos2 = RelTM.GetLocation();
        CS->DefaultInstance.PriAxis2 = RelTM.GetUnitAxis( EAxis::X );
        CS->DefaultInstance.SecAxis2 = RelTM.GetUnitAxis( EAxis::Y );

        CS->SetDefaultProfile( CS->DefaultInstance );

        // Disable collision between constrained bodies by default.
        pPhysicsAsset->DisableCollision( nBoneID, nParentBodyIndex );
    }
}

bool FRLPluginModule::SetShaderTextureSrgbCompression( UTexture* pTexture, FString strKey )
{
    bool bChanged = false;
    bool bSrgb = strKey == "Sclera Map";
    if ( pTexture->SRGB != bSrgb )
    {
        pTexture->SRGB = bSrgb;
        bChanged = true;
    }

    // https://docs.google.com/presentation/d/1vWeo3BmiqExtwlFF3u-V1gGVKpZQl1lZJQI_0je4CkA/edit#slide=id.g7e566ca1bd_1_0
    if ( strKey == "NormalMap_Blend" ||
         strKey == "MicroNormal Map" ||
         strKey == "Normal Map" ||     //Sclera Normal
         strKey == "Iris Normal Map" )
    {
        if ( pTexture->CompressionSettings != TextureCompressionSettings::TC_Normalmap )
        {
            pTexture->CompressionSettings = TextureCompressionSettings::TC_Normalmap;
            bChanged = true;
        }
    }
    return bChanged;
}

// https://docs.google.com/presentation/d/1vWeo3BmiqExtwlFF3u-V1gGVKpZQl1lZJQI_0je4CkA/edit#slide=id.g7e566ca1bd_1_0
void FRLPluginModule::SetShaderData( RLShaderData* pShaderData, UMaterialInstanceConstant* pMaterialInstance, const FString& strFolder )
{
    if ( !pShaderData )
    {
        return;
    }

    for ( auto& kPair : pShaderData->m_kParameter )
    {
        if ( kPair.Key == FLIP_MICRO_NORMAL_Y ||
             kPair.Key == FLIP_TANGENT_Y )
        {
            // -1 = checked, 1 = uncheck
            UpdateStaticParameter( pMaterialInstance, kPair.Key, kPair.Value < 0, true );
        }
        else if ( kPair.Key == AO_MAP_OCCLUDE_ALL_LIGHTS ||
                  kPair.Key == ACTIVATE_HAIR_COLOR )
        {
            // 1=checked, 0=uncheck
            UpdateStaticParameter( pMaterialInstance, kPair.Key, kPair.Value > 0, true );
        }
        else
        {
            pMaterialInstance->SetScalarParameterValueEditorOnly( FName( *FString( kPair.Key ) ), kPair.Value );
        }
    }

    for ( auto& kPair : pShaderData->m_kTexture )
    {
        auto strFileName = FPaths::GetBaseFilename( kPair.Value );
        auto strFilePath = strFolder + "/" + strFileName + "." + strFileName;
        UTexture* pTexture = Cast< UTexture >( StaticLoadObject( UTexture::StaticClass(), NULL, *strFilePath ) );
        if ( pTexture )
        {
            if ( kPair.Key.Find( "Flow Map" ) != -1 )
            {
                // 1=checked, 0=uncheck
                UpdateStaticParameter( pMaterialInstance, "Use Flow Map", true, true );
            }

            if ( SetShaderTextureSrgbCompression( pTexture, kPair.Key ) )
            {
                pTexture->MarkPackageDirty();
                pTexture->PostEditChange();
            }
            pMaterialInstance->SetTextureParameterValueEditorOnly( FName( *FString( kPair.Key ) ), pTexture );
        }
    }

    for ( auto& kPair : pShaderData->m_kColorParameter )
    {
        FLinearColor kColor = FLinearColor( 1.0, 1.0, 1.0, 1.0 );
        const auto& kColorList = kPair.Value;
        kColor.R = kColorList[ 0 ] / 255;
        kColor.G = kColorList[ 1 ] / 255;
        kColor.B = kColorList[ 2 ] / 255;
        pMaterialInstance->SetVectorParameterValueEditorOnly( FName( *FString( kPair.Key ) ), kColor );
    }
}

void FRLPluginModule::SetScatter( const RLScatter* pSss, UMaterialInstanceConstant* pMaterialInstance, const FString& strMaterialName, FString strSubsurfaceProfilePath, EShaderType eShaderType )
{
    bool bIsEye = false;
    RLScatter kSss;
    // IC,CC SSS UI 只有開放部分參數, 只設定 json 檔上有的,其他用 default
    switch ( eShaderType )
    {
        case EShaderType::Skin:
        case EShaderType::Head:
            kSss = g_kSkinSss;
            if ( eShaderType == EShaderType::Head )
            {
                kSss = g_kHeadSss;
            }
            if ( pSss )
            {
                kSss.m_kFalloff = pSss->m_kFalloff;
                kSss.m_fRadius = pSss->m_fRadius;

                kSss.m_fDistribution = pSss->m_fDistribution;
                kSss.m_fIOR = pSss->m_fIOR;
            }
            break;
        case EShaderType::Teeth:
            kSss = g_kTeethSss;
            if ( pSss )
            {
                kSss.m_kFalloff = pSss->m_kFalloff;
                kSss.m_fRadius = pSss->m_fRadius;
            }
            break;
        case EShaderType::Eye:
            bIsEye = true;
            kSss = g_kEyeSss;
            if ( pSss )
            {
                kSss.m_kFalloff = pSss->m_kFalloff;
                kSss.m_fRadius = pSss->m_fRadius;
            }
            break;
        case EShaderType::GeneralSSS:
            kSss = g_kGeneralSss;
            if ( pSss )
            {
                kSss.m_kFalloff = pSss->m_kFalloff;
                kSss.m_fRadius = pSss->m_fRadius;

                kSss.m_fDistribution = pSss->m_fDistribution;
                kSss.m_fIOR = pSss->m_fIOR;
                kSss.m_fExtinctionScale = pSss->m_fExtinctionScale;
                kSss.m_fLobeMix = pSss->m_fLobeMix;
            }
            break;
        default:
            break;
    }


    pMaterialInstance->bOverrideSubsurfaceProfile = true;

    const RLScatter* pScatter = &kSss;

    // try to reuse existing SSS profile
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" );
    TArray<FAssetData> AssetData;
    FName strClassName = USubsurfaceProfile::StaticClass()->GetFName();
    AssetRegistryModule.Get().GetAssetsByClass( strClassName, AssetData );

    bool bEqual = false;
    for ( int i = 0; i < AssetData.Num(); i++ )
    {
        FAssetData& kAssetData = AssetData[ i ];

        UObject* pObj = kAssetData.GetAsset();
        USubsurfaceProfile* pSSSProfile = ( USubsurfaceProfile* )pObj;
        if ( pSSSProfile )
        {
            bEqual = pSSSProfile->Settings.FalloffColor   .Equals( pScatter->m_kFalloff, 1.f/255.f )
                  && pSSSProfile->Settings.SubsurfaceColor.Equals( pScatter->m_kSubsurfaceColor, 1.f / 255.f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.ScatterRadius         , pScatter->m_fRadius         , 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.ScatteringDistribution, pScatter->m_fDistribution   , 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.IOR                   , pScatter->m_fIOR            , 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.ExtinctionScale       , pScatter->m_fExtinctionScale, 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.NormalScale           , pScatter->m_fNormalScale    , 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.Roughness0            , pScatter->m_fRoughness0     , 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.Roughness1            , pScatter->m_fRoughness1     , 1e-5f )
                  && FMath::IsNearlyEqual ( pSSSProfile->Settings.LobeMix               , pScatter->m_fLobeMix        , 1e-5f );

            if ( bIsEye )
            {
                bEqual = pSSSProfile->Settings.FalloffColor.Equals( pScatter->m_kFalloff, 1.f / 255.f )
                      && pSSSProfile->Settings.SubsurfaceColor.Equals( pScatter->m_kSubsurfaceColor, 1.f / 255.f )
                      && FMath::IsNearlyEqual( pSSSProfile->Settings.ScatterRadius, pScatter->m_fRadius, 1e-5f );
            }

            if ( bEqual )
            {
                pMaterialInstance->SubsurfaceProfile = pSSSProfile;
                break;
            }
        }
    }

    if ( !bEqual )
    {
        FString strPackageName = strSubsurfaceProfilePath + "/" + strMaterialName;
        UPackage* pPackage = CreatePackage( NULL, *strPackageName );

        USubsurfaceProfile* pSSSProfile = NewObject< USubsurfaceProfile >( pPackage, FName( *strMaterialName ), RF_Standalone | RF_Public );
        pSSSProfile->Settings.FalloffColor = pScatter->m_kFalloff;
        pSSSProfile->Settings.ScatterRadius = pScatter->m_fRadius;
        pSSSProfile->Settings.ScatteringDistribution = pScatter->m_fDistribution;
        pSSSProfile->Settings.IOR = pScatter->m_fIOR;
        pSSSProfile->Settings.SubsurfaceColor = pScatter->m_kSubsurfaceColor;
        pSSSProfile->Settings.ExtinctionScale = pScatter->m_fExtinctionScale;
        pSSSProfile->Settings.NormalScale = pScatter->m_fNormalScale;
        pSSSProfile->Settings.Roughness0 = pScatter->m_fRoughness0;
        pSSSProfile->Settings.Roughness1 = pScatter->m_fRoughness1;
        pSSSProfile->Settings.LobeMix = pScatter->m_fLobeMix;

        pSSSProfile->MarkPackageDirty();
        FAssetRegistryModule::AssetCreated( pSSSProfile );
        pMaterialInstance->SubsurfaceProfile = pSSSProfile;
    }
}

void FRLPluginModule::ParseJson( const FString& strJsonFilePath, FString& strGeneration, FString& strBoneType,
                                 bool& bSupportShaderSelect,
                                 TMap< FString, RLMaterialData >& kMaterialMap,
                                 TMap< FString, TArray<RLPhysicsCollisionShapeData> >& kCollisionShapeMap )
{
    FString strJsonConfig;
    FFileHelper::LoadFileToString( strJsonConfig, *strJsonFilePath );
    TSharedPtr<FJsonObject> kJsonObject;
    TSharedRef<TJsonReader<>> kReader = TJsonReaderFactory<>::Create( strJsonConfig );
    if ( !FJsonSerializer::Deserialize( kReader, kJsonObject ) )
    {
        return;
    }

    FString strFbxName = FPaths::GetBaseFilename( strJsonFilePath );
    TSharedPtr<FJsonObject> spObjectRoot = kJsonObject->GetObjectField( strFbxName )->GetObjectField( "Scene" );
    if ( spObjectRoot && spObjectRoot->HasField( "SupportShaderSelect" ) )
    {
        bSupportShaderSelect = spObjectRoot->GetBoolField( "SupportShaderSelect" );
    }

    spObjectRoot = kJsonObject->GetObjectField( strFbxName )->GetObjectField( "Object" );
    for ( auto& kObjectJsonField : spObjectRoot->Values )
    {
        TSharedPtr<FJsonObject> spMeshRoot = spObjectRoot->GetObjectField( kObjectJsonField.Key )->GetObjectField( "Meshes" );
        strGeneration = spObjectRoot->GetObjectField( kObjectJsonField.Key )->GetStringField( "Generation" );
        strBoneType = CharacterGenerationBoneMap.Contains(strGeneration) ? CharacterGenerationBoneMap[strGeneration] : "NULL";
        for ( auto& kMeshJsonField : spMeshRoot->Values )
        {
            TSharedPtr<FJsonObject> spMaterialRoot = spMeshRoot->GetObjectField( kMeshJsonField.Key )->GetObjectField( "Materials" );
            for ( auto& kMaterialJsonField : spMaterialRoot->Values )
            {
                TSharedPtr<FJsonObject> spMaterialObject = spMaterialRoot->GetObjectField( kMaterialJsonField.Key );
                TSharedPtr<RLMaterialData> spMaterialData = MakeShareable( new RLMaterialData );
                spMaterialData->m_bIsPbr = spMaterialObject->GetStringField( "Material Type" ).Equals( "Pbr" );

                int32_t iIndex = 0;
                if ( spMaterialObject->TryGetNumberField( "MultiUV Index", iIndex ) )
                {
                    spMaterialData->m_iUvChannelIndex = iIndex;
                }

                FString strType;
                if ( spMaterialObject->TryGetStringField( "Node Type", strType ) )
                {
                    if ( strType == "Hair" )
                    {
                        spMaterialData->m_eNodeType = ENodeType::Hair;
                    }
                    else if ( strType == "Brow" )
                    {
                        spMaterialData->m_eNodeType = ENodeType::Brow;
                    }
                }

                spMaterialData->m_bTwoSide = spMaterialObject->GetBoolField( "Two Side" );
                spMaterialData->m_fOpacity = ( float )spMaterialObject->GetNumberField( "Opacity" );
                spMaterialData->m_fSelfIllumination = ( float )spMaterialObject->GetNumberField( "Self Illumination" );
                spMaterialData->m_fSpecular = ( float )spMaterialObject->GetNumberField( "Specular" );
                spMaterialData->m_fGlossiness = ( float )spMaterialObject->GetNumberField( "Glossiness" );

                TArray<TSharedPtr<FJsonValue>> kDiffuseColor = spMaterialObject->GetArrayField( "Diffuse Color" );
                for ( int i = 0; i < kDiffuseColor.Num(); ++i )
                {
                    spMaterialData->m_kDiffuseColor[ i ] = FCString::Atof( *kDiffuseColor[ i ]->AsString() );
                }
                TArray<TSharedPtr<FJsonValue>> kAmbientColor = spMaterialObject->GetArrayField( "Ambient Color" );
                for ( int i = 0; i < kAmbientColor.Num(); ++i )
                {
                    spMaterialData->m_kAmbientColor[ i ] = FCString::Atof( *kAmbientColor[ i ]->AsString() );
                }
                TArray<TSharedPtr<FJsonValue>> kSpecularColor = spMaterialObject->GetArrayField( "Specular Color" );
                for ( int i = 0; i < kSpecularColor.Num(); ++i )
                {
                    spMaterialData->m_kSpecularColor[ i ] = FCString::Atof( *kSpecularColor[ i ]->AsString() );
                }

                TSharedPtr<FJsonObject> spTextureObject = spMaterialObject->GetObjectField( "Textures" );
                for ( auto& kTextureJsonField : spTextureObject->Values )
                {
                    TSharedPtr<RLTextureData> spTextureData = MakeShareable( new RLTextureData );
                    TSharedPtr<FJsonObject> kTextureObject = spTextureObject->GetObjectField( kTextureJsonField.Key );
                    FString strJsonTexturePath = RemoveInvalidChar( kTextureObject->GetStringField( "Texture Path" ) );
                    spTextureData->m_strTexturePath = strJsonTexturePath;
                    if ( kTextureJsonField.Key != "ORM" )
                    {
                        spTextureData->m_bShareImage = kTextureObject->GetBoolField( "Share Image" );
                        spTextureData->m_fStrength = ( float )kTextureObject->GetNumberField( "Strength" );

                        TArray<TSharedPtr<FJsonValue>> kOffset = kTextureObject->GetArrayField( "Offset" );
                        for ( int i = 0; i < kOffset.Num(); ++i )
                        {
                            spTextureData->m_kOffset[ i ] = FCString::Atof( *kOffset[ i ]->AsString() );
                        }
                        TArray<TSharedPtr<FJsonValue>> kTiling = kTextureObject->GetArrayField( "Tiling" );
                        for ( int i = 0; i < kTiling.Num(); ++i )
                        {
                            spTextureData->m_kTiling[ i ] = FCString::Atof( *kTiling[ i ]->AsString() );
                        }
                    }
                    if ( kTextureJsonField.Key == "Displacement" )
                    {
                        spTextureData->m_fTessellationLevel = ( float )kTextureObject->GetNumberField( "Tessellation Level" );
                        spTextureData->m_fTessellationMulitiplier = ( float )kTextureObject->GetNumberField( "Multiplier" );
                        spTextureData->m_fGrayScaleBaseValue = ( float )kTextureObject->GetNumberField( "Gray-scale Base Value" );
                        spTextureData->m_strDisplacementMapType = kTextureObject->GetStringField( "Displacement Map" );
                    }
                    if ( kTextureJsonField.Key == "Blend" )
                    {
                        spTextureData->m_strBlendMode = kTextureObject->GetStringField( "Blend Mode" );
                    }
                    spMaterialData->m_kTextureDatas.Add( kTextureJsonField.Key, *spTextureData );
                }


                ParseJsonShaderData( spMaterialData, spMaterialObject, kMaterialJsonField.Key, strBoneType );
                ParseJsonScatterData( spMaterialData, spMaterialObject );

                kMaterialMap.Add( RemoveInvalidChar( kMaterialJsonField.Key ), *spMaterialData );
            }
        }

        TSharedPtr<FJsonObject> spPhysicsRoot = spObjectRoot->GetObjectField( kObjectJsonField.Key )->GetObjectField( "Physics" );
        if ( spPhysicsRoot )
        {
            ParseJsonPhysicsData( spPhysicsRoot, kMaterialMap, kCollisionShapeMap, strBoneType == BONE_TYPE_G1 || strBoneType == BONE_TYPE_G3 || strBoneType == BONE_TYPE_G3PLUS );
        }
    }
}

void FRLPluginModule::GetShaderRenameMap( FString strMaterialName, EShaderType eType, RLShaderData* pShader, ENodeType eNodeType, TMap< FString, FString >& kRenameMap )
{
    switch ( eType )
    {
        case EShaderType::Teeth:
            kRenameMap = g_kTeethMap;
            if ( pShader )
            {
                if ( pShader->m_strShaderName == DigitalTongue )
                {
                    kRenameMap = g_kTongueMap;
                }
            }
            else if ( strMaterialName.ToLower().Contains( TougueKeyWord ) )
            {
                kRenameMap = g_kTongueMap;
            }
            break;

        case EShaderType::Eye:
            kRenameMap = g_kEyeMap;
            break;
        case EShaderType::Tearline:
            kRenameMap = g_kTearLineMap;
            break;
        case EShaderType::Occulsion:
            kRenameMap = g_kEyeOccusionMap;
            break;
        case EShaderType::Head:
            kRenameMap = g_kHeadMap;
            break;
        case EShaderType::Skin:
            kRenameMap = g_kSkinMap;
            break;
        case EShaderType::Hair:
            kRenameMap = g_kHairMap;
            break;
        case EShaderType::GeneralSSS:
            kRenameMap = g_kGeneralMap;
            break;
        case EShaderType::PBR:
        case EShaderType::TRA:
            kRenameMap = g_kGeneralMap;
            if ( eNodeType == ENodeType::Hair && !strMaterialName.ToLower().Contains( ScalpKeyWord ) )
            {
                kRenameMap = g_kHairMap;
            }
            break;
        default:
            checkNoEntry();
            break;
    }
}

void FRLPluginModule::ParseJsonShaderData( TSharedPtr< RLMaterialData > spMaterialData, TSharedPtr<FJsonObject> spJsonObject, FString strMaterialName, FString strBoneType )
{
    const TSharedPtr< FJsonObject >* spShaderObject;
    if ( spJsonObject->TryGetObjectField( "Custom Shader", spShaderObject ) )
    {
        TSharedPtr< RLShaderData > spShader = MakeShareable( new RLShaderData );
        spShader->m_strShaderName = ( *spShaderObject )->GetStringField( "Shader Name" );

        TMap< FString, FString > kRenameMap;

        EShaderType eType = GetPbrShaderType( spShader.Get(), strMaterialName, strBoneType );
        GetShaderRenameMap( strMaterialName, eType, spShader.Get(), spMaterialData->m_eNodeType, kRenameMap );

        TSharedPtr< FJsonObject > spImageObject = ( *spShaderObject )->GetObjectField( "Image" );

        auto fnGetUnrealParameterName = [ &kRenameMap ]( const FString& strName )
        {
            auto pResult = kRenameMap.Find( strName );
            return pResult ? *pResult : strName;
        };

        for ( auto& kJsonObject : spImageObject->Values )
        {
            const auto strKey = kJsonObject.Key;
            TSharedPtr<FJsonObject> kTexture = spImageObject->GetObjectField( strKey );

            spShader->m_kTexture.Add( fnGetUnrealParameterName( strKey ), RemoveInvalidChar( kTexture->GetStringField( "Texture Path" ) ) );
        }

        TSharedPtr< FJsonObject > spVarObject = ( *spShaderObject )->GetObjectField( "Variable" );
        for ( auto& kJsonObject : spVarObject->Values )
        {
            const auto strKey = kJsonObject.Key;
            const TArray< TSharedPtr < FJsonValue > >* kValue;
            if ( spVarObject->TryGetArrayField( strKey, kValue ) )
            {
                auto& kColor = spShader->m_kColorParameter.FindOrAdd( fnGetUnrealParameterName( strKey ) );
                for ( int i = 0; i < kValue->Num(); ++i )
                {
                    kColor.Add( FCString::Atof( *( ( *kValue )[ i ] )->AsString() ) );
                }
            }
            else
            {
                spShader->m_kParameter.Add( fnGetUnrealParameterName( strKey ), ( float )spVarObject->GetNumberField( strKey ) );
            }
        }
        spMaterialData->SetShaderData( spShader );
    }
}

void FRLPluginModule::ParseJsonPhysicsData( TSharedPtr<FJsonObject> spPhysicsRoot, TMap< FString, RLMaterialData >& kMaterialMap, TMap< FString, TArray<RLPhysicsCollisionShapeData> >& kCollisionShapeMap, bool bIsCCStdBoneAxis )
{
    TSharedPtr<FJsonObject> spSoftPhysicsRoot = spPhysicsRoot->GetObjectField( "Soft Physics" );
    TSharedPtr<FJsonObject> spMeshesRoot = spSoftPhysicsRoot->GetObjectField( "Meshes" );
    for ( auto& kMeshJsonField : spMeshesRoot->Values )
    {
        TSharedPtr<FJsonObject> spMaterialsRoot = spMeshesRoot->GetObjectField( kMeshJsonField.Key )->GetObjectField( "Materials" );
        for ( auto& kMaterialJsonField : spMaterialsRoot->Values )
        {
            TSharedPtr<FJsonObject> spMaterialRoot = spMaterialsRoot->GetObjectField( kMaterialJsonField.Key );
            FString strKeyName = kMaterialJsonField.Key;
            RemoveMaterialPostfix( strKeyName );
            if ( RLMaterialData* pMaterialData = kMaterialMap.Find( strKeyName ) )
            {
                TSharedPtr<RLPhysicClothData> spPhysicClothData = MakeShareable( new RLPhysicClothData );

                spPhysicClothData->m_bActivate = spMaterialRoot->GetBoolField( "Activate Physics" );
                spPhysicClothData->m_bUseGlobalGravity = spMaterialRoot->GetBoolField( "Use Global Gravity" );
                spPhysicClothData->m_fMass = ( float ) spMaterialRoot->GetNumberField( "Mass" );
                spPhysicClothData->m_fFriction = ( float ) spMaterialRoot->GetNumberField( "Friction" );
                spPhysicClothData->m_fDamping = ( float ) spMaterialRoot->GetNumberField( "Damping" );
                spPhysicClothData->m_fDrag = ( float ) spMaterialRoot->GetNumberField( "Drag" );
                spPhysicClothData->m_fSolverFrequency = ( float ) spMaterialRoot->GetNumberField( "Solver Frequency" );
                spPhysicClothData->m_fTetherLimit = ( float ) spMaterialRoot->GetNumberField( "Tether Limit" );
                spPhysicClothData->m_fElasticity = ( float ) spMaterialRoot->GetNumberField( "Elasticity" );
                spPhysicClothData->m_fStretch = ( float ) spMaterialRoot->GetNumberField( "Stretch" );
                spPhysicClothData->m_fBending = ( float ) spMaterialRoot->GetNumberField( "Bending" );
                {
                    TArray<TSharedPtr<FJsonValue>> kInertia = spMaterialRoot->GetArrayField( "Inertia" );
                    for ( int i = 0; i < kInertia.Num(); ++i )
                    {
                        spPhysicClothData->m_kInertia[ i ] = FCString::Atof( *kInertia[ i ]->AsString() );
                    }
                }
                spPhysicClothData->m_bSoftVsRigidCollision = spMaterialRoot->GetBoolField( "Soft Vs Rigid Collision" );
                spPhysicClothData->m_fSoftVsRigidCollisionMargin = ( float ) spMaterialRoot->GetNumberField( "Soft Vs Rigid Collision_Margin" );
                spPhysicClothData->m_bSelfCollision = spMaterialRoot->GetBoolField( "Self Collision" );
                spPhysicClothData->m_fSelfCollisionMargin = ( float ) spMaterialRoot->GetNumberField( "Self Collision Margin" );
                spPhysicClothData->m_fStiffnessFrequency = ( float ) spMaterialRoot->GetNumberField( "Stiffness Frequency" );

                pMaterialData->m_spPhysicClothData = spPhysicClothData;
            }
        }
    }

    TSharedPtr<FJsonObject> spCollisionShapesRoot = spPhysicsRoot->GetObjectField( "Collision Shapes" );
    for ( auto& kBoneJsonField : spCollisionShapesRoot->Values )
    {
        FString strBoneName = kBoneJsonField.Key;
        TSharedPtr<FJsonObject> spBoneRoot = spCollisionShapesRoot->GetObjectField( kBoneJsonField.Key );
        for ( auto& kShapeJsonField : spBoneRoot->Values )
        {
            FString strShapeName = kShapeJsonField.Key;
            TSharedPtr<FJsonObject> spShapeRoot = spBoneRoot->GetObjectField( kShapeJsonField.Key );
            TSharedPtr<RLPhysicsCollisionShapeData> spCollisionShapeData = MakeShareable( new RLPhysicsCollisionShapeData );

            spCollisionShapeData->m_strName = strShapeName;
            spCollisionShapeData->m_bBoneActivate = spShapeRoot->GetBoolField( "Bone Active" );
            spCollisionShapeData->m_bIsCCStdBoneAxis = bIsCCStdBoneAxis;
            spCollisionShapeData->m_strBoundType = spShapeRoot->GetStringField( "Bound Type" );
            spCollisionShapeData->m_strBoundAxis = spShapeRoot->GetStringField( "Bound Axis" );
            spCollisionShapeData->m_fMargin = spShapeRoot->GetNumberField( "Margin" );
            spCollisionShapeData->m_fFriction = spShapeRoot->GetNumberField( "Fraction" );
            spCollisionShapeData->m_fElasticity = spShapeRoot->GetNumberField( "Elasticity" );
            TArray<TSharedPtr<FJsonValue>> kCenter = spShapeRoot->GetArrayField( "Center" );

            TArray<TSharedPtr<FJsonValue>> kWorldTranslate = spShapeRoot->GetArrayField( "WorldTranslate" );
            for ( int i = 0; i < kWorldTranslate.Num(); ++i )
            {
                spCollisionShapeData->m_kWorldTranslate[ i ] = FCString::Atof( *kWorldTranslate[ i ]->AsString() );
            }

            TArray<TSharedPtr<FJsonValue>> kWorldRotation = spShapeRoot->GetArrayField( "WorldRotationQ" );
            for ( int i = 0; i < kWorldRotation.Num(); ++i )
            {
                spCollisionShapeData->m_kWorldRotation[ i ] = FCString::Atof( *kWorldRotation[ i ]->AsString() );
            }

            TArray<TSharedPtr<FJsonValue>> kWorldScale = spShapeRoot->GetArrayField( "WorldScale" );
            for ( int i = 0; i < kWorldScale.Num(); ++i )
            {
                spCollisionShapeData->m_kWorldScale[ i ] = FCString::Atof( *kWorldScale[ i ]->AsString() );
            }

            FString strShapeType = spCollisionShapeData->m_strBoundType;
            if ( strShapeType == "Box" )
            {
                TArray<TSharedPtr<FJsonValue>> kExtent = spShapeRoot->GetArrayField( "Extent" );
                for ( int i = 0; i < kExtent.Num(); ++i )
                {
                    spCollisionShapeData->m_kExtent[ i ] = FCString::Atof( *kExtent[ i ]->AsString() );
                }
            }
            else if ( strShapeType == "Sphere" )
            {
                spCollisionShapeData->m_fRadius = spShapeRoot->GetNumberField( "Radius" );
            }
            else if ( strShapeType == "Capsule" )
            {
                spCollisionShapeData->m_fRadius = spShapeRoot->GetNumberField( "Radius" );
                spCollisionShapeData->m_fCapsuleLength = spShapeRoot->GetNumberField( "Capsule Length" );
            }

            if ( kCollisionShapeMap.Contains( strBoneName ) )
            {
                kCollisionShapeMap[ strBoneName ].Add( *spCollisionShapeData );
            }
            else
            {
                kCollisionShapeMap.Add( strBoneName, { *spCollisionShapeData } );
            }
        }
    }
}

FString FRLPluginModule::GetMaterialTypeByName( FString strName )
{
    if ( strName.ToLower().Contains( "body" ) )
    {
        return "Body";
    }
    else if ( strName.ToLower().Contains( "leg" ) )
    {
        return "Leg";
    }
    else if ( strName.ToLower().Contains( "arm" ) )
    {
        return "Arm";
    }
    return "NULL";
}

void FRLPluginModule::ParseJsonScatterData( TSharedPtr< RLMaterialData > spMaterialData, TSharedPtr<FJsonObject> spJsonObject )
{
    const TSharedPtr< FJsonObject >* spScatterObject0;
    if ( spJsonObject->TryGetObjectField( "Subsurface Scatter", spScatterObject0 ) )
    {
        TSharedPtr< RLScatter > spScatter = MakeShareable( new RLScatter );

        TSharedPtr< FJsonObject > spScatterObject = spJsonObject->GetObjectField( "Subsurface Scatter" );
        const auto& kFalloff = spScatterObject->GetArrayField( "Falloff" );
        if ( kFalloff.Num() == 3 )
        {
            spScatter->m_kFalloff.R = kFalloff[ 0 ]->AsNumber() / 255.f;
            spScatter->m_kFalloff.G = kFalloff[ 1 ]->AsNumber() / 255.f;
            spScatter->m_kFalloff.B = kFalloff[ 2 ]->AsNumber() / 255.f;
        }

        spScatter->m_fRadius = ( float )spScatterObject->GetNumberField( "Radius" );
        spScatter->m_fDistribution = ( float )spScatterObject->GetNumberField( "Distribution" );
        spScatter->m_fIOR = ( float )spScatterObject->GetNumberField( "IOR" );
        spScatter->m_fExtinctionScale = ( float )spScatterObject->GetNumberField( "DecayScale" );
        spScatter->m_fLobeMix = ( float )spScatterObject->GetNumberField( "Lerp" );
        spMaterialData->SetScatter( spScatter );
    }
}

UObject* FRLPluginModule::RLCreatePhysicsAsset( FName InAssetName, UObject* InParent, USkeletalMesh* SkelMesh )
{
    FString Name = InAssetName.ToString();
    FString PackageName;

    if ( InAssetName == NAME_None )
    {
        // Get a unique package and asset name
        AnimationEditorUtils::CreateUniqueAssetName( SkelMesh->GetOutermost()->GetName(), TEXT( "_Physics" ), PackageName, Name );
    }

    UPackage* Package = Cast<UPackage>( InParent );
    if ( InParent == nullptr && !PackageName.IsEmpty() )
    {
        // Then find/create it.
        Package = CreatePackage( nullptr, *PackageName );
        if ( !ensure( Package ) )
        {
            // There was a problem creating the package
            return nullptr;
        }
    }

    UPhysicsAsset* NewAsset = NewObject<UPhysicsAsset>( Package, *Name, RF_Public | RF_Standalone | RF_Transactional );
    if ( NewAsset )
    {
        SkelMesh->PhysicsAsset = NewAsset;
        NewAsset->PreviewSkeletalMesh = SkelMesh;
        return NewAsset;
    }

    return nullptr;
}

void FRLPluginModule::UpdateWorld( int32 BoneIndex, FTransform ParentWorld, USkeleton* pSkeleton, const TArray<FTransform>& kBoneTransform, TArray<FTransform>& kBoneWorld )
{
    if ( BoneIndex != INDEX_NONE && BoneIndex < kBoneWorld.Num() && BoneIndex < kBoneTransform.Num() )
    {
        kBoneWorld[ BoneIndex ] = kBoneTransform[ BoneIndex ] * ParentWorld;
        TArray<int32> Children;
        pSkeleton->GetChildBones( BoneIndex, Children );
        for ( int i = 0; i < Children.Num(); i++ )
        {
            UpdateWorld( Children[ i ], kBoneWorld[ BoneIndex ], pSkeleton, kBoneTransform, kBoneWorld );
        }
    }
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FRLPluginModule, RLPlugin )

CMaterialType::CMaterialType( FString strMaterialName, FString strBoneType )
{
    TMap< FString, TPair< FString, float > > kTypeMap
    {
        { "body", TPair< FString, float >( "Body", 32 ) },
        { "leg", TPair< FString, float >( "Leg", 32 ) },
        { "arm", TPair< FString, float >( "Arm", 24 ) }
    };

    for ( auto& kPair : kTypeMap )
    {
        if ( strMaterialName.ToLower().Contains( kPair.Key ) )
        {
            m_strMaterialType = kPair.Value.Key;
            m_fTilingValue = kPair.Value.Value;
        }
    }

    if ( !strBoneType.IsEmpty() && strBoneType == "G1" )
    {
        m_fTilingValue *= 2;
    }
}

void FRLPluginModule::ConvertEyeMaterialName( FString& strMaterialName )
{
    if ( strMaterialName.ToLower().Contains( "std_cornea_r" ) || strMaterialName.ToLower().Contains( "std_cornea_l" ) )
    {
        strMaterialName = strMaterialName.Replace( TEXT( "Cornea" ), TEXT( "Eye" ), ESearchCase::IgnoreCase );
    }
}

EShaderType FRLPluginModule::GetShaderType( RLMaterialData* pMaterialData, FString strMaterialName, FString strBoneType )
{
    if ( pMaterialData->m_bIsPbr )
    {
        return GetPbrShaderType( pMaterialData->GetShaderData(), strMaterialName.ToLower(), strBoneType );
    }
    else
    {
        return EShaderType::TRA;
    }
}

EShaderType FRLPluginModule::GetPbrShaderType( const RLShaderData* pShaderData, FString strMaterialName, FString strBoneType )
{
    if ( pShaderData )
    {
        if ( pShaderData->m_strShaderName == DigitalTongue || pShaderData->m_strShaderName == DigitalTeeth )
        {
            return EShaderType::Teeth;
        }
        else if ( pShaderData->m_strShaderName == DigitalEye )
        {
            return EShaderType::Eye;
        }
        else if ( pShaderData->m_strShaderName == DigitalTearline )
        {
            return EShaderType::Tearline;
        }
        else if ( pShaderData->m_strShaderName == DigitalOcculsion )
        {
            return EShaderType::Occulsion;
        }
        else if ( pShaderData->m_strShaderName == DigitalHead )
        {
            return EShaderType::Head;
        }
        else if ( pShaderData->m_strShaderName == DigitalSkin )
        {
            return EShaderType::Skin;
        }
        else if ( pShaderData->m_strShaderName == RLHair )
        {
            return EShaderType::Hair;
        }
        else if ( pShaderData->m_strShaderName == RLGeneralSSS )
        {
            return EShaderType::GeneralSSS;
        }
    }

    if ( strBoneType == "NULL" ) //G5 & G6
    {
        return EShaderType::PBR;
    }

    if ( strMaterialName.Contains( UpperTeethKeyWord ) || strMaterialName.Contains( TougueKeyWord ) || strMaterialName.Contains( LowerTeethKeyWord ) )
    {
        return EShaderType::Teeth;
    }
    else if ( strMaterialName.Contains( ScalpKeyWord ) )
    {
        return EShaderType::Scalp;
    }
    else if( strMaterialName.Contains( EyelashKeyWord ) )
    {
        return EShaderType::Eyelash;
    }
    else if ( strMaterialName.Contains( CorneaKeyWord ) )
    {
        return EShaderType::PBR;
    }
    else if ( strMaterialName.Contains( TearKeyWord ) )
    {
        return EShaderType::Tearline;
    }
    else if ( strMaterialName.Contains( EyeOcclusionKeyWord ) )
    {
        return EShaderType::Occulsion;
    }
    else if ( BoneTypeSkinKeyWord.Contains( strBoneType ) )
    {
        FString SkinKeyword = BoneTypeSkinKeyWord[ strBoneType ];
        if ( strMaterialName.Contains( SkinKeyword ) )
        {
            if ( strMaterialName.Contains( HeadKeyWord ) )
            {
                return EShaderType::Head;
            }
            return EShaderType::Skin;
        }
        else if ( strBoneType == BONE_TYPE_G1 &&
                  strMaterialName.Contains( HeadKeyWordG1 ) )
        {
            return EShaderType::Head;
        }
        return EShaderType::PBR;
    }
    return EShaderType::PBR;
}

void FRLPluginModule::RemoveInvalidTexture( TMap< FString, RLMaterialData >& kMaterialMap, const FString& strTexturePath, const FString& strFbmTexturePath, TArray< FString >& kTexturesPathList )
{
    for ( auto& kMatarial : kMaterialMap )
    {
        auto& kMaterialData = kMatarial.Value;
        auto bPbr = kMaterialData.m_bIsPbr;
        for ( auto kPair : kMaterialData.m_kTextureDatas )
        {
            const auto& strPath = kPair.Value.m_strTexturePath;
            if ( !CheckTextureShouldImport( strPath, bPbr ) )
            {
                auto fnRemove = [ & ]( const FString& strTextureFolder )
                {
                    auto strTextureAssetPath = GetTexturePath( &kMaterialData, kPair.Key, strTextureFolder, kMatarial.Key );
                    auto it = kTexturesPathList.Find( strTextureAssetPath );
                    if ( it )
                    {
                        UTexture2D* pTexture = Cast<UTexture2D>( StaticLoadObject( UTexture2D::StaticClass(), NULL, *( strTextureAssetPath ) ) );
                        if ( pTexture )
                        {
                            kTexturesPathList.Remove( strTextureAssetPath );
                        }
                    }
                };

                fnRemove( strTexturePath );
                fnRemove( strFbmTexturePath );
            }
        }
    }
}

bool FRLPluginModule::CheckTextureShouldImport( const FString& strFilePath, bool bPbr )
{
    const auto& strFileName = FPaths::GetBaseFilename( strFilePath );
    if ( !bPbr && strFileName.Contains( "_ao" ) )
    {
        return true;
    }

    if ( !bPbr && strFileName.Contains( "_ORM" ) )
    {
        return false;
    }

    if ( bPbr && strFileName.Contains( "_specular" ) )
    {
        return false;
    }

    if ( strFileName.Contains( "_metallic" ) || strFileName.Contains( "_roughness" ) || strFileName.Contains( "_ao" ) )
    {
        if ( !bPbr )
        {
            return false;
        }

        int nPosition = 0;
        strFilePath.FindLastChar( '_', nPosition );

        int nPosPosition = 0;
        strFilePath.FindLastChar( '.', nPosPosition );

        auto strSubString = strFilePath.Mid( nPosition, nPosPosition - nPosition );
        FString strOrmPath = strFilePath.Replace( *strSubString, *FString( "_ORM" ) );

        if ( FPaths::FileExists( strOrmPath ) )
        {
            return false;
        }
    }
    return true;
}

#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 24
bool FRLPluginModule::RLPluginImportToLodInternal(
    USkeletalMesh* SourceMesh,
    int32 SourceLodIndex,
    int32 SourceSectionIndex,
    UClothingAssetCommon* DestAsset,
    UClothLODDataBase* DestLod,
    UClothLODDataBase* InParameterRemapSource )
{
    if ( !SourceMesh || !SourceMesh->GetImportedModel() )
    {
        // Invalid mesh
        return false;
    }

    FSkeletalMeshModel* SkeletalResource = SourceMesh->GetImportedModel();

    if ( !SkeletalResource->LODModels.IsValidIndex( SourceLodIndex ) )
    {
        // Invalid LOD
        return false;
    }

    FSkeletalMeshLODModel& SourceLod = SkeletalResource->LODModels[ SourceLodIndex ];

    if ( !SourceLod.Sections.IsValidIndex( SourceSectionIndex ) )
    {
        // Invalid Section
        return false;
    }

    FSkelMeshSection& SourceSection = SourceLod.Sections[ SourceSectionIndex ];

    const int32 NumVerts = SourceSection.SoftVertices.Num();
    const int32 NumIndices = SourceSection.NumTriangles * 3;
    const int32 BaseIndex = SourceSection.BaseIndex;
    const int32 BaseVertexIndex = SourceSection.BaseVertexIndex;

    // We need to weld the mesh verts to get rid of duplicates (happens for smoothing groups)
    TArray<FVector> UniqueVerts;
    TArray<uint32> OriginalIndexes;

    TArray<uint32> IndexRemap;
    IndexRemap.AddDefaulted( NumVerts );
    {
        static const float ThreshSq = SMALL_NUMBER * SMALL_NUMBER;
        for ( int32 VertIndex = 0; VertIndex < NumVerts; ++VertIndex )
        {
            const FSoftSkinVertex& SourceVert = SourceSection.SoftVertices[ VertIndex ];

            bool bUnique = true;
            int32 RemapIndex = INDEX_NONE;

            const int32 NumUniqueVerts = UniqueVerts.Num();
            for ( int32 UniqueVertIndex = 0; UniqueVertIndex < NumUniqueVerts; ++UniqueVertIndex )
            {
                FVector& UniqueVert = UniqueVerts[ UniqueVertIndex ];

                if ( ( UniqueVert - SourceVert.Position ).SizeSquared() <= ThreshSq )
                {
                    // Not unique
                    bUnique = false;
                    RemapIndex = UniqueVertIndex;

                    break;
                }
            }

            if ( bUnique )
            {
                // Unique
                UniqueVerts.Add( SourceVert.Position );
                OriginalIndexes.Add( VertIndex );
                IndexRemap[ VertIndex ] = UniqueVerts.Num() - 1;
            }
            else
            {
                IndexRemap[ VertIndex ] = RemapIndex;
            }
        }
    }

    const int32 NumUniqueVerts = UniqueVerts.Num();

    // If we're going to remap the parameters we need to cache the remap source
    // data. We copy it here incase the destination and remap source
    // lod models are aliased (as in a reimport)
    TArray<FVector> CachedPositions;
    TArray<FVector> CachedNormals;
    TArray<uint32> CachedIndices;
    int32 NumSourceMasks = 0;
    TArray<FPointWeightMap> SourceMaskCopy;

    bool bPerformParamterRemap = false;

    if ( InParameterRemapSource )
    {
        check( InParameterRemapSource->PhysicalMeshData );
        UClothPhysicalMeshDataBase& RemapPhysMesh = *InParameterRemapSource->PhysicalMeshData;
        CachedPositions = RemapPhysMesh.Vertices;
        CachedNormals = RemapPhysMesh.Normals;
        CachedIndices = RemapPhysMesh.Indices;
        SourceMaskCopy = InParameterRemapSource->ParameterMasks;
        NumSourceMasks = SourceMaskCopy.Num();

        bPerformParamterRemap = true;
    }

    check( DestLod->PhysicalMeshData );
    UClothPhysicalMeshDataBase& PhysMesh = *DestLod->PhysicalMeshData;
    PhysMesh.Reset( NumUniqueVerts );
    PhysMesh.Indices.Reset( NumIndices );
    PhysMesh.Indices.AddZeroed( NumIndices );

    TArray<TArray<float>*> WeightArrays = PhysMesh.GetFloatArrays();

    for ( int32 VertexIndex = 0; VertexIndex < NumUniqueVerts; ++VertexIndex )
    {
        const FSoftSkinVertex& SourceVert = SourceSection.SoftVertices[ OriginalIndexes[ VertexIndex ] ];

        PhysMesh.Vertices[ VertexIndex ] = SourceVert.Position;
        PhysMesh.Normals[ VertexIndex ] = SourceVert.TangentZ;
        PhysMesh.VertexColors[ VertexIndex ] = SourceVert.Color;

        for ( int32 WeightArrayIndex = 0; WeightArrayIndex < WeightArrays.Num(); ++WeightArrayIndex )
        {
            TArray<float>* Weights = WeightArrays[ WeightArrayIndex ];
            if ( Weights && Weights->IsValidIndex( VertexIndex ) )
            {
                ( *Weights )[ VertexIndex ] = 0.0f;
            }
        }

        FClothVertBoneData& BoneData = PhysMesh.BoneData[ VertexIndex ];
        for ( int32 InfluenceIndex = 0; InfluenceIndex < MAX_TOTAL_INFLUENCES; ++InfluenceIndex )
        {
            const uint16 SourceIndex = SourceSection.BoneMap[ SourceVert.InfluenceBones[ InfluenceIndex ] ];
            if ( SourceIndex != INDEX_NONE )
            {
                FName BoneName = SourceMesh->RefSkeleton.GetBoneName( SourceIndex );
                BoneData.BoneIndices[ InfluenceIndex ] = DestAsset->UsedBoneNames.AddUnique( BoneName );
                BoneData.BoneWeights[ InfluenceIndex ] = ( float ) SourceVert.InfluenceWeights[ InfluenceIndex ] / 255.0f;
            }
        }
    }

    // Add a max distance parameter mask to begin with
    DestLod->ParameterMasks.AddDefaulted();
    FPointWeightMap& Mask = DestLod->ParameterMasks.Last();
    Mask.CopyFrom( PhysMesh.GetFloatArray( ( uint32 ) MaskTarget_PhysMesh::MaxDistance ), ( uint8 ) MaskTarget_PhysMesh::MaxDistance );
    Mask.bEnabled = true;

    PhysMesh.MaxBoneWeights = SourceSection.MaxBoneInfluences;

    for ( int32 IndexIndex = 0; IndexIndex < NumIndices; ++IndexIndex )
    {
        PhysMesh.Indices[ IndexIndex ] = SourceLod.IndexBuffer[ BaseIndex + IndexIndex ] - BaseVertexIndex;
        PhysMesh.Indices[ IndexIndex ] = IndexRemap[ PhysMesh.Indices[ IndexIndex ] ];
    }

    // Validate the generated triangles. If the source mesh has colinear triangles then clothing simulation will fail
    const int32 NumTriangles = PhysMesh.Indices.Num() / 3;
    for ( int32 TriIndex = 0; TriIndex < NumTriangles; ++TriIndex )
    {
        FVector A = PhysMesh.Vertices[ PhysMesh.Indices[ TriIndex * 3 + 0 ] ];
        FVector B = PhysMesh.Vertices[ PhysMesh.Indices[ TriIndex * 3 + 1 ] ];
        FVector C = PhysMesh.Vertices[ PhysMesh.Indices[ TriIndex * 3 + 2 ] ];

        FVector TriNormal = ( B - A ) ^ ( C - A );
        if ( TriNormal.SizeSquared() <= SMALL_NUMBER )
        {
            // This triangle is colinear
            //LogAndToastWarning( FText::Format( LOCTEXT( "Colinear_Error", "Failed to generate clothing sim mesh due to degenerate triangle, found conincident vertices in triangle A={0} B={1} C={2}" ), FText::FromString( A.ToString() ), FText::FromString( B.ToString() ), FText::FromString( C.ToString() ) ) );

            return false;
        }
    }

    if ( bPerformParamterRemap )
    {
        ClothingMeshUtils::FVertexParameterMapper ParameterRemapper( PhysMesh.Vertices, PhysMesh.Normals, CachedPositions, CachedNormals, CachedIndices );

        DestLod->ParameterMasks.Reset( NumSourceMasks );

        for ( int32 MaskIndex = 0; MaskIndex < NumSourceMasks; ++MaskIndex )
        {
            FPointWeightMap& SourceMask = SourceMaskCopy[ MaskIndex ];

            DestLod->ParameterMasks.AddDefaulted();
            FPointWeightMap& DestMask = DestLod->ParameterMasks.Last();

            DestMask.Initialize( PhysMesh.Vertices.Num() );
            DestMask.CurrentTarget = SourceMask.CurrentTarget;
            DestMask.bEnabled = SourceMask.bEnabled;

            ParameterRemapper.Map( SourceMask.GetValueArray(), DestMask.Values );
        }

        DestAsset->ApplyParameterMasks();
    }

    return true;
}
#endif

void FRLPluginModule::CreatePhysicCollisionShape( UPhysicsAsset* pPhysicsAsset, 
                                                  USkeletalMesh* pMesh, 
                                                  USkeleton* pSkeleton, 
                                                  const TMap< FString, TArray<RLPhysicsCollisionShapeData> >& kCollisionShapeMap )
{
    //collision shape create start
    //delete all body shapes 刪除 unreal 自動建的資料

    if ( kCollisionShapeMap.Num() == 0 )
    {
        return;
    }

    int size = pPhysicsAsset->SkeletalBodySetups.Num();
    for ( int i = 0; i < size; i++ )
    {
        FPhysicsAssetUtils::DestroyBody( pPhysicsAsset, 0 );
    }

    TArray<FTransform> kBoneTransform = pSkeleton->GetReferenceSkeleton().GetRefBonePose();
    TArray<FTransform> kBoneWorld = pSkeleton->GetReferenceSkeleton().GetRefBonePose();
    int32 RoottBoneIndex = pSkeleton->GetReferenceSkeleton().FindBoneIndex( "root" );
    UpdateWorld( RoottBoneIndex, FTransform::Identity, pSkeleton, kBoneTransform, kBoneWorld );

    for ( auto kCollisionShapeMapPair : kCollisionShapeMap )
    {
        auto kCollisionShapeArray = kCollisionShapeMapPair.Value;
        for ( auto kCollisionShapeData : kCollisionShapeArray )
        {
            if ( kCollisionShapeData.m_bBoneActivate )
            {
                FName boneName = ( *kCollisionShapeMapPair.Key );
#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 20 || ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 21

                int boneID = FPhysicsAssetUtils::CreateNewBody( pPhysicsAsset, boneName );
#else
                int boneID = FPhysicsAssetUtils::CreateNewBody( pPhysicsAsset, boneName, FPhysAssetCreateParams::FPhysAssetCreateParams() );
#endif
                UBodySetup* pBodySetup = pPhysicsAsset->SkeletalBodySetups[ boneID ];

                int nBoneIndex = pSkeleton->GetReferenceSkeleton().FindBoneIndex( boneName );
                if ( nBoneIndex != INDEX_NONE )
                {
                    int nBoneParentIndex = pSkeleton->GetReferenceSkeleton().GetParentIndex( nBoneIndex );
                    if ( nBoneIndex < kBoneWorld.Num() )
                    {
                        CreateCollisionShapeFromData( &kCollisionShapeData, pBodySetup, &kBoneWorld[ nBoneIndex ] );
                    }
                }
            }
        }
    }

    //最後仿照unreal的方法重新建立 constraint
    for ( int i = 0; i < pMesh->RefSkeleton.GetNum(); i++ )
    {
        FName boneName = pMesh->RefSkeleton.GetBoneName( i );
        CreateConstraint( boneName, i, pMesh, pPhysicsAsset );
    }
    
    //collision shap create end
}

void FRLPluginModule::CreatePhysicSoftCloth( UPhysicsAsset* pPhysicsAsset, USkeletalMesh* pMesh, const FString& strBoneType, TMap< FString, RLMaterialData >& kMaterialMap )
{
    //cloth test-start 實作是在UE4.22上實作  4.20 4.21 還需要修改一些相容性
    for ( int i = 0; i < pMesh->Materials.Num(); ++i )
    {
        auto pMaterial = pMesh->Materials[ i ];
        if ( RLMaterialData* pMaterialData = kMaterialMap.Find( pMaterial.MaterialSlotName.ToString() ) )
        {
            auto pPhysicClothData = pMaterialData->m_spPhysicClothData;
            if ( pPhysicClothData && pPhysicClothData->m_bActivate )
            {
                FClothingSystemEditorInterfaceModule& ClothingEditorModule = FModuleManager::LoadModuleChecked<FClothingSystemEditorInterfaceModule>( "ClothingSystemEditorInterface" );
                UClothingAssetFactoryBase* AssetFactory = ClothingEditorModule.GetClothingAssetFactory();
                bool bIsHair = pMaterialData->m_eNodeType == ENodeType::Hair;
                auto eShaderType = GetShaderType( pMaterialData, pMaterial.MaterialSlotName.ToString().ToLower(), strBoneType );
                bIsHair |= ( eShaderType == EShaderType::Hair );

#if ( ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION >= 24 ) || ENGINE_MAJOR_VERSION >= 5
                FScopedSkeletalMeshPostEditChange ScopedSkeletalMeshPostEditChange( pMesh );
#endif

                pMesh->Modify();
                FSkeletalMeshClothBuildParams* Params = new FSkeletalMeshClothBuildParams();
                Params->LodIndex = 0;
                Params->SourceSection = i;//13; //要改成讀 json 找到material 對應的 ID //用 slot name 找
                Params->AssetName = pMaterial.MaterialSlotName.ToString() + "_ClothingData"; //Material: Dress_Pbr_ClothingData
                Params->PhysicsAsset = pPhysicsAsset;

                //1.
                UClothingAssetBase* NewClothingAsset = AssetFactory->CreateFromSkeletalMesh( pMesh, *Params );

#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION < 24
                if ( NewClothingAsset )
                {
                    pMesh->AddClothingAsset( NewClothingAsset );
                    UClothingAsset* ClothingAsset = Cast<UClothingAsset>( NewClothingAsset );
                    if ( ClothingAsset )
                    {
                        ClothingAsset->BindToSkeletalMesh( pMesh, Params->LodIndex, Params->SourceSection, Params->LodIndex );//Apply
                    }

                    FClothLODData& ClothLODData = ClothingAsset->LodData[ Params->LodIndex ];
                    const int32 NumRequiredValues = ClothLODData.PhysicalMeshData.Vertices.Num();
                    FClothParameterMask_PhysMesh* mask = &ClothLODData.ParameterMasks[ 0 ];
                    mask->bEnabled = true;
                    //讀入vertex color
                    check( ClothLODData.PhysicalMeshData.Vertices.Num() == ClothLODData.PhysicalMeshData.VertexColors.Num() );
                    int32 NumVerts = ClothLODData.PhysicalMeshData.Vertices.Num();
                    check( mask->Values.Num() == NumVerts );
                    float scaleFactor = 100.0;
                    for ( int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++ )
                    {
                        const FColor VertColor = ClothLODData.PhysicalMeshData.VertexColors[ VertIdx ];
                        uint8 Value = 0;
                        Value = VertColor.R;
                        mask->Values[ VertIdx ] = ( Value / 255.f ) * scaleFactor;
                    }
#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 20 || ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 21
                    mask->MaxValue = scaleFactor; //View Max
                    mask->MinValue = 0; //View Min
#elif ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 22
                    mask->MaxValue_DEPRECATED = 100;
                    mask->MinValue_DEPRECATED = 0;
#endif                  
                    mask->Apply( ClothLODData.PhysicalMeshData );

                    ClothingAsset->ClothConfig.SelfCollisionCullScale = 1.f;
                    ClothingAsset->ClothConfig.VerticalConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fStretch ) * 0.01 ); // 1 - ((Stretch) * 0.01)
                    ClothingAsset->ClothConfig.HorizontalConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fStretch ) * 0.01 ); // 1 - ((Stretch) * 0.01)
                    ClothingAsset->ClothConfig.BendConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fBending ) * 0.01 ); // 1 - ((Bending) * 0.01)
                    ClothingAsset->ClothConfig.ShearConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fStretch ) * 0.01 ); // 1 - ((Stretch) * 0.01)
                    ClothingAsset->ClothConfig.SelfCollisionRadius = pPhysicClothData->m_bSelfCollision ? pPhysicClothData->m_fSelfCollisionMargin : 0.f; // Self Collision Margin
                    ClothingAsset->ClothConfig.SelfCollisionStiffness = 0.9f; //Self Collision Stiffness
                    ClothingAsset->ClothConfig.Damping.X = ClothingAsset->ClothConfig.Damping.Y = ClothingAsset->ClothConfig.Damping.Z = pPhysicClothData->m_fDamping; //Damping
                    ClothingAsset->ClothConfig.Friction = pPhysicClothData->m_fFriction; //Friction
                    ClothingAsset->ClothConfig.LinearDrag.X = ClothingAsset->ClothConfig.LinearDrag.Y = ClothingAsset->ClothConfig.LinearDrag.Z = pPhysicClothData->m_fDrag;//Drag
                    ClothingAsset->ClothConfig.AngularDrag.X = ClothingAsset->ClothConfig.AngularDrag.Y = ClothingAsset->ClothConfig.AngularDrag.Z = pPhysicClothData->m_fDrag; //Drag
                    ClothingAsset->ClothConfig.LinearInertiaScale.X = ClothingAsset->ClothConfig.LinearInertiaScale.Y = ClothingAsset->ClothConfig.LinearInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f; //Ineria / 10
                    ClothingAsset->ClothConfig.AngularInertiaScale.X = ClothingAsset->ClothConfig.AngularInertiaScale.Y = ClothingAsset->ClothConfig.AngularInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f;  //Ineria / 10
                    ClothingAsset->ClothConfig.CentrifugalInertiaScale.X = ClothingAsset->ClothConfig.CentrifugalInertiaScale.Y = ClothingAsset->ClothConfig.CentrifugalInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f; //Ineria / 10
                    ClothingAsset->ClothConfig.SolverFrequency = pPhysicClothData->m_fSolverFrequency > 1000.f ? 1000.f : pPhysicClothData->m_fSolverFrequency; //Solver Frequency
                    ClothingAsset->ClothConfig.StiffnessFrequency = pPhysicClothData->m_fStiffnessFrequency;
                    ClothingAsset->ClothConfig.GravityScale = pPhysicClothData->m_bUseGlobalGravity ? 1 : 0; //Object Gravity false =0, true =1
                    ClothingAsset->ClothConfig.TetherStiffness = 1 - ( ( pPhysicClothData->m_fElasticity - 1 ) * 0.01 ); //1 -  ((Elasticity - 1) * 0.01)
                    ClothingAsset->ClothConfig.TetherLimit = pPhysicClothData->m_fTetherLimit; //Tether Limit
                    ClothingAsset->ClothConfig.CollisionThickness = pPhysicClothData->m_bSoftVsRigidCollision ? pPhysicClothData->m_fSoftVsRigidCollisionMargin : 0; //Soft vs Rigid  Collision Margin

                    ClothingAsset->RefreshBoneMapping( pMesh );
                    ClothingAsset->BuildLodTransitionData();
                    ClothingAsset->InvalidateCachedData();
                    ClothingAsset->MarkPackageDirty();
                    ClothingAsset->PostEditChange();
                    NewClothingAsset->MarkPackageDirty();
                    NewClothingAsset->PostEditChange();
                }
#elif ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 24
                if ( NewClothingAsset )
                {
                    pMesh->AddClothingAsset( NewClothingAsset );
                    UClothingAssetNv* pClothingAssetNv = Cast<UClothingAssetNv>( NewClothingAsset );
                    if ( pClothingAssetNv )
                    {
                        float scaleFactor = 100.0;

                        { //vertex color to cloth paint w/ none zero value
                            UClothLODDataBase* ClothLODData = pClothingAssetNv->ClothLodData[ Params->LodIndex ];
                            FPointWeightMap* mask = &ClothLODData->ParameterMasks[ 0 ];
                            mask->bEnabled = true;

                            check( ClothLODData->PhysicalMeshData->Vertices.Num() == ClothLODData->PhysicalMeshData->VertexColors.Num() );
                            int32 NumVerts = ClothLODData->PhysicalMeshData->Vertices.Num();
                            check( mask->Values.Num() == NumVerts );
                            for ( int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++ )
                            {
                                const FColor VertColor = ClothLODData->PhysicalMeshData->VertexColors[ VertIdx ];
                                uint8 Value = 0;
                                Value = VertColor.R;
                                mask->Values[ VertIdx ] = 0.9f;
                            }
                            pClothingAssetNv->ApplyParameterMasks();
                            pClothingAssetNv->RefreshBoneMapping( pMesh );
                            pClothingAssetNv->BuildLodTransitionData();
                            pClothingAssetNv->InvalidateCachedData();
                            pClothingAssetNv->MarkPackageDirty();
                            pClothingAssetNv->PostEditChange();
                        }

                        {
                            FSkeletalMeshLODModel& LODModel = pMesh->GetImportedModel()->LODModels[ Params->LodIndex ];
                            const FSkelMeshSection& Section = LODModel.Sections[ Params->SourceSection ];
                            FSkelMeshSourceSectionUserData& OriginalSectionData = LODModel.UserSectionsData.FindOrAdd( Section.OriginalDataSectionIndex );
                            int32 AssetIndex = INDEX_NONE;
                            pMesh->MeshClothingAssets.Find( pClothingAssetNv, AssetIndex );
                            OriginalSectionData.CorrespondClothAssetIndex = AssetIndex;
                            OriginalSectionData.ClothingData.AssetGuid = pClothingAssetNv->GetAssetGuid();
                            OriginalSectionData.ClothingData.AssetLodIndex = Params->LodIndex;
                        }

                        { //Bind
                            if ( pClothingAssetNv->BindToSkeletalMesh( pMesh, Params->LodIndex, Params->SourceSection, Params->LodIndex ) )//Apply) 
                            {
                                pMesh->MarkPackageDirty();
                                pMesh->PostEditChange();
                            }
                        }

                        { //vertex color to cloth paint AGAIN w/ intended value
                            UClothLODDataBase* ClothLODData = pClothingAssetNv->ClothLodData[ Params->LodIndex ];
                            FPointWeightMap* mask = &ClothLODData->ParameterMasks[ 0 ];
                            mask->bEnabled = true;

                            check( ClothLODData->PhysicalMeshData->Vertices.Num() == ClothLODData->PhysicalMeshData->VertexColors.Num() );
                            int32 NumVerts = ClothLODData->PhysicalMeshData->Vertices.Num();
                            check( mask->Values.Num() == NumVerts );
                            for ( int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++ )
                            {
                                const FColor VertColor = ClothLODData->PhysicalMeshData->VertexColors[ VertIdx ];
                                uint8 Value = 0;
                                Value = VertColor.R;
                                mask->Values[ VertIdx ] = ( Value / 255.f ) * scaleFactor;
                            }
                            pClothingAssetNv->ApplyParameterMasks();
                            pClothingAssetNv->RefreshBoneMapping( pMesh );
                            pClothingAssetNv->BuildLodTransitionData();
                            pClothingAssetNv->InvalidateCachedData();
                            pClothingAssetNv->MarkPackageDirty();
                            pClothingAssetNv->PostEditChange();
                        }

                        { //Settings
                            UClothConfigBase* ClothConfigBase = pClothingAssetNv->ClothSimConfig;
                            UClothConfigNv* ClothConfigNv = Cast<UClothConfigNv>( ClothConfigBase );

                            ClothConfigNv->SelfCollisionCullScale = 1.f;
                            ClothConfigNv->VerticalConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fStretch ) * 0.01 ); // 1 - ((Stretch) * 0.01)
                            ClothConfigNv->HorizontalConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fStretch ) * 0.01 ); // 1 - ((Stretch) * 0.01)
                            ClothConfigNv->BendConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fBending ) * 0.01 ); // 1 - ((Bending) * 0.01)
                            ClothConfigNv->ShearConstraintConfig.Stiffness = 1 - ( ( pPhysicClothData->m_fStretch ) * 0.01 ); // 1 - ((Stretch) * 0.01)
                            ClothConfigNv->SelfCollisionRadius = pPhysicClothData->m_bSelfCollision ? pPhysicClothData->m_fSelfCollisionMargin : 0.f; // Self Collision Margin
                            ClothConfigNv->SelfCollisionStiffness = 0.9f; //Self Collision Stiffness
                            ClothConfigNv->Damping.X = ClothConfigNv->Damping.Y = ClothConfigNv->Damping.Z = pPhysicClothData->m_fDamping; //Damping
                            ClothConfigNv->Friction = pPhysicClothData->m_fFriction; //Friction
                            ClothConfigNv->LinearDrag.X = ClothConfigNv->LinearDrag.Y = ClothConfigNv->LinearDrag.Z = pPhysicClothData->m_fDrag;//Drag
                            ClothConfigNv->AngularDrag.X = ClothConfigNv->AngularDrag.Y = ClothConfigNv->AngularDrag.Z = pPhysicClothData->m_fDrag; //Drag
                            ClothConfigNv->LinearInertiaScale.X = ClothConfigNv->LinearInertiaScale.Y = ClothConfigNv->LinearInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f; //Ineria / 10
                            ClothConfigNv->AngularInertiaScale.X = ClothConfigNv->AngularInertiaScale.Y = ClothConfigNv->AngularInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f;  //Ineria / 10
                            ClothConfigNv->CentrifugalInertiaScale.X = ClothConfigNv->CentrifugalInertiaScale.Y = ClothConfigNv->CentrifugalInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f; //Ineria / 10
                            ClothConfigNv->SolverFrequency = pPhysicClothData->m_fSolverFrequency > 1000.f ? 1000.f : pPhysicClothData->m_fSolverFrequency; //Solver Frequency
                            ClothConfigNv->StiffnessFrequency = pPhysicClothData->m_fStiffnessFrequency;
                            ClothConfigNv->GravityScale = pPhysicClothData->m_bUseGlobalGravity ? 1 : 0; //Object Gravity false =0, true =1
                            ClothConfigNv->TetherStiffness = 1 - ( ( pPhysicClothData->m_fElasticity - 1 ) * 0.01 ); //1 -  ((Elasticity - 1) * 0.01)
                            ClothConfigNv->TetherLimit = pPhysicClothData->m_fTetherLimit; //Tether Limit
                            ClothConfigNv->CollisionThickness = pPhysicClothData->m_bSoftVsRigidCollision ? pPhysicClothData->m_fSoftVsRigidCollisionMargin : 0; //Soft vs Rigid  Collision Margin

                            pClothingAssetNv->RefreshBoneMapping( pMesh );
                            pClothingAssetNv->BuildLodTransitionData();
                            pClothingAssetNv->InvalidateCachedData();
                            pClothingAssetNv->MarkPackageDirty();
                            pClothingAssetNv->PostEditChange();
                        }
                    }

                    NewClothingAsset->MarkPackageDirty();
                    NewClothingAsset->PostEditChange();
                }

#else
                if ( NewClothingAsset )
                {
                    pMesh->AddClothingAsset( NewClothingAsset );
                    UClothingAssetCommon* pClothingAssetCommon = Cast<UClothingAssetCommon>( NewClothingAsset );
                    if ( pClothingAssetCommon )
                    {
                        float scaleFactor = bIsHair ? 15.f : 100.0;
                        {
                            FSkeletalMeshLODModel& LODModel = pMesh->GetImportedModel()->LODModels[ Params->LodIndex ];
                            const FSkelMeshSection& Section = LODModel.Sections[ Params->SourceSection ];
                            FSkelMeshSourceSectionUserData& OriginalSectionData = LODModel.UserSectionsData.FindOrAdd( Section.OriginalDataSectionIndex );
                            int32 AssetIndex = INDEX_NONE;
                            pMesh->MeshClothingAssets.Find( pClothingAssetCommon, AssetIndex );
                            OriginalSectionData.CorrespondClothAssetIndex = AssetIndex;
                            OriginalSectionData.ClothingData.AssetGuid = pClothingAssetCommon->GetAssetGuid();
                            OriginalSectionData.ClothingData.AssetLodIndex = Params->LodIndex;
                        }
                
                        { //Bind
                            if ( pClothingAssetCommon->BindToSkeletalMesh( pMesh, Params->LodIndex, Params->SourceSection, Params->LodIndex ) )//Apply) 
                            {
                                pMesh->MarkPackageDirty();
                                pMesh->PostEditChange();
                            }
                        }
                
                        { //vertex color to cloth paint w/ intended value
                            FClothLODDataCommon* ClothLODData = &pClothingAssetCommon->LodData[ Params->LodIndex ];
                            FPointWeightMap* mask = &ClothLODData->PointWeightMaps[ 0 ];
                            mask->bEnabled = true;
                
                            check( ClothLODData->PhysicalMeshData.Vertices.Num() == ClothLODData->PhysicalMeshData.VertexColors.Num() );
                            int32 NumVerts = ClothLODData->PhysicalMeshData.Vertices.Num();
                            check( mask->Values.Num() == NumVerts );
                            for ( int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++ )
                            {
                                const FColor VertColor = ClothLODData->PhysicalMeshData.VertexColors[ VertIdx ];
                                uint8 Value = 0;
                                Value = VertColor.R;
                                mask->Values[ VertIdx ] = ( Value / 255.f ) * scaleFactor;
                            }

                            pClothingAssetCommon->ApplyParameterMasks();
                            pClothingAssetCommon->RefreshBoneMapping( pMesh );
                            pClothingAssetCommon->BuildLodTransitionData();
                            pClothingAssetCommon->InvalidateCachedData();
                            pClothingAssetCommon->MarkPackageDirty();
                            pClothingAssetCommon->PostEditChange();
                        }

                        { //Settings
                            UClothConfigNv* ClothConfigNv = pClothingAssetCommon->GetClothConfig<UClothConfigNv >();
                            if ( ClothConfigNv )
                            {
                                ClothConfigNv->SelfCollisionCullScale = 1.f;
                                ClothConfigNv->VerticalConstraint.Stiffness = 1 - ((pPhysicClothData->m_fStretch) * 0.01); // 1 - ((Stretch) * 0.01)
                                ClothConfigNv->HorizontalConstraint.Stiffness = 1 - ((pPhysicClothData->m_fStretch) * 0.01); // 1 - ((Stretch) * 0.01)
                                ClothConfigNv->BendConstraint.Stiffness = 1 - ((pPhysicClothData->m_fBending) * 0.01); // 1 - ((Bending) * 0.01)
                                ClothConfigNv->ShearConstraint.Stiffness = 1 - ((pPhysicClothData->m_fStretch) * 0.01); // 1 - ((Stretch) * 0.01)
                                ClothConfigNv->SelfCollisionRadius = pPhysicClothData->m_bSelfCollision ? pPhysicClothData->m_fSelfCollisionMargin : 0.f; // Self Collision Margin
                                ClothConfigNv->SelfCollisionStiffness = 0.9f; //Self Collision Stiffness
                                ClothConfigNv->Damping.X = ClothConfigNv->Damping.Y = ClothConfigNv->Damping.Z = pPhysicClothData->m_fDamping; //Damping
                                ClothConfigNv->Friction = pPhysicClothData->m_fFriction; //Friction
                                ClothConfigNv->LinearDrag.X = ClothConfigNv->LinearDrag.Y = ClothConfigNv->LinearDrag.Z = pPhysicClothData->m_fDrag;//Drag
                                ClothConfigNv->AngularDrag.X = ClothConfigNv->AngularDrag.Y = ClothConfigNv->AngularDrag.Z = pPhysicClothData->m_fDrag; //Drag
                                ClothConfigNv->LinearInertiaScale.X = ClothConfigNv->LinearInertiaScale.Y = ClothConfigNv->LinearInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f; //Ineria / 10
                                ClothConfigNv->AngularInertiaScale.X = ClothConfigNv->AngularInertiaScale.Y = ClothConfigNv->AngularInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f;  //Ineria / 10
                                ClothConfigNv->CentrifugalInertiaScale.X = ClothConfigNv->CentrifugalInertiaScale.Y = ClothConfigNv->CentrifugalInertiaScale.Z = pPhysicClothData->m_kInertia[ 0 ] / 10.f; //Ineria / 10
                                ClothConfigNv->SolverFrequency = pPhysicClothData->m_fSolverFrequency > 1000.f ? 1000.f : pPhysicClothData->m_fSolverFrequency; //Solver Frequency
                                ClothConfigNv->StiffnessFrequency = pPhysicClothData->m_fStiffnessFrequency;
                                ClothConfigNv->GravityScale = pPhysicClothData->m_bUseGlobalGravity ? 1 : 0; //Object Gravity false =0, true =1
                                ClothConfigNv->TetherStiffness = 1 - ((pPhysicClothData->m_fElasticity - 1) * 0.01); //1 -  ((Elasticity - 1) * 0.01)
                                ClothConfigNv->TetherLimit = pPhysicClothData->m_fTetherLimit; //Tether Limit
                                ClothConfigNv->CollisionThickness = pPhysicClothData->m_bSoftVsRigidCollision ? pPhysicClothData->m_fSoftVsRigidCollisionMargin : 0; //Soft vs Rigid  Collision Margin
                            }
#if ENGINE_MAJOR_VERSION >= 5
                            UChaosClothConfig* pChaosClothConfig = pClothingAssetCommon->GetClothConfig<UChaosClothConfig>();
                            if ( pChaosClothConfig )
                            {
                                pChaosClothConfig->CollisionThickness = 0;
                            }
                            if ( bIsHair )
                            {
                                if ( pChaosClothConfig )
                                {
                                    pChaosClothConfig->AnimDriveStiffness.Low = 0.3f;
                                    pChaosClothConfig->AnimDriveDamping.Low   = 0.2f;
                                }
                                UChaosClothSharedSimConfig* pClothSharedSimConfig = pClothingAssetCommon->GetClothConfig<UChaosClothSharedSimConfig>();
                                if ( pClothSharedSimConfig )
                                {
                                    pClothSharedSimConfig->SubdivisionCount = 3;
                                }
                            }
#endif
                            pClothingAssetCommon->RefreshBoneMapping( pMesh );
                            pClothingAssetCommon->BuildLodTransitionData();
                            pClothingAssetCommon->InvalidateCachedData();
                            pClothingAssetCommon->MarkPackageDirty();
                            pClothingAssetCommon->PostEditChange();
                        }
                    }
                
                    NewClothingAsset->MarkPackageDirty();
                    NewClothingAsset->PostEditChange();
                }
#endif
            }

            pMesh->MarkPackageDirty();
            pMesh->PostEditChange();
        }
    }
#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 20 || ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 21 || ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION == 22

    if ( !IsRunningCommandlet() )
    {
        ISkeletalMeshEditorModule& kSkeletalMeshEditorModule = FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>( "SkeletalMeshEditor" );
        TSharedPtr<class IToolkitHost> spEditWithinLevelEditor = TSharedPtr<IToolkitHost>();
        if ( &kSkeletalMeshEditorModule )
        {
            TSharedRef<ISkeletalMeshEditor> kSkeletalMeshEditor = kSkeletalMeshEditorModule.CreateSkeletalMeshEditor( EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), pMesh );
            if ( &kSkeletalMeshEditor )
            {
                kSkeletalMeshEditor.Get().GetAssetEditorModeManager()->ActivateMode( "ClothPaintMode", true );
            }
        }
    }
#endif
    //cloth test-end
}

USkeleton* FRLPluginModule::GetAssetSkeleton( const FAssetData& kAssetData )
{
    FName strSkeleton( "Skeleton" );
    FString strSkeletonAddress;
    kAssetData.GetTagValue( strSkeleton, strSkeletonAddress );
    FString strSkeletonPath;
    if ( strSkeletonAddress.Len() == 0 )
    {
        strSkeletonPath = FPaths::GetPath( kAssetData.ObjectPath.ToString() );
        strSkeletonPath = strSkeletonPath + "/" + kAssetData.AssetName.ToString() + "_Skeleton." + kAssetData.AssetName.ToString() + "_Skeleton";
    }
    else
    {
        strSkeletonPath = strSkeletonPath + "/" + strSkeletonAddress;
    }
    USkeleton* pSkeleton = LoadObject< USkeleton >( nullptr, *strSkeletonPath );
    return pSkeleton;
}
