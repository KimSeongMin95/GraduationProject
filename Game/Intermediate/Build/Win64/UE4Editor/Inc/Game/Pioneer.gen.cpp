// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Game/Pioneer.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePioneer() {}
// Cross Module References
	GAME_API UClass* Z_Construct_UClass_APioneer_NoRegister();
	GAME_API UClass* Z_Construct_UClass_APioneer();
	ENGINE_API UClass* Z_Construct_UClass_ACharacter();
	UPackage* Z_Construct_UPackage__Script_Game();
// End Cross Module References
	void APioneer::StaticRegisterNativesAPioneer()
	{
	}
	UClass* Z_Construct_UClass_APioneer_NoRegister()
	{
		return APioneer::StaticClass();
	}
	struct Z_Construct_UClass_APioneer_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_APioneer_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ACharacter,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::Class_MetaDataParams[] = {
		{ "Comment", "// ?\xdf\xb0??? ????\n" },
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "Pioneer.h" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?\xdf\xb0??? ????" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneer_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneer>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneer_Statics::ClassParams = {
		&APioneer::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_APioneer()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_APioneer_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(APioneer, 1017864397);
	template<> GAME_API UClass* StaticClass<APioneer>()
	{
		return APioneer::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_APioneer(Z_Construct_UClass_APioneer, &APioneer::StaticClass, TEXT("/Script/Game"), TEXT("APioneer"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(APioneer);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
