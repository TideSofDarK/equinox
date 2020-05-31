// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FD10EditorTarget : TargetRules
{
    public FD10EditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        DefaultBuildSettings = BuildSettingsVersion.V2;

        ExtraModuleNames.AddRange(new[] {"FD10"});
    }
}