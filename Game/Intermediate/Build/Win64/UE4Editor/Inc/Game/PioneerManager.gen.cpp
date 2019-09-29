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
	GAME_API UFunction* Z_Construct_UFunction_APioneerManager_PossessPioneer();
	GAME_API UFunction* Z_Construct_UFunction_APioneerManager_SwitchViewTarget();
	ENGINE_API UEnum* Z_Construct_UEnum_Engine_EViewTargetBlendFunction();
	GAME_API UClass* Z_Construct_UClass_APioneer_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USceneComponent_NoRegister();
// End Cross Module References
	void APioneerManager::StaticRegisterNativesAPioneerManager()
	{
		UClass* Class = APioneerManager::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "PossessPioneer", &APioneerManager::execPossessPioneer },
			{ "SwitchViewTarget", &APioneerManager::execSwitchViewTarget },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics
	{
		struct PioneerManager_eventPossessPioneer_Parms
		{
			int32 ID;
		};
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_ID;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::NewProp_ID = { "ID", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerManager_eventPossessPioneer_Parms, ID), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::NewProp_ID,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::Function_MetaDataParams[] = {
		{ "Comment", "/** ?\xd9\xb8? ???? \xc4\xab?\xde\xb6??? ?????\xcf\xb4? ?\xd4\xbc??\xd4\xb4\xcf\xb4?. */// FTimerDelegate.BindUFunction( , FName(\"?\xd4\xbc??\xcc\xb8?\"), ...);???? ?\xd4\xbc? ?\xcc\xb8??? \xc3\xa3?????? ?????? UFUNCTION()?? ?????? ?\xd5\xb4\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "PioneerManager.h" },
		{ "ToolTip", "?\xd9\xb8? ???? \xc4\xab?\xde\xb6??? ?????\xcf\xb4? ?\xd4\xbc??\xd4\xb4\xcf\xb4?. // FTimerDelegate.BindUFunction( , FName(\"?\xd4\xbc??\xcc\xb8?\"), ...);???? ?\xd4\xbc? ?\xcc\xb8??? \xc3\xa3?????? ?????? UFUNCTION()?? ?????? ?\xd5\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_APioneerManager, nullptr, "PossessPioneer", nullptr, nullptr, sizeof(PioneerManager_eventPossessPioneer_Parms), Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_APioneerManager_PossessPioneer()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_APioneerManager_PossessPioneer_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics
	{
		struct PioneerManager_eventSwitchViewTarget_Parms
		{
			int32 ID;
			float BlendTime;
			TEnumAsByte<EViewTargetBlendFunction> blendFunc;
			float BlendExp;
			bool bLockOutgoing;
		};
		static void NewProp_bLockOutgoing_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bLockOutgoing;
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_BlendExp;
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_blendFunc;
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_BlendTime;
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_ID;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_bLockOutgoing_SetBit(void* Obj)
	{
		((PioneerManager_eventSwitchViewTarget_Parms*)Obj)->bLockOutgoing = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_bLockOutgoing = { "bLockOutgoing", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(PioneerManager_eventSwitchViewTarget_Parms), &Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_bLockOutgoing_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_BlendExp = { "BlendExp", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerManager_eventSwitchViewTarget_Parms, BlendExp), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_blendFunc = { "blendFunc", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerManager_eventSwitchViewTarget_Parms, blendFunc), Z_Construct_UEnum_Engine_EViewTargetBlendFunction, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_BlendTime = { "BlendTime", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerManager_eventSwitchViewTarget_Parms, BlendTime), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_ID = { "ID", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerManager_eventSwitchViewTarget_Parms, ID), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_bLockOutgoing,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_BlendExp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_blendFunc,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_BlendTime,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::NewProp_ID,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::Function_MetaDataParams[] = {
		{ "Comment", "/** ?\xd9\xb8? ?????? ?????\xcf\xb4? ?\xd4\xbc??\xd4\xb4\xcf\xb4?. */// FTimerDelegate.BindUFunction( , FName(\"?\xd4\xbc??\xcc\xb8?\"), ...);???? ?\xd4\xbc? ?\xcc\xb8??? \xc3\xa3?????? ?????? UFUNCTION()?? ?????? ?\xd5\xb4\xcf\xb4?.\n" },
		{ "ModuleRelativePath", "PioneerManager.h" },
		{ "ToolTip", "?\xd9\xb8? ?????? ?????\xcf\xb4? ?\xd4\xbc??\xd4\xb4\xcf\xb4?. // FTimerDelegate.BindUFunction( , FName(\"?\xd4\xbc??\xcc\xb8?\"), ...);???? ?\xd4\xbc? ?\xcc\xb8??? \xc3\xa3?????? ?????? UFUNCTION()?? ?????? ?\xd5\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_APioneerManager, nullptr, "SwitchViewTarget", nullptr, nullptr, sizeof(PioneerManager_eventSwitchViewTarget_Parms), Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_APioneerManager_SwitchViewTarget()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_APioneerManager_SwitchViewTarget_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_APioneerManager_NoRegister()
	{
		return APioneerManager::StaticClass();
	}
	struct Z_Construct_UClass_APioneerManager_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SwitchTime_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_SwitchTime;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TmapPioneers_MetaData[];
#endif
		static const UE4CodeGen_Private::FMapPropertyParams NewProp_TmapPioneers;
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_TmapPioneers_Key_KeyProp;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_TmapPioneers_ValueProp;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SceneComp_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SceneComp;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_APioneerManager_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_APioneerManager_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_APioneerManager_PossessPioneer, "PossessPioneer" }, // 4068250838
		{ &Z_Construct_UFunction_APioneerManager_SwitchViewTarget, "SwitchViewTarget" }, // 3581031503
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerManager_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "PioneerManager.h" },
		{ "ModuleRelativePath", "PioneerManager.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerManager_Statics::NewProp_SwitchTime_MetaData[] = {
		{ "Category", "PioneerManager" },
		{ "Comment", "/** Pioneer ???? ??\xc6\xae?\xd1\xb7? ?\xd4\xb4\xcf\xb4?. */" },
		{ "ModuleRelativePath", "PioneerManager.h" },
		{ "ToolTip", "Pioneer ???? ??\xc6\xae?\xd1\xb7? ?\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_SwitchTime = { "SwitchTime", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneerManager, SwitchTime), METADATA_PARAMS(Z_Construct_UClass_APioneerManager_Statics::NewProp_SwitchTime_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneerManager_Statics::NewProp_SwitchTime_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_MetaData[] = {
		{ "Category", "PioneerManager" },
		{ "ModuleRelativePath", "PioneerManager.h" },
	};
#endif
	const UE4CodeGen_Private::FMapPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers = { "TmapPioneers", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneerManager, TmapPioneers), METADATA_PARAMS(Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_MetaData)) };
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_Key_KeyProp = { "TmapPioneers_Key", nullptr, (EPropertyFlags)0x0000000000000001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_ValueProp = { "TmapPioneers", nullptr, (EPropertyFlags)0x0000000000000001, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 1, Z_Construct_UClass_APioneer_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerManager_Statics::NewProp_SceneComp_MetaData[] = {
		{ "Category", "PioneerManager" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "PioneerManager.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APioneerManager_Statics::NewProp_SceneComp = { "SceneComp", nullptr, (EPropertyFlags)0x0010000000080009, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(APioneerManager, SceneComp), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_APioneerManager_Statics::NewProp_SceneComp_MetaData, ARRAY_COUNT(Z_Construct_UClass_APioneerManager_Statics::NewProp_SceneComp_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APioneerManager_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_SwitchTime,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_Key_KeyProp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_TmapPioneers_ValueProp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APioneerManager_Statics::NewProp_SceneComp,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneerManager_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneerManager>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneerManager_Statics::ClassParams = {
		&APioneerManager::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_APioneerManager_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
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
	IMPLEMENT_CLASS(APioneerManager, 742574034);
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
