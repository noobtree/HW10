// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HW10 : ModuleRules
{
	public HW10(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		PrivateIncludePaths.AddRange(new string[] { "HW10" });
    }
}
