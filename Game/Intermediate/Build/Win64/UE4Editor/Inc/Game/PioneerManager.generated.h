// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAME_PioneerManager_generated_h
#error "PioneerManager.generated.h already included, missing '#pragma once' in PioneerManager.h"
#endif
#define GAME_PioneerManager_generated_h

#define Game_Source_Game_PioneerManager_h_21_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execPossessPioneer) \
	{ \
		P_GET_PROPERTY(UIntProperty,Z_Param_ID); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->PossessPioneer(Z_Param_ID); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSwitchViewTarget) \
	{ \
		P_GET_PROPERTY(UIntProperty,Z_Param_ID); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_BlendTime); \
		P_GET_PROPERTY(UByteProperty,Z_Param_blendFunc); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_BlendExp); \
		P_GET_UBOOL(Z_Param_bLockOutgoing); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SwitchViewTarget(Z_Param_ID,Z_Param_BlendTime,EViewTargetBlendFunction(Z_Param_blendFunc),Z_Param_BlendExp,Z_Param_bLockOutgoing); \
		P_NATIVE_END; \
	}


#define Game_Source_Game_PioneerManager_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execPossessPioneer) \
	{ \
		P_GET_PROPERTY(UIntProperty,Z_Param_ID); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->PossessPioneer(Z_Param_ID); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSwitchViewTarget) \
	{ \
		P_GET_PROPERTY(UIntProperty,Z_Param_ID); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_BlendTime); \
		P_GET_PROPERTY(UByteProperty,Z_Param_blendFunc); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_BlendExp); \
		P_GET_UBOOL(Z_Param_bLockOutgoing); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SwitchViewTarget(Z_Param_ID,Z_Param_BlendTime,EViewTargetBlendFunction(Z_Param_blendFunc),Z_Param_BlendExp,Z_Param_bLockOutgoing); \
		P_NATIVE_END; \
	}


#define Game_Source_Game_PioneerManager_h_21_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPioneerManager(); \
	friend struct Z_Construct_UClass_APioneerManager_Statics; \
public: \
	DECLARE_CLASS(APioneerManager, AActor, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneerManager)


#define Game_Source_Game_PioneerManager_h_21_INCLASS \
private: \
	static void StaticRegisterNativesAPioneerManager(); \
	friend struct Z_Construct_UClass_APioneerManager_Statics; \
public: \
	DECLARE_CLASS(APioneerManager, AActor, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneerManager)


#define Game_Source_Game_PioneerManager_h_21_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API APioneerManager(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(APioneerManager) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneerManager); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneerManager); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneerManager(APioneerManager&&); \
	NO_API APioneerManager(const APioneerManager&); \
public:


#define Game_Source_Game_PioneerManager_h_21_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneerManager(APioneerManager&&); \
	NO_API APioneerManager(const APioneerManager&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneerManager); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneerManager); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APioneerManager)


#define Game_Source_Game_PioneerManager_h_21_PRIVATE_PROPERTY_OFFSET
#define Game_Source_Game_PioneerManager_h_18_PROLOG
#define Game_Source_Game_PioneerManager_h_21_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_PioneerManager_h_21_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_PioneerManager_h_21_RPC_WRAPPERS \
	Game_Source_Game_PioneerManager_h_21_INCLASS \
	Game_Source_Game_PioneerManager_h_21_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Game_Source_Game_PioneerManager_h_21_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_PioneerManager_h_21_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_PioneerManager_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	Game_Source_Game_PioneerManager_h_21_INCLASS_NO_PURE_DECLS \
	Game_Source_Game_PioneerManager_h_21_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAME_API UClass* StaticClass<class APioneerManager>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Game_Source_Game_PioneerManager_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
