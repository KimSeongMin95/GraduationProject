// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        /* 기존 모듈에 새로 추가했습니다.
         * VR: "HeadMountedDisplay" 
         * Navigation: "NavigationSystem", "AIModule"
         * Landscape: "Landscape" 
         */
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule" });

        /* ALandscape::GetPrivateStaticClass() 오류가 발생하여 PrivateDependencyModuleNames에 "Landscape"를 추가.
         * 
         */
        PrivateDependencyModuleNames.AddRange(new string[] { "Landscape" });




		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
