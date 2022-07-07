// Copright Pandores Marketplace 2022. All Rights Reserved.

#include "DynamicLinks/DynamicLinks.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_DYNAMIC_LINKS
THIRD_PARTY_INCLUDES_START
#	include "firebase/dynamic_links/components.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Async/Async.h"

#if WITH_FIREBASE_DYNAMIC_LINKS
static FAndroidParameters Convert(const firebase::dynamic_links::AndroidParameters& Other)
{
	FAndroidParameters Param;

	Param.FallbackUrl    = UTF8_TO_TCHAR(Other.fallback_url);
	Param.MinimumVersion = Other.minimum_version;
	Param.PackageName    = UTF8_TO_TCHAR(Other.package_name);

	return Param;
}

static FDynamicLinkComponents Convert(const firebase::dynamic_links::DynamicLinkComponents& Other)
{
	FDynamicLinkComponents Comp;

	if (Other.android_parameters)
	{
		Comp.AndroidParameters = Convert(*Other.android_parameters);
	}

	if (Other.domain_uri_prefix)
	{
		Comp.DomainUriPrefix = UTF8_TO_TCHAR(Other.domain_uri_prefix);
	}

	if (Other.link)
	{
		Comp.Link = UTF8_TO_TCHAR(Other.link);
	}

	return Comp;
}

static firebase::dynamic_links::DynamicLinkOptions Convert(const FDynamicLinkOptions& Other)
{
	firebase::dynamic_links::DynamicLinkOptions Options;

	Options.path_length = (firebase::dynamic_links::PathLength)Other.PathLength;

	return Options;
}

static FGeneratedDynamicLink Convert(const firebase::dynamic_links::GeneratedDynamicLink& Other)
{
	FGeneratedDynamicLink Gen;

	Gen.Error = UTF8_TO_TCHAR(Other.error.c_str());
	Gen.Url   = UTF8_TO_TCHAR(Other.url  .c_str());

	Gen.Warnings.Reserve(Other.warnings.size());

	for (const auto& Warning : Other.warnings)
	{
		Gen.Warnings.Emplace(UTF8_TO_TCHAR(Warning.c_str()));
	}

	return Gen;
}
#endif // WITH_FIREBASE_DYNAMIC_LINKS

/**
 * Handles ressources allocated to convert a FDynamicLinkComponents to its 
 * raw type.
*/
#if WITH_FIREBASE_DYNAMIC_LINKS
struct FDynamicLinkComponentsHandler
{
	FDynamicLinkComponentsHandler() = delete;
	FDynamicLinkComponentsHandler(const FDynamicLinkComponentsHandler&)  = delete;
	FDynamicLinkComponentsHandler(FDynamicLinkComponentsHandler&& Other) = delete;

	FDynamicLinkComponentsHandler(const FDynamicLinkComponents& InComponents)
		: AndroidParameters				(MakeUnique<firebase::dynamic_links::AndroidParameters>())
		, GoogleAnalyticsParameters		(MakeUnique<firebase::dynamic_links::GoogleAnalyticsParameters>())
		, IOSParameters					(MakeUnique<firebase::dynamic_links::IOSParameters>())
		, ItunesConnectParameters		(MakeUnique<firebase::dynamic_links::ITunesConnectAnalyticsParameters>())
		, SocialMetaTagParameters		(MakeUnique<firebase::dynamic_links::SocialMetaTagParameters>())

		, DomainUriPrefix	(TCHAR_TO_UTF8(*InComponents.DomainUriPrefix))
		, Link				(TCHAR_TO_UTF8(*InComponents.Link))

		, AndroidFallbackURL(TCHAR_TO_UTF8(*InComponents.AndroidParameters.FallbackUrl))
		, AndroidPackageName(TCHAR_TO_UTF8(*InComponents.AndroidParameters.PackageName))

