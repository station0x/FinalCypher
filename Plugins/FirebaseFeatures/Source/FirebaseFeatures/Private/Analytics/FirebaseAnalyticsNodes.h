// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "FirebaseAnalyticsNodes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetAnalyticsInstanceIdCallback, const FString&, InstanceId);

UCLASS()
class UGetAnalyticsInstanceIdProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FGetAnalyticsInstanceIdCallback OnInstanceIdReceived;

public:
	UGetAnalyticsInstanceIdProxy();

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", meta = (BlueprintInternalUseOnly = "true"))
	static UGetAnalyticsInstanceIdProxy* GetAnalyticsInstanceId();

private:
	void OnActionOver(const int32 Error, const FString& InstanceId);
};


