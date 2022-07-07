// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once


#include "DynamicLinks/DynamicLinks.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseDynamicLinksNodes.generated.h"

UCLASS()
class UFirebaseDynamicLinksBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Dynamic Links")
	FGeneratedDynamicLink GetLongLink(const FDynamicLinkComponents& Components);

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultOnDynamicLinkReceived, const FString&, URL, const ELinkMatchStrength, MatchStrength);

UCLASS()
class UBlueprintFirebaseDynamicLinksProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultOnDynamicLinkReceived OnDynamicLinkReceived;

public:
	/**
	 * Sets a callback wich will be called when a dynamic link event occurs.
	 * Don't call it twice.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Dynamic Links", meta = (BlueprintInternalUseOnly = "true", DisplayName = "On Dynamic Link Received"))
	static UBlueprintFirebaseDynamicLinksProxy* ListenOnDynamicLinkReceived();

	virtual void Activate();

private:
	UFUNCTION()
	void OnLink(const FString& URL, const ELinkMatchStrength MatchStrength);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynMultGetShortLink, const FGeneratedDynamicLink&, DynamicLink);

UCLASS()
class UDynamicLinksGetShortLinkBase: public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * The dynamic link has been generated.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultGetShortLink Success;

	/**
	 * An error occured while generating the dynamic link.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultGetShortLink Failed;

private:
	void OnTaskOver(bool bSuccess, const FGeneratedDynamicLink& Link);

protected:
	FDynamicLinksCallback GetCallback();
};

UCLASS()
class UDynamicLinksGetShortLinkWithComponents final : public UDynamicLinksGetShortLinkBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Firebase|Dynamic Links", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Short Link with Components"))
	static UDynamicLinksGetShortLinkWithComponents* GetShortLink(const FDynamicLinkComponents& Components);

	virtual void Activate() override final;

private:
	FDynamicLinkComponents Components;
};

UCLASS()
class UDynamicLinksGetShortLinkWithComponentsAndOptions final : public UDynamicLinksGetShortLinkBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Firebase|Dynamic Links", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Short Link with Components and Options"))
	static UDynamicLinksGetShortLinkWithComponentsAndOptions* GetShortLink(const FDynamicLinkComponents& Components, const FDynamicLinkOptions& Options);

	virtual void Activate() override final;

private:
	FDynamicLinkComponents Components;
	FDynamicLinkOptions Options;
};

UCLASS()
class UDynamicLinksGetShortLinkWithString final : public UDynamicLinksGetShortLinkBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Firebase|Dynamic Links", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Short Link"))
	static UDynamicLinksGetShortLinkWithString* GetShortLink(const FString& LongDynamicLink);

	virtual void Activate() override final;

private:
	FString LongDynamicLink;
};

UCLASS()
class UDynamicLinksGetShortLinkWithStringAndOptions final : public UDynamicLinksGetShortLinkBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Firebase|Dynamic Links", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Short Link with Options"))
	static UDynamicLinksGetShortLinkWithStringAndOptions* GetShortLink(const FString& LongDynamicLink, const FDynamicLinkOptions& Options);

	virtual void Activate() override final;

private:
	FString LongDynamicLink;
	FDynamicLinkOptions Options;
};


