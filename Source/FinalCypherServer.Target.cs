// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FinalCypherServerTarget : TargetRules
{
    public FinalCypherServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUseLoggingInShipping = true;
        ExtraModuleNames.Add("FinalCypher");
    }
}