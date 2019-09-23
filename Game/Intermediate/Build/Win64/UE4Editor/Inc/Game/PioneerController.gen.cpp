// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Game/PioneerController.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePioneerController() {}
// Cross Module References
	GAME_API UClass* Z_Construct_UClass_APioneerController_NoRegister();
	GAME_API UClass* Z_Construct_UClass_APioneerController();
	ENGINE_API UClass* Z_Construct_UClass_APlayerController();
	UPackage* Z_Construct_UPackage__Script_Game();
	GAME_API UFunction* Z_Construct_UFunction_APioneerController_MoveForward();
	GAME_API UFunction* Z_Construct_UFunction_APioneerController_MoveRight();
// End Cross Module References
	void APioneerController::StaticRegisterNativesAPioneerController()
	{
		UClass* Class = APioneerController::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "MoveForward", &APioneerController::execMoveForward },
			{ "MoveRight", &APioneerController::execMoveRight },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_APioneerController_MoveForward_Statics
	{
		struct PioneerController_eventMoveForward_Parms
		{
			float value;
		};
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_value;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_APioneerController_MoveForward_Statics::NewProp_value = { "value", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerController_eventMoveForward_Parms, value), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_APioneerController_MoveForward_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerController_MoveForward_Statics::NewProp_value,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_APioneerController_MoveForward_Statics::Function_MetaDataParams[] = {
		{ "Category", "Player Actions" },
		{ "ModuleRelativePath", "PioneerController.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_APioneerController_MoveForward_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_APioneerController, nullptr, "MoveForward", nullptr, nullptr, sizeof(PioneerController_eventMoveForward_Parms), Z_Construct_UFunction_APioneerController_MoveForward_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_APioneerController_MoveForward_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04080401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_APioneerController_MoveForward_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_APioneerController_MoveForward_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_APioneerController_MoveForward()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_APioneerController_MoveForward_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_APioneerController_MoveRight_Statics
	{
		struct PioneerController_eventMoveRight_Parms
		{
			float value;
		};
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_value;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_APioneerController_MoveRight_Statics::NewProp_value = { "value", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(PioneerController_eventMoveRight_Parms, value), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_APioneerController_MoveRight_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_APioneerController_MoveRight_Statics::NewProp_value,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_APioneerController_MoveRight_Statics::Function_MetaDataParams[] = {
		{ "Category", "Player Actions" },
		{ "Comment", "/** ?\xc3\xb7??\xcc\xbe\xee\xb8\xa6 ?\xd5\xb5\xda\xb7? ?\xcc\xb5???\xc5\xb0?? ?\xd4\xbc??\xd4\xb4\xcf\xb4?. */" },
		{ "ModuleRelativePath", "PioneerController.h" },
		{ "ToolTip", "?\xc3\xb7??\xcc\xbe\xee\xb8\xa6 ?\xd5\xb5\xda\xb7? ?\xcc\xb5???\xc5\xb0?? ?\xd4\xbc??\xd4\xb4\xcf\xb4?." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_APioneerController_MoveRight_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_APioneerController, nullptr, "MoveRight", nullptr, nullptr, sizeof(PioneerController_eventMoveRight_Parms), Z_Construct_UFunction_APioneerController_MoveRight_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_APioneerController_MoveRight_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04080401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_APioneerController_MoveRight_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_APioneerController_MoveRight_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_APioneerController_MoveRight()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_APioneerController_MoveRight_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_APioneerController_NoRegister()
	{
		return APioneerController::StaticClass();
	}
	struct Z_Construct_UClass_APioneerController_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_APioneerController_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_APlayerController,
		(UObject* (*)())Z_Construct_UPackage__Script_Game,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_APioneerController_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_APioneerController_MoveForward, "MoveForward" }, // 3531983888
		{ &Z_Construct_UFunction_APioneerController_MoveRight, "MoveRight" }, // 401217706
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_APioneerController_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "PioneerController.h" },
		{ "ModuleRelativePath", "PioneerController.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_APioneerController_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APioneerController>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_APioneerController_Statics::ClassParams = {
		&APioneerController::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x009002A4u,
		METADATA_PARAMS(Z_Construct_UClass_APioneerController_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_APioneerController_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_APioneerController()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_APioneerController_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(APioneerController, 1594023030);
	template<> GAME_API UClass* StaticClass<APioneerController>()
	{
		return APioneerController::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_APioneerController(Z_Construct_UClass_APioneerController, &APioneerController::StaticClass, TEXT("/Script/Game"), TEXT("APioneerController"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(APioneerController);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
