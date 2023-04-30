// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ArenaBattle1 : ModuleRules
{
	public ArenaBattle1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "NavigationSystem", "AIModule", "GameplayTasks" });

        // 구현부가 모여있는 Private 폴더에서 사용할 모듈
        PrivateDependencyModuleNames.AddRange(new string[] { "ArenaBattleSetting" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
