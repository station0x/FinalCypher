// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam (FFutureDelegateVoid, const int32 /* Error */);
DECLARE_DELEGATE_TwoParams(FFutureDelegateString, const int32 /* Error */, const FString&);