		, GoogleCampaign	(TCHAR_TO_UTF8(*InComponents.GoogleAnalyticsParameters.Campaign))
		, GoogleContent		(TCHAR_TO_UTF8(*InComponents.GoogleAnalyticsParameters.Content))
		, GoogleMedium		(TCHAR_TO_UTF8(*InComponents.GoogleAnalyticsParameters.Medium))
		, GoogleSource		(TCHAR_TO_UTF8(*InComponents.GoogleAnalyticsParameters.Source))
		, GoogleTerm		(TCHAR_TO_UTF8(*InComponents.GoogleAnalyticsParameters.Term))

		, IOSAppID				(TCHAR_TO_UTF8(*InComponents.IOSParameters.AppStoreId))
		, IOSBundleID			(TCHAR_TO_UTF8(*InComponents.IOSParameters.BundleId))
		, IOSCustomScheme		(TCHAR_TO_UTF8(*InComponents.IOSParameters.CustomScheme))
		, IOSFallbackURL		(TCHAR_TO_UTF8(*InComponents.IOSParameters.FallbackUrl))
		, IOSIpadBundleID		(TCHAR_TO_UTF8(*InComponents.IOSParameters.IpadBundleId))
		, IOSIpabFallbackURL	(TCHAR_TO_UTF8(*InComponents.IOSParameters.IpadFallbackUrl))
		, IOSMinimumVersion		(TCHAR_TO_UTF8(*InComponents.IOSParameters.MinimumVersion))

		, ItunesAffiliateToken (TCHAR_TO_UTF8(*InComponents.ItunesConnectAnalyticsParameters.AffiliateToken))
		, ItunesCampaignToken  (TCHAR_TO_UTF8(*InComponents.ItunesConnectAnalyticsParameters.CampaignToken))
		, ItunesProviderToken  (TCHAR_TO_UTF8(*InComponents.ItunesConnectAnalyticsParameters.ProviderToken))

		, SocialDescription	(TCHAR_TO_UTF8(*InComponents.SocialMetaTagParameters.Description))
		, SocialImageURL	(TCHAR_TO_UTF8(*InComponents.SocialMetaTagParameters.ImageUrl))
		, SocialTitle		(TCHAR_TO_UTF8(*InComponents.SocialMetaTagParameters.Title))
	{
		Components.android_parameters					= AndroidParameters			.Get();
		Components.google_analytics_parameters			= GoogleAnalyticsParameters	.Get();
		Components.ios_parameters						= IOSParameters				.Get();
		Components.itunes_connect_analytics_parameters	= ItunesConnectParameters	.Get();
		Components.social_meta_tag_parameters			= SocialMetaTagParameters	.Get();

		Components.domain_uri_prefix	= DomainUriPrefix	.c_str();
		Components.link					= Link				.c_str();

		Components.android_parameters->fallback_url    = AndroidFallbackURL.c_str();
		Components.android_parameters->minimum_version = InComponents.AndroidParameters.MinimumVersion;
		Components.android_parameters->package_name    = AndroidPackageName.c_str();
		
		Components.google_analytics_parameters->campaign = GoogleCampaign.c_str();
		Components.google_analytics_parameters->content  = GoogleContent .c_str();
		Components.google_analytics_parameters->medium   = GoogleMedium  .c_str();
		Components.google_analytics_parameters->source   = GoogleSource  .c_str();
		Components.google_analytics_parameters->term     = GoogleTerm    .c_str();

		Components.ios_parameters->app_store_id			= IOSAppID			.c_str();
		Components.ios_parameters->bundle_id			= IOSBundleID		.c_str();
		Components.ios_parameters->custom_scheme		= IOSCustomScheme	.c_str();
		Components.ios_parameters->fallback_url			= IOSFallbackURL	.c_str();
		Components.ios_parameters->ipad_bundle_id		= IOSIpadBundleID	.c_str();
		Components.ios_parameters->ipad_fallback_url	= IOSIpabFallbackURL.c_str();
		Components.ios_parameters->minimum_version		= IOSMinimumVersion	.c_str();

		Components.itunes_connect_analytics_parameters->affiliate_token = ItunesAffiliateToken.c_str();
		Components.itunes_connect_analytics_parameters->campaign_token  = ItunesCampaignToken .c_str();
		Components.itunes_connect_analytics_parameters->provider_token	= ItunesProviderToken .c_str();

		Components.social_meta_tag_parameters->description	= SocialDescription.c_str();
		Components.social_meta_tag_parameters->image_url	= SocialImageURL   .c_str();
		Components.social_meta_tag_parameters->title		= SocialTitle      .c_str();
	}

