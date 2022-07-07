// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Auth/Credential.h"

#include "FirebaseFeatures.h"

#if WITH_FIREBASE_AUTH
THIRD_PARTY_INCLUDES_START
#	include "firebase/app.h"
#	include "firebase/future.h"
#	include "firebase/auth.h"
#	include "firebase/auth/credential.h"
THIRD_PARTY_INCLUDES_END
#	if PLATFORM_IOS
#		import <AuthenticationServices/AuthenticationServices.h>
#	endif
#endif

#if PLATFORM_IOS
#	import <GameKit/GameKit.h>
#	include "IOS/IOSAppDelegate.h"
#	include <CommonCrypto/CommonDigest.h>
#endif

#include "Async/Async.h"


#if WITH_FIREBASE_AUTH
class FPhoneProviderListener final : public firebase::auth::PhoneAuthProvider::Listener
{
public:
	typedef firebase::auth::PhoneAuthProvider::ForceResendingToken TForceResendingToken;

public:
	FPhoneProviderListener
	(
		const FCredentialCallback&					CredCb,
		const FCodeAutoRetrievalTimeoutCallback&	RetrCb,
		const FVerificationFailedCallback&			VerifCb,
		const FCodeSentCallback&					CodeCb
	)
		: Cred  (CredCb)
		, Retr  (RetrCb)
		, Verif (VerifCb)
		, Code  (CodeCb)
	{	
	}
	
	virtual ~FPhoneProviderListener()
	{
	}
	
	virtual void OnVerificationCompleted(firebase::auth::Credential credential)
	{
		UE_LOG(LogFirebaseAuth, Log, TEXT("Phone provider verification completed."));

		FCredentialCallback Cb = Cred;
		FCredential Data(&credential);

		AsyncTask(ENamedThreads::GameThread, [Cb, Data = MoveTemp(Data)]() -> void
		{
			Cb.ExecuteIfBound(Data);
		});
	}

	virtual void OnVerificationFailed(const std::string& error)
	{
		const FString Error = UTF8_TO_TCHAR(error.c_str());
		UE_LOG(LogFirebaseAuth, Log, TEXT("Phone provider verification failed. Error: %s"), *Error);

		FVerificationFailedCallback Cb = Verif;

		AsyncTask(ENamedThreads::GameThread, [Cb, Error]() -> void
		{
			Cb.ExecuteIfBound(Error);
		});
	}

	virtual void OnCodeSent(const std::string& verification_id, const TForceResendingToken& force_resending_token)
	{
		UE_LOG(LogFirebaseAuth, Log, TEXT("Code for phone provider sent."));
		
		FForceResendingToken Token((void*)&force_resending_token);
		FString VerifId = UTF8_TO_TCHAR(verification_id.c_str());

		FCodeSentCallback Cb = Code;
		AsyncTask(ENamedThreads::GameThread, [Cb, Token, VerifId = MoveTemp(VerifId)]() -> void
		{
			Cb.ExecuteIfBound(VerifId, Token);
		});
	}

	virtual void OnCodeAutoRetrievalTimeOut(const std::string& verification_id)
	{
		UE_LOG(LogFirebaseAuth, Log, TEXT("Code auto retrieval timeout for phone provider reached."));

		FCodeAutoRetrievalTimeoutCallback Cb = Retr;
		FString VerifId = UTF8_TO_TCHAR(verification_id.c_str());

		AsyncTask(ENamedThreads::GameThread, [Cb, VerifId = MoveTemp(VerifId)]() -> void
		{
			Cb.ExecuteIfBound(VerifId);
		});
	}

private:
	FCredentialCallback					Cred;
	FCodeAutoRetrievalTimeoutCallback	Retr;
	FVerificationFailedCallback			Verif;
	FCodeSentCallback					Code;
};
#endif

FForceResendingToken::FForceResendingToken() 
	: Token(nullptr)
{
}

FForceResendingToken::FForceResendingToken(void* const InToken)
{
#if WITH_FIREBASE_AUTH
	Token = InToken ? new FPhoneProviderListener::TForceResendingToken(*(FPhoneProviderListener::TForceResendingToken*)InToken) : nullptr;
#endif
}

FForceResendingToken::FForceResendingToken(const FForceResendingToken& Other)
{
#if WITH_FIREBASE_AUTH
	Token = Other.Token ? new FPhoneProviderListener::TForceResendingToken(*(FPhoneProviderListener::TForceResendingToken*)Other.Token) : nullptr;
#endif
}

