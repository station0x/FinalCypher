// Fill out your copyright notice in the Description page of Project Settings.
#define LODGROUP "_LODGroup"
#include "CCFbxFactory.h"
#include "RLPlugin.h"
#include "Containers/UnrealString.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/FeedbackContext.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Editor/EditorEngine.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxSkeletalMeshImportData.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "Factories/FbxTextureImportData.h"
#include "Factories/FbxImportUI.h"
#include "Engine/StaticMesh.h"
#include "Editor.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "EditorReimportHandler.h"
#include "Editor/UnrealEd/Classes/Factories/TextureFactory.h"
#include "Logging/TokenizedMessage.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"
//#include "SlateBasics.h"
#include "CCImportWindow.h"
#include "ARFilter.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "Commandlets/ImportAssetsCommandlet.h"
#include "Misc/FbxErrors.h"
#include "AssetRegistryModule.h"
#include "ObjectTools.h"
#include "HAL/FileManager.h"
#include "LODUtilities.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"

#define LOCTEXT_NAMESPACE "CCAutoSetup"

void RemoveSpecialCharToUnderline( FString& strString )
{
    TArray< const TCHAR* > kSpecialChar{ TEXT( " " ),
                                         TEXT( "(" ),
                                         TEXT( ")" ),
                                         TEXT( "." ) };
    for ( auto& strChar : kSpecialChar )
    {
        strString = strString.Replace( strChar, TEXT( "_" ), ESearchCase::IgnoreCase );
    }
}

void GetLodZeroOriginFbxName( const FString& strLodZeroName, FString& strFbxOriginName )
{
    strFbxOriginName = strLodZeroName;
    if ( strFbxOriginName.Contains( "LOD0" ) )
    {
        int nPosition = 0;
        if ( strFbxOriginName.FindLastChar( '_', nPosition ) )
        {
            int nCount = strFbxOriginName.Len() - nPosition;
            strFbxOriginName.RemoveAt( nPosition, nCount, true );
        }

        if ( strFbxOriginName.FindChar( '_', nPosition ) )
        {
            strFbxOriginName.RemoveAt( 0, nPosition, true );
        }
    }
}

UCCFbxFactory::UCCFbxFactory( const FObjectInitializer& ObjectInitializer ) : Super( ObjectInitializer )
{
    Formats.Add( FString( TEXT( "fbx;" ) ) + NSLOCTEXT( "fbx", "fbx", "Fbx File" ).ToString() );
    SupportedClass = NULL;
    bCreateNew = false;
    bEditorImport = true;
}

void UCCFbxFactory::PostInitProperties()
{
    Super::PostInitProperties();
}