	~FDynamicLinkComponentsHandler() = default;

	firebase::dynamic_links::DynamicLinkComponents& Get()
	{
		return Components;
	}

private:
	const TUniquePtr<firebase::dynamic_links::AndroidParameters>				AndroidParameters;
	const TUniquePtr<firebase::dynamic_links::GoogleAnalyticsParameters>		GoogleAnalyticsParameters;
	const TUniquePtr<firebase::dynamic_links::IOSParameters>					IOSParameters;
	const TUniquePtr<firebase::dynamic_links::ITunesConnectAnalyticsParameters>	ItunesConnectParameters;
	const TUniquePtr<firebase::dynamic_links::SocialMetaTagParameters>			SocialMetaTagParameters;

	const std::string DomainUriPrefix;
	const std::string Link;

	const std::string AndroidFallbackURL;
	const std::string AndroidPackageName;

	const std::string GoogleCampaign;
	const std::string GoogleContent;
	const std::string GoogleMedium;
	const std::string GoogleSource;
	const std::string GoogleTerm;

	const std::string IOSAppID;
	const std::string IOSBundleID;
	const std::string IOSCustomScheme;
	const std::string IOSFallbackURL;
	const std::string IOSIpadBundleID;
	const std::string IOSIpabFallbackURL;
	const std::string IOSMinimumVersion;

	const std::string ItunesAffiliateToken;
	const std::string ItunesCampaignToken;
	const std::string ItunesProviderToken;

	const std::string SocialDescription;
	const std::string SocialImageURL;
	const std::string SocialTitle;

private:
	firebase::dynamic_links::DynamicLinkComponents Components;
};
#endif

FGoogleAnalyticsParameters::FGoogleAnalyticsParameters(const FGoogleAnalyticsParameters& Other)
{
	*this = Other;
}

FGoogleAnalyticsParameters::FGoogleAnalyticsParameters(FGoogleAnalyticsParameters&& Other)
{
	*this = MoveTemp(Other);
}

FGoogleAnalyticsParameters& FGoogleAnalyticsParameters::operator=(const FGoogleAnalyticsParameters& Other)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	Source		= (Other.Source);
	Medium		= (Other.Medium);
	Campaign	= (Other.Campaign);
	Content		= (Other.Content);
	Term		= (Other.Term);
#endif

	return *this;
}

FGoogleAnalyticsParameters& FGoogleAnalyticsParameters::operator=(FGoogleAnalyticsParameters&& Other)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	Source		= MoveTemp(Other.Source);
	Medium		= MoveTemp(Other.Medium);
	Campaign	= MoveTemp(Other.Campaign);
	Content		= MoveTemp(Other.Content);
	Term		= MoveTemp(Other.Term);
#endif

	return *this;
}


FGeneratedDynamicLink FDynamicLinksLibrary::GetLongLink(const FDynamicLinkComponents& Components)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	FDynamicLinkComponentsHandler Handler(Components);
	return Convert(firebase::dynamic_links::GetLongLink(Handler.Get()));
#else
	return {};
#endif
}