FForceResendingToken::FForceResendingToken(FForceResendingToken&& Other)
{
	Token = Other.Token;

	Other.Token = nullptr;
}

FForceResendingToken& FForceResendingToken::operator=(const FForceResendingToken& Other)
{
#if WITH_FIREBASE_AUTH
	if (Token && Other.Token)
	{
		*((FPhoneProviderListener::TForceResendingToken*)Token) = *((FPhoneProviderListener::TForceResendingToken*)Other.Token);
	}
	else if (Other.Token)
	{
		Token = new FPhoneProviderListener::TForceResendingToken(*(FPhoneProviderListener::TForceResendingToken*)Other.Token);
	}
	else
	{
		delete (FPhoneProviderListener::TForceResendingToken*)Token;
		Token = nullptr;
	}
#endif
	return *this;
}

FForceResendingToken::~FForceResendingToken()
{
#if WITH_FIREBASE_AUTH
	if (Token)
	{
		delete (FPhoneProviderListener::TForceResendingToken*)Token;
	}
#endif
}

FCredential UCredentialLibrary::GetCredentialFromEmail(const FString& Email, const FString& Password)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	const firebase::auth::Credential RawCredential = firebase::auth::EmailAuthProvider::GetCredential(TCHAR_TO_UTF8(*Email), TCHAR_TO_UTF8(*Password));

	*Credential.Credential = RawCredential;
#endif

	return Credential;
}

FCredential UCredentialLibrary::GetCredentialFromFacebook(const FString& AccessToken)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	*Credential.Credential = firebase::auth::FacebookAuthProvider::GetCredential(TCHAR_TO_UTF8(*AccessToken));
#endif

	return Credential;
}

#if PLATFORM_IOS && WITH_FIREBASE_AUTH
static void GetGameCenterCredentials(FGetCredentialCallback&& Callback)
{
	firebase::auth::GameCenterAuthProvider::GetCredential().OnCompletion([Callback = MoveTemp(Callback)]
	(const firebase::Future<firebase::auth::Credential>& Future) -> void
	{
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();
		if (Error != EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to get credential from Game Center. Code: %d. Message: %s"), 
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		FCredential Credential = Future.result();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Credential = MoveTemp(Credential)]() -> void
		{
			Callback.ExecuteIfBound(Error, Credential);
		});
	});
}
#endif

void UCredentialLibrary::GetCredentialFromGameCenter(FGetCredentialCallback Callback)
{
#if PLATFORM_IOS && WITH_FIREBASE_AUTH
	if ([GKLocalPlayer localPlayer].authenticateHandler != nil)
	{
		GetGameCenterCredentials(MoveTemp(Callback));
		return;
	}

	[GKLocalPlayer localPlayer].authenticateHandler = [Callback = MoveTemp(Callback)](UIViewController* gcAuthViewController, NSError* error) mutable -> void
	{
		if (gcAuthViewController != nil) 
		{
			[[IOSAppDelegate GetDelegate].IOSController 
				presentViewController: gcAuthViewController
				             animated: YES 
						   completion: nil];
		}
		else if ([GKLocalPlayer localPlayer].isAuthenticated)
		{
			GetGameCenterCredentials(MoveTemp(Callback));
		}
		else 
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to get credentials from Game Center: %d - %s"), error.code, *FString(error.description));
			AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(Callback)]() -> void
			{
				Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
			});
		}
	};
#else // !PLATFORM_IOS && !PLATFORM_MAC
	UE_LOG(LogFirebaseAuth, Error, TEXT("Tried to get credential from Game Center but the platform doesn't support it."));
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, FCredential());
#endif // !PLATFORM_IOS && !PLATFORM_MAC
}

bool UCredentialLibrary::IsAuthenticatedWithGameCenter()
{
#if WITH_FIREBASE_AUTH
	return firebase::auth::GameCenterAuthProvider::IsPlayerAuthenticated();
#else
	return false;
#endif
}

FCredential UCredentialLibrary::GetCredentialFromGitHub(const FString& Token)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	*Credential.Credential = firebase::auth::GitHubAuthProvider::GetCredential(TCHAR_TO_UTF8(*Token));
#endif

	return Credential;
}

FCredential UCredentialLibrary::GetCredentialFromGoogle(const FString& Token, const FString& AccessToken)
{
	FCredential Credential;
#if WITH_FIREBASE_AUTH
	*Credential.Credential = firebase::auth::GoogleAuthProvider::GetCredential(TCHAR_TO_UTF8(*Token), TCHAR_TO_UTF8(*AccessToken));;
#endif

	return Credential;
}

