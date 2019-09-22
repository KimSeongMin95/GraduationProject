// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAME_Pioneer_generated_h
#error "Pioneer.generated.h already included, missing '#pragma once' in Pioneer.h"
#endif
#define GAME_Pioneer_generated_h

#define Game_Source_Game_Pioneer_h_22_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execStopEvasionRoll) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->StopEvasionRoll(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEvasionRoll) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->EvasionRoll(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMoveRight) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_value); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MoveRight(Z_Param_value); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMoveForward) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_value); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MoveForward(Z_Param_value); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnSetPlayerController) \
	{ \
		P_GET_UBOOL(Z_Param_status); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnSetPlayerController(Z_Param_status); \
		P_NATIVE_END; \
	}


#define Game_Source_Game_Pioneer_h_22_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execStopEvasionRoll) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->StopEvasionRoll(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEvasionRoll) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->EvasionRoll(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMoveRight) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_value); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MoveRight(Z_Param_value); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMoveForward) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_value); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MoveForward(Z_Param_value); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnSetPlayerController) \
	{ \
		P_GET_UBOOL(Z_Param_status); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnSetPlayerController(Z_Param_status); \
		P_NATIVE_END; \
	}


#define Game_Source_Game_Pioneer_h_22_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPioneer(); \
	friend struct Z_Construct_UClass_APioneer_Statics; \
public: \
	DECLARE_CLASS(APioneer, ACharacter, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneer)


#define Game_Source_Game_Pioneer_h_22_INCLASS \
private: \
	static void StaticRegisterNativesAPioneer(); \
	friend struct Z_Construct_UClass_APioneer_Statics; \
public: \
	DECLARE_CLASS(APioneer, ACharacter, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneer)


#define Game_Source_Game_Pioneer_h_22_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API APioneer(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(APioneer) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneer); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneer); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneer(APioneer&&); \
	NO_API APioneer(const APioneer&); \
public:


#define Game_Source_Game_Pioneer_h_22_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneer(APioneer&&); \
	NO_API APioneer(const APioneer&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneer); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneer); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APioneer)


#define Game_Source_Game_Pioneer_h_22_PRIVATE_PROPERTY_OFFSET
#define Game_Source_Game_Pioneer_h_19_PROLOG
#define Game_Source_Game_Pioneer_h_22_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_Pioneer_h_22_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_Pioneer_h_22_RPC_WRAPPERS \
	Game_Source_Game_Pioneer_h_22_INCLASS \
	Game_Source_Game_Pioneer_h_22_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Game_Source_Game_Pioneer_h_22_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_Pioneer_h_22_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_Pioneer_h_22_RPC_WRAPPERS_NO_PURE_DECLS \
	Game_Source_Game_Pioneer_h_22_INCLASS_NO_PURE_DECLS \
	Game_Source_Game_Pioneer_h_22_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAME_API UClass* StaticClass<class APioneer>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Game_Source_Game_Pioneer_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