UObject* UCCFbxFactory::FactoryCreateFile( UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, const FString& InFilename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled )
{
    FRLPluginModule kPluginModule;
    FString strOriginalFbxName = FPaths::GetBaseFilename( InFilename );
    FString fbxRootPath = FPaths::GetPath( InFilename ) + "\\";
    FString strJsonFilePath = fbxRootPath + strOriginalFbxName + ".json";
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    //------檢查版號------//
    if ( !FPaths::FileExists( strJsonFilePath ) || !kPluginModule.CheckAutoSetupVersionPass( strJsonFilePath ) )
    {
        UObject* fbxOb = UFbxFactory::FactoryCreateFile( Class, InParent, Name, Flags, InFilename, Parms, Warn, bOutOperationCanceled );
        return fbxOb;
    }

    UCCImportUI* pImportUI = NewObject<UCCImportUI>( this, NAME_None, RF_NoFlags );
    if ( !IsRunningCommandlet() )
    {
        FString strJsonConfig;
        FFileHelper::LoadFileToString( strJsonConfig, *strJsonFilePath );
        TSharedPtr<FJsonObject> kJsonObject;
        TSharedRef<TJsonReader<>> kReader = TJsonReaderFactory<>::Create( strJsonConfig );
        bool bSupportShaderSelect = true;
        if ( FJsonSerializer::Deserialize( kReader, kJsonObject ) )
        {
            TSharedPtr<FJsonObject> spObjectRoot = kJsonObject->GetObjectField( strOriginalFbxName )->GetObjectField( "Scene" );
            if ( spObjectRoot && spObjectRoot->HasField( "SupportShaderSelect" ) )
            {
                bSupportShaderSelect = spObjectRoot->GetBoolField( "SupportShaderSelect" );
            }
        }
        if ( bSupportShaderSelect )
        {
            TSharedPtr<SWindow> ParentWindow;
            if ( FModuleManager::Get().IsModuleLoaded( "MainFrame" ) )
            {
                IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>( "MainFrame" );
                ParentWindow = MainFrame.GetParentWindow();
            }
            TSharedRef<SWindow> Window = SNew( SWindow )
                .IsTopmostWindow( true )
                .Title( LOCTEXT( "CCAutoSetup_ImportWindowTitle", "Character Creator & iClone Auto Setup Import Options" ) )
                .SizingRule( ESizingRule::Autosized );

            TSharedPtr<SCCImportWindow> CCWindow;
            pImportUI->isCanChangeAutoEnable = true;

            FString shaderPath = FPaths::ProjectContentDir() + "CC_Shaders/";
            if ( !PlatformFile.DirectoryExists( *shaderPath ) )
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

            Window->SetContent
            (
                SAssignNew( CCWindow, SCCImportWindow )
                .ImportUI( pImportUI )
                .WidgetWindow( Window )
            );
            FSlateApplication::Get().AddModalWindow( Window, ParentWindow, false );

            if ( !pImportUI->isCanceled )
            {
                pImportUI->WriteConfig();
                pImportUI->SaveConfig();
            }
        }
        else
        {
            pImportUI->isCanChangeAutoEnable = true;
            pImportUI->isCCAutoSetup = true;
            pImportUI->isCanceled = false;

            FString shaderPath = FPaths::ProjectContentDir() + "CC_Shaders/";
            pImportUI->hasCCShader = PlatformFile.DirectoryExists( *shaderPath );
            pImportUI->isHQSkin = true;
            pImportUI->isLWSkin = false;
            pImportUI->isStandardSkin = false;
        }
    }
    else
    {
        FString strShaderJsonRootPath = FPaths::GetPath( InFilename ) + "\\" + "import_settings.json";
        FString strJsonConfig;
        FFileHelper::LoadFileToString( strJsonConfig, *strShaderJsonRootPath );
        TSharedPtr<FJsonObject> kJsonObject;
        TSharedRef<TJsonReader<>> kReader = TJsonReaderFactory<>::Create( strJsonConfig );

        if ( FJsonSerializer::Deserialize( kReader, kJsonObject ) )
        {
            TArray<TSharedPtr<FJsonValue>> kImportGroups = kJsonObject->GetArrayField( "ImportGroups" );
            FString ShaderType = kImportGroups[ 0 ]->AsObject()->GetStringField( "ShaderType" );
            SetShaderType( ShaderType );
        }
        //For LiveLink Import FBX
        pImportUI->isCanChangeAutoEnable = true;
        pImportUI->isCCAutoSetup = true;
        pImportUI->isCanceled = false;
        pImportUI->isLiveLink = true;

        FString shaderPath = FPaths::ProjectContentDir() + "CC_Shaders/";
        //IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        if ( !PlatformFile.DirectoryExists( *shaderPath ) )
        {
            pImportUI->hasCCShader = false;
            pImportUI->isHQSkin = false;
            pImportUI->isLWSkin = false;
            pImportUI->isStandardSkin = true;
        }
        else
        {
            pImportUI->hasCCShader = true;
            if ( m_ShaderType == "HQ" )
            {
                pImportUI->isHQSkin = true;
                pImportUI->isLWSkin = false;
                pImportUI->isStandardSkin = false;
            }
            else if ( m_ShaderType == "LW" )
            {
                pImportUI->isHQSkin = false;
                pImportUI->isLWSkin = true;
                pImportUI->isStandardSkin = false;
            }
            else
            {
                pImportUI->isHQSkin = false;
                pImportUI->isLWSkin = false;
                pImportUI->isStandardSkin = true;
            }
        }
        pImportUI->WriteConfig();
        pImportUI->SaveConfig();
    }

    UObject* pFbxObject = UFbxFactory::FactoryCreateFile( Class, InParent, Name, Flags, InFilename, Parms, Warn, bOutOperationCanceled );
    if ( !pFbxObject )
    {
        return pFbxObject;
    }

    UObject* pCheckObjectType = nullptr;
    pCheckObjectType = StaticFindObject( UObject::StaticClass(), InParent, *( Name.ToString() ) );
    bool bIsAnimation = false;
    if ( pCheckObjectType )
    {
        UAnimSequence* pCheckAnimSequence = Cast<UAnimSequence>( pCheckObjectType );
        if ( pCheckAnimSequence )
        {
            bIsAnimation = true;
        }
    }

    FString strJsonFileCopyPath = FPaths::ProjectSavedDir() + "JsonData/" + strOriginalFbxName + ".json";
    if ( PlatformFile.FileExists( *strJsonFileCopyPath ) )
    {
        PlatformFile.DeleteFile( *strJsonFileCopyPath );
    }
    FFileManagerGeneric::Get().Copy( *strJsonFileCopyPath, *strJsonFilePath );

    if ( !bOutOperationCanceled && !bIsAnimation && pImportUI->isCCAutoSetup && !pImportUI->isCanceled )
    {
        FString strRootGamePath = FPaths::GetPath( pFbxObject->GetPathName() );

        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" );
        TArray<FAssetData> kCheckAssetData;
        TArray<FAssetData> kFbxAssetData;
        FARFilter kCheckFilter;
        kCheckFilter.ClassNames.Add( USkeletalMesh::StaticClass()->GetFName() );
        kCheckFilter.ClassNames.Add( UStaticMesh::StaticClass()->GetFName() );
        kCheckFilter.PackagePaths.Add( *strRootGamePath );
        AssetRegistryModule.Get().GetAssets( kCheckFilter, kCheckAssetData );

        for( auto& kAssetData : kCheckAssetData )
        {
            if ( kAssetData.AssetName.ToString() == Name.ToString() )
            {
                kFbxAssetData.Add( kAssetData );
            }
        }
        TArray<FString> kTextureList;
        TArray < TCHAR* > kFileExtension{ _T( "*.png" ),
                                          _T( "*.jpg" ),
                                          _T( "*.tga" ),
                                          _T( "*.tif" ),
                                          _T( "*.exr" ),
                                          _T( "*.bmp" ) };

        ImportTextureFolder( fbxRootPath, strRootGamePath, kFileExtension, strOriginalFbxName, kTextureList );
        FString shaderType = " ";
        if ( pImportUI->isStandardSkin )
        {
            shaderType = "Standard";
        }
        else if ( pImportUI->isHQSkin )
        {
            shaderType = "HQ";
        }
        else if ( pImportUI->isLWSkin )
        {
            shaderType = "LW";
        }

        TArray<FString> LODPathList = GetLODPaths( fbxRootPath, strOriginalFbxName );
        kPluginModule.AutoSetup( kFbxAssetData, shaderType, LODPathList, true );
    }
    return pFbxObject;
}

