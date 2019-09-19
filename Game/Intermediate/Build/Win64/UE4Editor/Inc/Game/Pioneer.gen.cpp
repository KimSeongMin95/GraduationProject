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
	GAME_API UFunction* Z_Construct_UFunction_APioneer_OnSetPlayerController();
	PAPER2D_API UClass* Z_Construct_UClass_UPaperSpriteComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USpringArmComponent_NoRegister();
// End Cross Module References
	void APioneer::StaticRegisterNativesAPioneer()
	{
		UClass* Class = APioneer::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "OnSetPlayerController", &APioneer::execOnSetPlayerController },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics
	{
		struct Pioneer_eventOnSetPlayerController_Parms
		{
			bool status;
		};
		static void NewProp_status_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_status;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::NewProp_status_SetBit(void* Obj)
	{
		((Pioneer_eventOnSetPlayerController_Parms*)Obj)->status = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::NewProp_status = { "status", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(Pioneer_eventOnSetPlayerController_Parms), &Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::NewProp_status_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::NewProp_status,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::Function_MetaDataParams[] = {
		{ "Category", "Player Attributes" },
		{ "Comment", "// Functions \n// ?\xd4\xb7? \xc8\xb0??\xc8\xad ?\xc7\xb4? ??\xc8\xb0??\xc8\xad\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "Functions\n?\xd4\xb7? \xc8\xb0??\xc8\xad ?\xc7\xb4? ??\xc8\xb0??\xc8\xad" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_APioneer, nullptr, "OnSetPlayerController", nullptr, nullptr, sizeof(Pioneer_eventOnSetPlayerController_Parms), Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_APioneer_OnSetPlayerController()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_APioneer_OnSetPlayerController_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_APioneer_NoRegister()
	{
		return APioneer::StaticClass();
	}
	struct Z_Construct_UClass_APioneer_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_IsAlive_MetaData[];
#endif
		static void NewProp_IsAlive_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_IsAlive;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_JumppingVelocity_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_JumppingVelocity;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_BaseLookUpRate_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_BaseLookUpRate;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_BaseTurnRate_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_BaseTurnRate;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_EffectSprite_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_EffectSprite;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FollowCamera_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_FollowCamera;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CameraBoom_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_CameraBoom;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_APioneer_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ACharacter,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_APioneer_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_APioneer_OnSetPlayerController, "OnSetPlayerController" }, // 2629305524
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
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive_MetaData[] = {
		{ "Category", "Player Attributes" },
		{ "Comment", "// ??\xc3\xb4???? ???? ?\xd6\xb4??? ?\xc6\xb4??? ????\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "??\xc3\xb4???? ???? ?\xd6\xb4??? ?\xc6\xb4??? ????" },
	};
#endif
	void Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive_SetBit(void* Obj)
	{
		((APioneer*)Obj)->IsAlive = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive = { "IsAlive", nullptr, (EPropertyFlags)0x0010000000020015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(APioneer), &Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive_SetBit, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_JumppingVelocity_MetaData[] = {
		{ "Category", "Player Attributes" },
		{ "Comment", "// ?\xe2\xba\xbb ???? ?\xd3\xb5?\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?\xe2\xba\xbb ???? ?\xd3\xb5?" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_JumppingVelocity = { "JumppingVelocity", nullptr, (EPropertyFlags)0x0010000000020015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, JumppingVelocity), METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_JumppingVelocity_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_JumppingVelocity_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_BaseLookUpRate_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// ?\xe2\xba\xbb ?\xc3\xbc? ??->???? ?????? ?\xca\xb4? ?????? ?????\xcb\xb4\xcf\xb4?. ???? ???? ???? ?????? ?????? ??\xc4\xa8?\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?\xe2\xba\xbb ?\xc3\xbc? ??->???? ?????? ?\xca\xb4? ?????? ?????\xcb\xb4\xcf\xb4?. ???? ???? ???? ?????? ?????? ??\xc4\xa8?\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_BaseLookUpRate = { "BaseLookUpRate", nullptr, (EPropertyFlags)0x0010000000020015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, BaseLookUpRate), METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_BaseLookUpRate_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_BaseLookUpRate_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_BaseTurnRate_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// ?\xe2\xba\xbb ??\xc8\xb8???? ?\xca\xb4? \xc8\xb8?? ?????? ?????\xcb\xb4\xcf\xb4?. ???? ???? ???? ??\xc8\xb8???? ?????? ??\xc4\xa8?\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?\xe2\xba\xbb ??\xc8\xb8???? ?\xca\xb4? \xc8\xb8?? ?????? ?????\xcb\xb4\xcf\xb4?. ???? ???? ???? ??\xc8\xb8???? ?????? ??\xc4\xa8?\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_BaseTurnRate = { "BaseTurnRate", nullptr, (EPropertyFlags)0x0010000000020015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, BaseTurnRate), METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_BaseTurnRate_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_BaseTurnRate_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_EffectSprite_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Effects" },
		{ "Comment", "// ????????\xc6\xae?? HUD?? ?\xd8\xbd?\xc3\xb3???? ?????\xcf\xb1? ???? ?? ???? \xc8\xbf???? ?\xd7\xb8??? ?? ?????\xcb\xb4\xcf\xb4?.\n" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "????????\xc6\xae?? HUD?? ?\xd8\xbd?\xc3\xb3???? ?????\xcf\xb1? ???? ?? ???? \xc8\xbf???? ?\xd7\xb8??? ?? ?????\xcb\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_EffectSprite = { "EffectSprite", nullptr, (EPropertyFlags)0x00100000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, EffectSprite), Z_Construct_UClass_UPaperSpriteComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_EffectSprite_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_EffectSprite_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_FollowCamera_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
		{ "Comment", "// ?????\xd9\xb4\xcf\xb4? \xc4\xab?\xde\xb6?\n" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "?????\xd9\xb4\xcf\xb4? \xc4\xab?\xde\xb6?" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_FollowCamera = { "FollowCamera", nullptr, (EPropertyFlags)0x00100000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, FollowCamera), Z_Construct_UClass_UCameraComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_FollowCamera_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_FollowCamera_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
		{ "Comment", "// Variables\n// \xc4\xb3???? ???\xdf\xbf??? \xc4\xab?\xde\xb6??? ??\xc4\xa1?? ?????\xd5\xb4\xcf\xb4?.\n" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Pioneer.h" },
		{ "ToolTip", "Variables\n\xc4\xb3???? ???\xdf\xbf??? \xc4\xab?\xde\xb6??? ??\xc4\xa1?? ?????\xd5\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom = { "CameraBoom", nullptr, (EPropertyFlags)0x00100000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneer, CameraBoom), Z_Construct_UClass_USpringArmComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APioneer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_IsAlive,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_JumppingVelocity,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_BaseLookUpRate,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_BaseTurnRate,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_EffectSprite,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_FollowCamera,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneer_Statics::NewProp_CameraBoom,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneer_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneer>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneer_Statics::ClassParams = {
		&APioneer::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_APioneer_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
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
	IMPLEMENT_CLASS(APioneer, 3807684637);
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
