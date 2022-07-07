// Copright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DynamicLinks.generated.h"

/// @brief Google Analytics Parameters.
///
/// Note that the strings used by the struct are not copied, as so must
/// either be statically allocated, or must persist in memory until the
/// DynamicLinkComponents that uses them goes out of scope.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FGoogleAnalyticsParameters 
{
    GENERATED_BODY()
public:
    FGoogleAnalyticsParameters() = default;

    FGoogleAnalyticsParameters(const FGoogleAnalyticsParameters&);
    FGoogleAnalyticsParameters(FGoogleAnalyticsParameters&&);

    FGoogleAnalyticsParameters& operator=(const FGoogleAnalyticsParameters&);
    FGoogleAnalyticsParameters& operator=(FGoogleAnalyticsParameters&&);

    /// The campaign source; used to identify a search engine, newsletter,
    /// or other source.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Source;
    /// The campaign medium; used to identify a medium such as email or
    /// cost-per-click (cpc).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Medium;
    /// The campaign name; The individual campaign name, slogan, promo code, etc.
    /// for a product.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Campaign;
    /// The campaign term; used with paid search to supply the keywords for ads.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Term;
    /// The campaign content; used for A/B testing and content-targeted ads to
    /// differentiate ads or links that point to the same URL.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Content;
};

/// @brief iOS Parameters.
///
/// Note that the strings used by the struct are not copied, as so must
/// either be statically allocated, or must persist in memory until the
/// DynamicLinkComponents that uses them goes out of scope.

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FIOSParameters 
{
    GENERATED_BODY()
public:
    /// The parameters ID of the iOS app to use to open the link. The app must be
    /// connected to your project from the Overview page of the Firebase console.
    /// Note this field is required.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString BundleId;
    /// The link to open on iOS if the app is not installed.
    ///
    /// Specify this to do something other than install your app from the
    /// App Store when the app isn't installed, such as open the mobile
    /// web version of the content, or display a promotional page for your app.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString FallbackUrl;
    /// The app's custom URL scheme, if defined to be something other than your
    /// app's parameters ID.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString CustomScheme;
    /// The link to open on iPad if the app is not installed.
    ///
    /// Overrides fallback_url when on iPad.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString IpadFallbackUrl;
    /// The iPad parameters ID of the app.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString IpadBundleId;
    /// The App Store ID, used to send users to the App Store when the app isn't
    /// installed.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString AppStoreId;
    /// The minimum version of your app that can open the link.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString MinimumVersion;
};

/// @brief iTunes Connect App Analytics Parameters.
///
/// Note that the strings used by the struct are not copied, as so must
/// either be statically allocated, or must persist in memory until the
/// DynamicLinkComponents that uses them goes out of scope.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FITunesConnectAnalyticsParameters 
{
    GENERATED_BODY()
public:
    /// The provider token that enables analytics for Dynamic Links from
    /// within iTunes Connect.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString ProviderToken;
    /// The affiliate token used to create affiliate-coded links.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString AffiliateToken;
    /// The campaign token that developers can add to any link in order to
    /// track sales from a specific marketing campaign.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString CampaignToken;
};

/// @brief Android Parameters.
///
/// Note that the strings used by the struct are not copied, as so must
/// either be statically allocated, or must persist in memory until the
/// DynamicLinkComponents that uses them goes out of scope.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FAndroidParameters 
{
    GENERATED_BODY()
public:
    /// Constructs a set of Android parameters with the given package name.
    ///
    /// The package name of the Android app to use to open the link.
    FAndroidParameters(FString InPackageName)
        : PackageName(MoveTemp(InPackageName)),
        MinimumVersion(0) {}

    /// Constructs an empty set of Android parameters.
    FAndroidParameters()
        : MinimumVersion(0) {}

    /// The package name of the Android app to use to open the link. The app
    /// must be connected to your project from the Overview page of the Firebase
    /// console.
    /// Note this field is required.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString PackageName;
    /// The link to open when the app isn't installed.
    ///
    /// Specify this to do something other than install your app from the
    /// Play Store when the app isn't installed, such as open the mobile web
    /// version of the content, or display a promotional page for your app.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString FallbackUrl;
    /// The versionCode of the minimum version of your app that can open the link.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    int MinimumVersion;
};

/// @brief Social meta-tag Parameters.
///
/// Note that the strings used by the struct are not copied, as so must
/// either be statically allocated, or must persist in memory until the
/// DynamicLinkComponents that uses them goes out of scope.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FSocialMetaTagParameters
{
    GENERATED_BODY()
public:
    /// The title to use when the Dynamic Link is shared in a social post.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Title;
    /// The description to use when the Dynamic Link is shared in a social post.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Description;
    /// The URL to an image related to this link.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString ImageUrl;
};

