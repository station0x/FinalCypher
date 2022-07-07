// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseApiConfig.generated.h"

UCLASS(config=Engine, DefaultConfig)
class UFirebaseApiConfig : public UObject
{
	GENERATED_BODY()
public:
	
	UPROPERTY(config)
	FString GoogleServicesJson;
};

