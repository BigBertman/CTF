// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CaptureTheFlag : ModuleRules
{
    public CaptureTheFlag(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "UMG" });
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