#if WITH_FIREBASE_DYNAMIC_LINKS
static void SetupGetShortLinkCallback(TSharedPtr<FDynamicLinkComponentsHandler>&& Handler, FDynamicLinksCallback&& Callback)
{
	firebase::dynamic_links::GetShortLinkLastResult().OnCompletion([
		Handler  = MoveTemp(Handler), // Handler outlives the call.
		Callback = MoveTemp(Callback)
	]
	(const firebase::Future<firebase::dynamic_links::GeneratedDynamicLink>& Future) mutable -> void
	{
		const bool bSuccess = Future.error() == 0;

		if (!bSuccess)
		{
			UE_LOG(LogFirebaseDynamicLinks, Error, TEXT("GetShortLink() failed. Reason: code %d, %s"),
				Future.error(), UTF8_TO_TCHAR(Future.error_message()));
		}

		FGeneratedDynamicLink Result;

		if (Future.result())
		{
			Result = Convert(*Future.result());
		}

		AsyncTask(ENamedThreads::GameThread, [bSuccess, Callback = MoveTemp(Callback), Result = MoveTemp(Result)]() -> void
		{
			Callback.ExecuteIfBound(bSuccess, Result);
		});
	});
}
#endif

#if WITH_FIREBASE_DYNAMIC_LINKS
static void SetupGetShortLinkCallback(FDynamicLinksCallback&& Callback)
{
	firebase::dynamic_links::GetShortLinkLastResult().OnCompletion([
			Callback = MoveTemp(Callback)
	]
	(const firebase::Future<firebase::dynamic_links::GeneratedDynamicLink>& Future) mutable -> void
		{
			const bool bSuccess = Future.error() == 0;

			if (!bSuccess)
			{
				UE_LOG(LogFirebaseDynamicLinks, Error, TEXT("GetShortLink() failed. Reason: code %d, %s"),
					Future.error(), UTF8_TO_TCHAR(Future.error_message()));
			}

			FGeneratedDynamicLink Result;

			if (Future.result())
			{
				Result = Convert(*Future.result());
			}

			AsyncTask(ENamedThreads::GameThread, [bSuccess, Callback = MoveTemp(Callback), Result = MoveTemp(Result)]() -> void
			{
				Callback.ExecuteIfBound(bSuccess, Result);
			});
		});
}
#endif

void FDynamicLinksLibrary::GetShortLink(const FDynamicLinkComponents& Components, FDynamicLinksCallback Callback)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	// Shared because FB uses std::function which must be copiable.
	TSharedPtr<FDynamicLinkComponentsHandler> Handler = MakeShared<FDynamicLinkComponentsHandler>(Components);

	firebase::dynamic_links::GetShortLink(Handler->Get());

	SetupGetShortLinkCallback(MoveTemp(Handler), MoveTemp(Callback));
#endif
}

void FDynamicLinksLibrary::GetShortLink(const FDynamicLinkComponents& Components, const FDynamicLinkOptions& Options, FDynamicLinksCallback Callback)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	// Shared because FB uses std::function which must be copiable.
	TSharedPtr<FDynamicLinkComponentsHandler> Handler = MakeShared<FDynamicLinkComponentsHandler>(Components);
	
	firebase::dynamic_links::GetShortLink(Handler->Get(), Convert(Options));

	SetupGetShortLinkCallback(MoveTemp(Handler), MoveTemp(Callback));
#endif
}

void FDynamicLinksLibrary::GetShortLink(const FString& LongDynamicLink, FDynamicLinksCallback Callback)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	firebase::dynamic_links::GetShortLink(TCHAR_TO_UTF8(*LongDynamicLink));

	SetupGetShortLinkCallback(MoveTemp(Callback));
#endif
}

void FDynamicLinksLibrary::GetShortLink(const FString& LongDynamicLink, const FDynamicLinkOptions& Options, FDynamicLinksCallback Callback)
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	firebase::dynamic_links::GetShortLink(TCHAR_TO_UTF8(*LongDynamicLink), Convert(Options));

	SetupGetShortLinkCallback(MoveTemp(Callback));
#endif
}
