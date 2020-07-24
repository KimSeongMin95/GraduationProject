// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(ReadOnlyTargetRules Target) : base(Target)
	{
        // IWYU 종속성 모델 사용
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        /* 기존 모듈에 새로 추가했습니다.
         * VR: "HeadMountedDisplay" 
         * Navigation: "NavigationSystem", "AIModule"
         * Landscape: "Landscape" 
         * UMG, Slate, SlateCore: UMG 추가를 위해 (Widget Blueprint)
         */
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
            "HeadMountedDisplay", "NavigationSystem", "AIModule", "UMG" });

        /* ALandscape::GetPrivateStaticClass() 오류가 발생하여 PrivateDependencyModuleNames에 "Landscape"를 추가.
         * 
         */
        PrivateDependencyModuleNames.AddRange(new string[] { "Landscape", "Slate", "SlateCore" });




		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