void UCCFbxFactory::ImportTextureFolder( FString& fbxRootPath, FString& rootGamePath, TArray< TCHAR* > kFileExtension, FString &fbxName, TArray<FString> &kTextureList )
{
    TArray<FString> kFilePathList;
    kFilePathList.Empty();

    FString strTextureFbmPath = fbxRootPath + fbxName + ".fbm\\";
    FString texturePath = fbxRootPath + "textures\\";
    for ( auto& kExtension : kFileExtension )
    {
        FFileManagerGeneric::Get().FindFilesRecursive( kFilePathList, *strTextureFbmPath, kExtension, true, false, false );
        FFileManagerGeneric::Get().FindFilesRecursive( kFilePathList, *texturePath, kExtension, true, false, false );
    }

    FString fbxNameCheck;
    GetLodZeroOriginFbxName( fbxName, fbxNameCheck );

    UTextureFactory* pTextureFactory = NewObject< UTextureFactory >();

    FScopedSlowTask SlowTask( 100 );
    SlowTask.MakeDialog();
    SlowTask.EnterProgressFrame( 0 );

    for ( const FString& strFilePath : kFilePathList )
    {
        FString strFilePathCheck = strFilePath;
        strFilePathCheck.RemoveFromStart( fbxRootPath );

        bool bIsTextureFolder = strFilePathCheck.Contains( "fbm" ) || strFilePathCheck.Contains( "textures" );
        if ( !strFilePathCheck.Contains( fbxNameCheck ) && !bIsTextureFolder )
        {
            continue;
        }

        FString strTargetFilePath = strFilePath;
        strTargetFilePath.RemoveFromStart( fbxRootPath );
        strTargetFilePath = rootGamePath + "/" + strTargetFilePath;

        FString strFileName = FPaths::GetBaseFilename( strTargetFilePath );
        FString strPackageName = FPaths::GetPath( strTargetFilePath ) + "/" + strFileName;
        if ( !strFilePathCheck.Contains( "fbm" ) && strFilePathCheck.Contains( "textures" ) )
        {
            strPackageName = rootGamePath + "/textures/" + fbxName + "/" + strFileName;
        }

        RemoveSpecialCharToUnderline( strFileName );
        RemoveSpecialCharToUnderline( strPackageName );

        FString strSlowTaskstr = "Importing Texture : " + strFileName;
        SlowTask.EnterProgressFrame( 100 / ( kFilePathList.Num() ), FText::FromString( strSlowTaskstr ) );

        FString strTexturePathToLoad = strPackageName + "." + strFileName;
        if ( !kTextureList.Contains( FPaths::GetBaseFilename( strTexturePathToLoad ) ) )
        {
            kTextureList.Add( FPaths::GetBaseFilename( strTexturePathToLoad ) );
            TArray<uint8> RawData;
            if ( FFileHelper::LoadFileToArray( RawData, *strFilePath ) )
            {
                UPackage* AssetPackage = CreatePackage( NULL, *strPackageName );
                if ( !AssetPackage || !pTextureFactory )
                {
                    continue;
                }
                EObjectFlags Flags = RF_Public | RF_Standalone;
                RawData.Add( 0 );
                const uint8* Ptr = &RawData[ 0 ];

                try
                {
                    UObject* pTexAsset = pTextureFactory->FactoryCreateBinary( UTexture::StaticClass(), AssetPackage, FName( *strFileName ), Flags, NULL, *FPaths::GetExtension( strFilePath ), Ptr, Ptr + RawData.Num() - 1, GWarn );
                    if ( pTexAsset )
                    {
                        UTexture* pTexture = Cast<UTexture>( pTexAsset );
                        pTexture->SRGB = true;
                        pTexture->MarkPackageDirty();
                        ULevel::LevelDirtiedEvent.Broadcast();
                        pTexture->PostEditChange();
                        AssetPackage->SetDirtyFlag( true );
                        FAssetRegistryModule::AssetCreated( pTexture );
                    }
                }
                catch ( ... )
                {
                    UE_LOG( LogTemp, Warning, TEXT( "Create Texture Error." ) );
                }
            }
            RawData.Empty();
        }
    }
}

TArray<FString> UCCFbxFactory::GetLODPaths( FString &strTargetFolderPath, FString &strFbxName )
{
    TArray<FString> kFilePathList;
    TArray<FString> kLodPathList;
    kFilePathList.Empty();
    const TCHAR* strExtension = _T( "*.fbx" );
    FFileManagerGeneric::Get().FindFiles( kFilePathList, *strTargetFolderPath, strExtension );

    FString strFileName;
    GetLodZeroOriginFbxName( strFbxName, strFileName );

    for ( auto& strFilePath : kFilePathList )
    {
        if ( strFilePath.Contains( strFileName ) )
        {
            if ( !strFilePath.Contains( "LOD0" ) && ( strFilePath.Contains( "LOD" ) ) )
            {
                FString strLodPath = strTargetFolderPath + strFilePath;
                kLodPathList.Add( strLodPath );
            }
        }
    }
    return kLodPathList;
}

void UCCFbxFactory::SetShaderType( FString shaderType )
{
    m_ShaderType = "Standard";
    if( shaderType == "HQ" || shaderType == "LW" || shaderType == "Standard" )
    {
        m_ShaderType = shaderType;
    }
}

#undef LOCTEXT_NAMESPACE