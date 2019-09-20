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
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
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
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ListOfPioneerCamera_MetaData[];
#endif
		static const UE4CodeGen_Private::FMapPropertyParams NewProp_ListOfPioneerCamera;
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_ListOfPioneerCamera_Key_KeyProp;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ListOfPioneerCamera_ValueProp;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
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
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_MetaData[] = {
		{ "Category", "PioneerManager" },
		{ "Comment", "// ?? ??\xc3\xb4???? AActor* Camera?? ?????\xcf\xb4? ????\xc6\xae?\xd4\xb4\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "PioneerManager.h" },
		{ "ToolTip", "?? ??\xc3\xb4???? AActor* Camera?? ?????\xcf\xb4? ????\xc6\xae?\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FMapPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera = { "ListOfPioneerCamera", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneerManager, ListOfPioneerCamera), METADATA_PARAMS(Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_MetaData)) };
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_Key_KeyProp = { "ListOfPioneerCamera_Key", nullptr, (EPropertyFlags)0x0000000000000001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_ValueProp = { "ListOfPioneerCamera", nullptr, (EPropertyFlags)0x0000000000000001, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 1, Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APioneerManager_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_Key_KeyProp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_ListOfPioneerCamera_ValueProp,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneerManager_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneerManager>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneerManager_Statics::ClassParams = {
		&APioneerManager::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_APioneerManager_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_APioneerManager_Statics::PropPointers),
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
	IMPLEMENT_CLASS(APioneerManager, 3934165904);
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