FCredential UCredentialLibrary::GetCredentialFromOAuth(const FString& ProviderId, const FString& IdToken, const FString& AccessToken)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	*Credential.Credential = firebase::auth::OAuthProvider::GetCredential(TCHAR_TO_UTF8(*ProviderId), TCHAR_TO_UTF8(*IdToken), TCHAR_TO_UTF8(*AccessToken));;
#endif

	return Credential;
}

FCredential UCredentialLibrary::GetCredentialFromOAuthWithOnce(const FString& ProviderId, const FString& IdToken, const FString& Nonce, const FString& AccessToken)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	*Credential.Credential = firebase::auth::OAuthProvider::GetCredential(TCHAR_TO_UTF8(*ProviderId), TCHAR_TO_UTF8(*IdToken), TCHAR_TO_UTF8(*Nonce), TCHAR_TO_UTF8(*AccessToken));;
#endif

	return Credential;
}

FCredential UCredentialLibrary::GetCredentialFromPlayGames(const FString& ServerAuthCode)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	if (ServerAuthCode.IsEmpty())
	{
		UE_LOG(LogFirebaseAuth, Warning, TEXT("Tried to create Google Play Games credentials with an empty ServerAuthCode."));
	}
	else
	{
		UE_LOG(LogFirebaseAuth, Log, TEXT("Credentials for Google Play Games created."));
	}
	
	*Credential.Credential = firebase::auth::PlayGamesAuthProvider::GetCredential(TCHAR_TO_UTF8(*ServerAuthCode));
#endif

	return Credential;
}

void UCredentialLibrary::VerifyPhoneNumber
(
	const FString&							PhoneNumber,
	const int64								AutoVerifyTimeoutMilliseconds,
	const FCredentialCallback&				OnVerificationCompleted,
	const FVerificationFailedCallback&		OnVerificationFailed,
	const FCodeSentCallback&				OnCodeSent,
	const FCodeAutoRetrievalTimeoutCallback OnCodeAutoRetrievalTimeout,
	const FForceResendingToken&				ForceResendingToken
)
{
#if WITH_FIREBASE_AUTH
	firebase::auth::PhoneAuthProvider& Provider = firebase::auth::PhoneAuthProvider::GetInstance(firebase::auth::Auth::GetAuth(FFirebaseFeaturesModule::GetCurrentFirebaseApp()));

	FPhoneProviderListener* const Listener = new FPhoneProviderListener(OnVerificationCompleted, OnCodeAutoRetrievalTimeout, OnVerificationFailed, OnCodeSent);
	Provider.VerifyPhoneNumber(TCHAR_TO_UTF8(*PhoneNumber), AutoVerifyTimeoutMilliseconds, (FPhoneProviderListener::TForceResendingToken*)ForceResendingToken.Token, Listener);
#else
	OnVerificationFailed.ExecuteIfBound(TEXT("Firebase Auth is disabled."));
#endif
}

FCredential UCredentialLibrary::GetCredential(const FString& VerificationId, const FString& VerificationCode)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	firebase::auth::PhoneAuthProvider& Provider = firebase::auth::PhoneAuthProvider::GetInstance(firebase::auth::Auth::GetAuth(FFirebaseFeaturesModule::GetCurrentFirebaseApp()));

	const firebase::auth::Credential RawCredential = Provider.GetCredential(TCHAR_TO_UTF8(*VerificationId), TCHAR_TO_UTF8(*VerificationCode));

	*Credential.Credential = RawCredential;
#endif

	return Credential;
}

FCredential UCredentialLibrary::GetCredentialFromTwitter(const FString& Token, const FString& Secret)
{
	FCredential Credential;

#if WITH_FIREBASE_AUTH
	const firebase::auth::Credential RawCredential = firebase::auth::TwitterAuthProvider::GetCredential(TCHAR_TO_UTF8(*Token), TCHAR_TO_UTF8(*Secret));

	*Credential.Credential = RawCredential;
#endif

	return Credential;
}

#if PLATFORM_IOS && WITH_FIREBASE_AUTH
@interface FFIRAppleSignInDelegate : NSObject<ASAuthorizationControllerDelegate>
{
	FGetAppleSignInCredentialCallback _Callback;
	FString _Nonce;
}

- (void)authorizationController: (ASAuthorizationController*)controller didCompleteWithAuthorization: (ASAuthorization*)authorization;
- (void)authorizationController: (ASAuthorizationController*)controller didCompleteWithError: (NSError*)error;
@end

@implementation FFIRAppleSignInDelegate
{

}

