// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SolarSystem2 : ModuleRules
{
	public SolarSystem2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ProceduralMeshComponent",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