/// @brief The desired path length for shortened Dynamic Link URLs.
UENUM(BlueprintType)
enum class EPathLength : uint8 
{
    /// Uses the server-default for the path length.
    /// See https://goo.gl/8yDAqC for more information.
    PathLengthDefault = 0,
    /// Typical short link for non-sensitive links.
    PathLengthShort,
    /// Short link that uses a very long path to make it more difficult to
    /// guess. Useful for sensitive links.
    PathLengthUnguessable,
};

/// @brief Additional options for Dynamic Link creation.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FDynamicLinkOptions
{
    GENERATED_BODY()
public:
    /// The desired path length for shortened Dynamic Link URLs.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    EPathLength PathLength = EPathLength::PathLengthDefault;
};

/// @brief The returned value from creating a Dynamic Link.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FGeneratedDynamicLink
{
    GENERATED_BODY()
public:
    /// The Dynamic Link value.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Url;
    /// Information about potential warnings on link creation.
    ///
    /// Usually presence of warnings means parameter format errors, parameter
    /// value errors, or missing parameters.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    TArray<FString> Warnings;
    /// If non-empty, the cause of the Dynamic Link generation failure.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Error;
};

/// @brief The information needed to generate a Dynamic Link.
///
/// Note that the strings used by the struct are not copied, as so must
/// either be statically allocated, or must persist in memory until this
/// struct goes out of scope.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FDynamicLinkComponents
{
    GENERATED_BODY()
public:
    /// The link your app will open.
    /// You can specify any URL your app can handle, such as a link to your
    /// app's content, or a URL that initiates some
    /// app-specific logic such as crediting the user with a coupon, or
    /// displaying a specific welcome screen. This link must be a well-formatted
    /// URL, be properly URL-encoded, and use the HTTP or HTTPS scheme.
    /// Note, this field is required.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString Link;

    /// The domain (of the form "https://xyz.app.goo.gl") to use for this Dynamic
    /// Link. You can find this value in the Dynamic Links section of the Firebase
    /// console.
    ///
    /// If you have set up custom domains on your project, set this to your
    /// project's custom domain as listed in the Firebase console.
    ///
    /// Only https:// links are supported.
    ///
    /// Note, this field is required.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FString DomainUriPrefix;
    /// The Google Analytics parameters.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FGoogleAnalyticsParameters GoogleAnalyticsParameters;
    /// The iOS parameters.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FIOSParameters IOSParameters;
    /// The iTunes Connect App Analytics parameters.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FITunesConnectAnalyticsParameters ItunesConnectAnalyticsParameters;
    /// The Android parameters.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FAndroidParameters AndroidParameters;
    /// The social meta-tag parameters.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|DynamicLinks")
    FSocialMetaTagParameters SocialMetaTagParameters;
};

DECLARE_DELEGATE_TwoParams(
    FDynamicLinksCallback,
        bool /* bSuccess */,
        const FGeneratedDynamicLink& /* DynamicLink */
);

class FIREBASEFEATURES_API FDynamicLinksLibrary
{
    FDynamicLinksLibrary() = delete;

public:
    /// Creates a long Dynamic Link from the given parameters.
    static FGeneratedDynamicLink GetLongLink(const FDynamicLinkComponents& Components);

    /// Creates a shortened Dynamic Link from the given parameters.
    /// @param components: Settings used to configure the behavior for the link.
    static void GetShortLink(const FDynamicLinkComponents& Components, FDynamicLinksCallback Callback);

    /// Creates a shortened Dynamic Link from the given parameters.
    /// @param components: Settings used to configure the behavior for the link.
    /// @param options: Additional options for Dynamic Link shortening, indicating
    /// whether or not to produce an unguessable or shortest possible link.
    /// No references to the options object will be retained after the call.
    static void GetShortLink(const FDynamicLinkComponents& Components, const FDynamicLinkOptions& Options, FDynamicLinksCallback Callback);

    /// Creates a shortened Dynamic Link from a given long Dynamic Link.
    /// @param long_dynamic_link A link previously generated from GetLongLink.
    static void GetShortLink(const FString& LongDynamicLink, FDynamicLinksCallback Callback);

    /// Creates a shortened Dynamic Link from a given long Dynamic Link.
    /// @param long_dynamic_link: A link previously generated from GetLongLink.
    /// @param options: Additional options for Dynamic Link shortening, indicating
    /// whether or not to produce an unguessable or shortest possible link.
    /// No references to the options object will be retained after the call.
    static void GetShortLink(const FString& LongDynamicLink, const FDynamicLinkOptions& Options, FDynamicLinksCallback Callback);
};