- (id) initWithNonce: (FString&&) Nonce callback: (FGetAppleSignInCredentialCallback&&) Callback
{
	self = [super init];

	if (self != nil)
	{
		_Callback = MoveTemp(Callback);
		_Nonce = MoveTemp(Nonce);

		if (@available(iOS 13, tvOS 13, macOS 10.15, *))
		{
		    ASAuthorizationAppleIDProvider* const Provider = [[ASAuthorizationAppleIDProvider alloc] init];
		    ASAuthorizationAppleIDRequest*  const Request  = [Provider createRequest];
		
		    Request.requestedScopes = @[ ASAuthorizationScopeEmail, ASAuthorizationScopeFullName ];
			Request.nonce = [self stringBySha256HashingString: _Nonce.GetNSString()];

		    ASAuthorizationController* const Controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests: @[ Request ]];
		
		    Controller.delegate = self;
		
			UE_LOG(LogFirebaseAuth, Log, TEXT("Performing Apple Sign-In request."));

		    [Controller performRequests];

			[Controller release];
			[Provider release];
		}
		else // iOS < 13
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Apple Sign-In is unavailable on device with iOS 12 or earlier."));
			Callback.ExecuteIfBound(false, {}, 151, TEXT("Apple Sign-In is unavailable on device with iOS 12 or earlier."));
			[self release];
		}
	}

	return self;
}

- (void) authorizationController: (ASAuthorizationController*)controller 
    didCompleteWithAuthorization: (ASAuthorization*)authorization
{
	if ([authorization.credential isKindOfClass: [ASAuthorizationAppleIDCredential class]])
    {
        ASAuthorizationAppleIDCredential* const AppleIDCredential = authorization.credential;

		if (AppleIDCredential.identityToken == nil) 
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to get identity token."));
			AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(_Callback)]() -> void
			{
				Callback.ExecuteIfBound(false, {}, 152, TEXT("Unable to fetch Identity Token."));
			});
			return;
		}
		
		UE_LOG(LogFirebaseAuth, Log, TEXT("Apple Sign-In request succeeded."));

		NSString* IdTokenRaw = [[NSString alloc] initWithData: AppleIDCredential.identityToken
												     encoding: NSUTF8StringEncoding];

		FString IdToken = IdTokenRaw;

		[IdTokenRaw release];

		auto FirCred = firebase::auth::OAuthProvider::GetCredential("apple.com", TCHAR_TO_UTF8(*IdToken), TCHAR_TO_UTF8(*_Nonce), nullptr);


		AsyncTask(ENamedThreads::GameThread, [FirCred = MoveTemp(FirCred), Callback = MoveTemp(_Callback)]() -> void
		{
			Callback.ExecuteIfBound(true, FirCred, 0, {});
		});
    }
	else
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid credential received."));

		AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(_Callback)]() -> void
		{
			Callback.ExecuteIfBound(false, {}, 150, TEXT("Invalid credential class."));
		});
	}

	[self release];
}

- (void) authorizationController: (ASAuthorizationController*)controller 
            didCompleteWithError: (NSError*)error
{
	int32 ErrorCode = 0;
	FString ErrorMessage;

	if (error != nil)
	{
		ErrorCode = error.code;
		ErrorMessage = error.description;

		UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to get Apple Sign-In credential: %d - %s"), ErrorCode, *ErrorMessage);
	}

	AsyncTask(ENamedThreads::GameThread, [ErrorCode, ErrorMessage = MoveTemp(ErrorMessage), Callback = MoveTemp(_Callback)]() -> void
	{
		Callback.ExecuteIfBound(false, {}, ErrorCode, ErrorMessage);
	});

	[self release];
}

- (NSString*) stringBySha256HashingString: (NSString*) input 
 {
    const char *string = [input UTF8String];
    unsigned char result[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256(string, (CC_LONG)strlen(string), result);

    NSMutableString *hashed = [NSMutableString stringWithCapacity:CC_SHA256_DIGEST_LENGTH * 2];
    for (NSInteger i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) 
	{
      [hashed appendFormat:@"%02x", result[i]];
    }
    return hashed;
}

@end

#endif

void UCredentialLibrary::GetCredentialFromAppleSignIn(FString Nonce, FGetAppleSignInCredentialCallback Callback)
{
#if PLATFORM_IOS && WITH_FIREBASE_AUTH
	[[FFIRAppleSignInDelegate alloc] initWithNonce: MoveTemp(Nonce) callback: MoveTemp(Callback)];
#else
	Callback.ExecuteIfBound(false, {}, 154, TEXT("Not implemented on this platform."));
#endif
}
