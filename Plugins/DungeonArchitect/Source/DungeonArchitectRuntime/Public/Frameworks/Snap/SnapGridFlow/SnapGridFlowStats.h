//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Stats/Stats.h"

DECLARE_STATS_GROUP(TEXT("SGF"), STATGROUP_SGF, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Build"), STAT_SGFBuild, STATGROUP_SGF, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Resolve"), STAT_SGFResolve, STATGROUP_SGF, );

DECLARE_CYCLE_STAT_EXTERN(TEXT("IsValid"), STAT_SGFIsValid, STATGROUP_SGF, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Asm_Build"), STAT_SGFAsmBuild, STATGROUP_SGF, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Asm_CanFit"), STAT_SGFAsmCanFit, STATGROUP_SGF, );

