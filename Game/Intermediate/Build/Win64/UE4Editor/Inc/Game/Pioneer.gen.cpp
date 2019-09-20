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
	ENGINE_API UClass* Z_Construct_UClass_UStaticMeshComponent_NoRegister();
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
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TempStaticMesh_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_TempStaticMesh;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_APioneer_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ACharacter,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "Pioneer.h" },
		{ "ModuleRelativePath", "Pioneer.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_TempStaticMesh_MetaData[] = {
		{ "Category", "Pioneer" },
		{ "Comment", "// ????\xc6\xbd ?\xde\xbd??\xd4\xb4\xcf\xb4?.\n" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "????\xc6\xbd ?\xde\xbd??\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_TempStaticMesh = { "TempStaticMesh", nullptr, (EPropertyFlags)0x0010000000080009, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, TempStaticMesh), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_TempStaticMesh_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_TempStaticMesh_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APioneer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_TempStaticMesh,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneer_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneer>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneer_Statics::ClassParams = {
		&APioneer::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_APioneer_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::PropPointers),
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
	IMPLEMENT_CLASS(APioneer, 3604620460);
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
