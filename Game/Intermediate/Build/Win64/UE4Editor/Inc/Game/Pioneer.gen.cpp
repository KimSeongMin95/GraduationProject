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
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FRotator();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
	ENGINE_API UClass* Z_Construct_UClass_UDecalComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USpringArmComponent_NoRegister();
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
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CameraLagSpeed_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_CameraLagSpeed;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TargetArmLength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_TargetArmLength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CameraBoomRotation_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CameraBoomRotation;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CameraBoomLocation_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CameraBoomLocation;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CursorToWorld_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_CursorToWorld;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TopDownCameraComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_TopDownCameraComponent;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CameraBoom_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_CameraBoom;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticMeshComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticMeshComponent;
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
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_CameraLagSpeed_MetaData[] = {
		{ "Category", "Pioneer" },
		{ "Comment", "// ArmSpring?? FollowCamera???? ?\xc5\xb8??\xd4\xb4\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "ArmSpring?? FollowCamera???? ?\xc5\xb8??\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_CameraLagSpeed = { "CameraLagSpeed", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, CameraLagSpeed), METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_CameraLagSpeed_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_CameraLagSpeed_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_TargetArmLength_MetaData[] = {
		{ "Category", "Pioneer" },
		{ "Comment", "// ArmSpring?? World \xc8\xb8???\xd4\xb4\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "ArmSpring?? World \xc8\xb8???\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_TargetArmLength = { "TargetArmLength", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, TargetArmLength), METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_TargetArmLength_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_TargetArmLength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomRotation_MetaData[] = {
		{ "Category", "Pioneer" },
		{ "Comment", "// ArmSpring?? World ??\xc7\xa5?\xd4\xb4\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "ArmSpring?? World ??\xc7\xa5?\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomRotation = { "CameraBoomRotation", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, CameraBoomRotation), Z_Construct_UScriptStruct_FRotator, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomRotation_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomRotation_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomLocation_MetaData[] = {
		{ "Category", "Pioneer" },
		{ "Comment", "/*** \xc4\xab?\xde\xb6? ?????? PIE?? ?????\xd5\xb4\xcf\xb4?. : ???? ***/" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "\xc4\xab?\xde\xb6? ?????? PIE?? ?????\xd5\xb4\xcf\xb4?. : ???? **" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomLocation = { "CameraBoomLocation", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, CameraBoomLocation), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomLocation_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomLocation_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_CursorToWorld_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
		{ "Comment", "/** ?????\xd9\xb4\xcf\xb4? \xc4\xab?\xde\xb6??\xd4\xb4\xcf\xb4?. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?????\xd9\xb4\xcf\xb4? \xc4\xab?\xde\xb6??\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_CursorToWorld = { "CursorToWorld", nullptr, (EPropertyFlags)0x00400000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, CursorToWorld), Z_Construct_UClass_UDecalComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_CursorToWorld_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_CursorToWorld_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_TopDownCameraComponent_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
		{ "Comment", "/** \xc4\xb3???? ???????? \xc4\xab?\xde\xb6??? ??\xc4\xa1?? ?????\xd5\xb4\xcf\xb4?. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "\xc4\xb3???? ???????? \xc4\xab?\xde\xb6??? ??\xc4\xa1?? ?????\xd5\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_TopDownCameraComponent = { "TopDownCameraComponent", nullptr, (EPropertyFlags)0x00400000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, TopDownCameraComponent), Z_Construct_UClass_UCameraComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_TopDownCameraComponent_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_TopDownCameraComponent_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
		{ "Comment", "/** ?\xd3\xbd\xc3\xb7? StaticMesh?? ?????\xd5\xb4\xcf\xb4?. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?\xd3\xbd\xc3\xb7? StaticMesh?? ?????\xd5\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom = { "CameraBoom", nullptr, (EPropertyFlags)0x00400000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, CameraBoom), Z_Construct_UClass_USpringArmComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_StaticMeshComponent_MetaData[] = {
		{ "Category", "Pioneer" },
		{ "Comment", "/*** Components : Start ***/" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "Components : Start **" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_StaticMeshComponent = { "StaticMeshComponent", nullptr, (EPropertyFlags)0x0040000000080009, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, StaticMeshComponent), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_StaticMeshComponent_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_StaticMeshComponent_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APioneer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_CameraLagSpeed,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_TargetArmLength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomRotation,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoomLocation,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_CursorToWorld,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_TopDownCameraComponent,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_StaticMeshComponent,
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
	IMPLEMENT_CLASS(APioneer, 2526170486);
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
