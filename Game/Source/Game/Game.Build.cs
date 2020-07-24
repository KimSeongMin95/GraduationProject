// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(ReadOnlyTargetRules Target) : base(Target)
	{
        // IWYU ���Ӽ� �� ���
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        /* ���� ��⿡ ���� �߰��߽��ϴ�.
         * VR: "HeadMountedDisplay" 
         * Navigation: "NavigationSystem", "AIModule"
         * Landscape: "Landscape" 
         * UMG, Slate, SlateCore: UMG �߰��� ���� (Widget Blueprint)
         */
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
            "HeadMountedDisplay", "NavigationSystem", "AIModule", "UMG" });

        /* ALandscape::GetPrivateStaticClass() ������ �߻��Ͽ� PrivateDependencyModuleNames�� "Landscape"�� �߰�.
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
