// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FD10Target : TargetRules
{
    public FD10Target(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;

        DefaultBuildSettings = BuildSettingsVersion.V2;

        ExtraModuleNames.AddRange(new[] {"FD10"});
    }
}