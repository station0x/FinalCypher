// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "FirebaseSdkNodes.h"

void UFirebaseSDKLibrary::CreateFirebaseApp(const FString& AppName, const FFirebaseAppOptions& Options, EFirebaseSDKCreateApp& Branches)
{
	auto* const App = FFirebaseFeaturesModule::CreateFirebaseApp(AppName, Options);

	Branches = (App ? EFirebaseSDKCreateApp::Success : EFirebaseSDKCreateApp::Failed);
}

void UFirebaseSDKLibrary::SwitchFirebaseApp(const FString& AppName, EFirebaseSDKCreateApp& Branches)
{
	Branches = (FFirebaseFeaturesModule::SwitchFirebaseApp(AppName) ? EFirebaseSDKCreateApp::Success : EFirebaseSDKCreateApp::Failed);
}

UFirebaseInitializationProxy* UFirebaseInitializationProxy::WaitForInitialization()
{
	return NewObject<ThisClass>();
}

void UFirebaseInitializationProxy::Activate()
{
	if (FFirebaseFeaturesModule::IsFirebaseSDKInitialized())
	{
		OnSdkInitialized();
	}
	else
	{
		FFirebaseFeaturesModule::OnSDKInitialized().AddUObject(this, &ThisClass::OnSdkInitialized);
	}
}

void UFirebaseInitializationProxy::OnSdkInitialized()
{
	FirebaseInitialized.Broadcast();
	SetReadyToDestroy();
}

