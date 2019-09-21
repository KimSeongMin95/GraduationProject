// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Game/PioneerManager.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePioneerManager() {}
// Cross Module References
	GAME_API UClass* Z_Construct_UClass_APioneerManager_NoRegister();
	GAME_API UClass* Z_Construct_UClass_APioneerManager();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	UPackage* Z_Construct_UPackage__Script_Game();
// End Cross Module References
	void APioneerManager::StaticRegisterNativesAPioneerManager()
	{
	}
	UClass* Z_Construct_UClass_APioneerManager_NoRegister()
	{
		return APioneerManager::StaticClass();
	}
	struct Z_Construct_UClass_APioneerManager_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_APioneerManager_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerManager_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "PioneerManager.h" },
		{ "ModuleRelativePath", "PioneerManager.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneerManager_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneerManager>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneerManager_Statics::ClassParams = {
		&APioneerManager::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A0u,
		METADATA_PARAMS(Z_Construct_UClass_APioneerManager_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_APioneerManager_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_APioneerManager()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_APioneerManager_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(APioneerManager, 3373788769);
	template<> GAME_API UClass* StaticClass<APioneerManager>()
	{
		return APioneerManager::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_APioneerManager(Z_Construct_UClass_APioneerManager, &APioneerManager::StaticClass, TEXT("/Script/Game"), TEXT("APioneerManager"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(APioneerManager);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
