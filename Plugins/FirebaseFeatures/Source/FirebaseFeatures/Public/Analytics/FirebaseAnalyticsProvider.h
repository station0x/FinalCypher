// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FirebaseFeatures.h"

#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	include "Interfaces/IAnalyticsProvider.h"
#endif // FIREBASE_FEATURES_UE_4_25_OR_NEWER

DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseAnalytics, Log, All);

#if !FIREBASE_FEATURES_UE_4_25_OR_NEWER
// Dummy struct for compatibility with previous UE versions
struct FIREBASEFEATURES_API FAnalyticsEventAttribute {};
#endif // FIREBASE_FEATURES_UE_4_25_OR_NEWER

#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	define FIREBASE_FEATURES_ANALYTICS_OVERRIDE override
#else // !FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	define FIREBASE_FEATURES_ANALYTICS_OVERRIDE
#endif // !FIREBASE_FEATURES_UE_4_25_OR_NEWER

class FIREBASEFEATURES_API FFirebaseAnalyticsProvider 
#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
	: public IAnalyticsProvider
#endif // FIREBASE_FEATURES_UE_4_25_OR_NEWER
{
public:
	FFirebaseAnalyticsProvider();
	virtual ~FFirebaseAnalyticsProvider();

	virtual bool	StartSession(const TArray<FAnalyticsEventAttribute>& Attributes = TArray<FAnalyticsEventAttribute>()) FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual void	EndSession() FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual void	RecordEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes = TArray<FAnalyticsEventAttribute>()) FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual FString GetSessionID() const FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual bool	SetSessionID(const FString& InSessionID) FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual void	FlushEvents() FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual void	SetUserID(const FString& InUserID) FIREBASE_FEATURES_ANALYTICS_OVERRIDE;
	virtual FString GetUserID() const FIREBASE_FEATURES_ANALYTICS_OVERRIDE;

	FORCEINLINE bool HasAnalyticSessionStarted()
	{
		return bAnalyticSessionStarted;
	}

private:
	bool bAnalyticSessionStarted;
};
