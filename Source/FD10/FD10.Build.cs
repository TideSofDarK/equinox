// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FD10 : ModuleRules
{
    public FD10(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore",
            "RHI"
        });
        
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new [] {
                    "UnrealEd"
                }
            );
        }
    }
}