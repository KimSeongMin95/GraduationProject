// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Game/GameGameModeBase.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameGameModeBase() {}
// Cross Module References
	GAME_API UClass* Z_Construct_UClass_AGameGameModeBase_NoRegister();
	GAME_API UClass* Z_Construct_UClass_AGameGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_Game();
// End Cross Module References
	void AGameGameModeBase::StaticRegisterNativesAGameGameModeBase()
	{
	}
	UClass* Z_Construct_UClass_AGameGameModeBase_NoRegister()
	{
		return AGameGameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_AGameGameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGameGameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameGameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "GameGameModeBase.h" },
		{ "ModuleRelativePath", "GameGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGameGameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameGameModeBase>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGameGameModeBase_Statics::ClassParams = {
		&AGameGameModeBase::StaticClass,
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
		0x009002A8u,
		METADATA_PARAMS(Z_Construct_UClass_AGameGameModeBase_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AGameGameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGameGameModeBase()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGameGameModeBase_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGameGameModeBase, 4275740555);
	template<> GAME_API UClass* StaticClass<AGameGameModeBase>()
	{
		return AGameGameModeBase::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGameGameModeBase(Z_Construct_UClass_AGameGameModeBase, &AGameGameModeBase::StaticClass, TEXT("/Script/Game"), TEXT("AGameGameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGameGameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
