// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Game/WorldViewCameraActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWorldViewCameraActor() {}
// Cross Module References
	GAME_API UClass* Z_Construct_UClass_AWorldViewCameraActor_NoRegister();
	GAME_API UClass* Z_Construct_UClass_AWorldViewCameraActor();
	ENGINE_API UClass* Z_Construct_UClass_ACameraActor();
	UPackage* Z_Construct_UPackage__Script_Game();
// End Cross Module References
	void AWorldViewCameraActor::StaticRegisterNativesAWorldViewCameraActor()
	{
	}
	UClass* Z_Construct_UClass_AWorldViewCameraActor_NoRegister()
	{
		return AWorldViewCameraActor::StaticClass();
	}
	struct Z_Construct_UClass_AWorldViewCameraActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AWorldViewCameraActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ACameraActor,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AWorldViewCameraActor_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Input Rendering" },
		{ "IncludePath", "WorldViewCameraActor.h" },
		{ "ModuleRelativePath", "WorldViewCameraActor.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AWorldViewCameraActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AWorldViewCameraActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AWorldViewCameraActor_Statics::ClassParams = {
		&AWorldViewCameraActor::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_AWorldViewCameraActor_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AWorldViewCameraActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AWorldViewCameraActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AWorldViewCameraActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AWorldViewCameraActor, 3647779200);
	template<> GAME_API UClass* StaticClass<AWorldViewCameraActor>()
	{
		return AWorldViewCameraActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AWorldViewCameraActor(Z_Construct_UClass_AWorldViewCameraActor, &AWorldViewCameraActor::StaticClass, TEXT("/Script/Game"), TEXT("AWorldViewCameraActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AWorldViewCameraActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
