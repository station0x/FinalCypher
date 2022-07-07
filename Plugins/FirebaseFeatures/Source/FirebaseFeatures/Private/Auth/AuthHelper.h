// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "FirebaseFeatures.h"

#if WITH_FIREBASE_AUTH

#include "CoreMinimal.h"
#include "Auth/Auth.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/auth.h"
#	include "firebase/auth/user.h"
THIRD_PARTY_INCLUDES_END

class UUser;

class FUserProfileDataManager
{
public:
	FUserProfileDataManager(const struct FUserProfile* const Profile);
	~FUserProfileDataManager();
	firebase::auth::User::UserProfile Get();
private:
	char* DisplayName;
	char* PhotoUrl;

	bool bAllocatedDisplayName;
	bool bAllocatedPhotoUrl;
};

#define CONVERT_USER_PROFILE(UserProfile) FUserProfileDataManager(UserProfile).Get()

class FAuthHelper
{
private:
	FAuthHelper() = delete;

public:
	static FSignInResult ConvertSignInResult(const firebase::auth::SignInResult* const SignInResult);

	static UUser* ConvertUser(firebase::auth::User* const User);
};
#endif
