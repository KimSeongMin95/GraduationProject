// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAME_PioneerController_generated_h
#error "PioneerController.generated.h already included, missing '#pragma once' in PioneerController.h"
#endif
#define GAME_PioneerController_generated_h

#define Game_Source_Game_PioneerController_h_17_RPC_WRAPPERS \
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
	}


#define Game_Source_Game_PioneerController_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
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
	}


#define Game_Source_Game_PioneerController_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPioneerController(); \
	friend struct Z_Construct_UClass_APioneerController_Statics; \
public: \
	DECLARE_CLASS(APioneerController, APlayerController, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneerController)


#define Game_Source_Game_PioneerController_h_17_INCLASS \
private: \
	static void StaticRegisterNativesAPioneerController(); \
	friend struct Z_Construct_UClass_APioneerController_Statics; \
public: \
	DECLARE_CLASS(APioneerController, APlayerController, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneerController)


#define Game_Source_Game_PioneerController_h_17_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API APioneerController(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(APioneerController) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneerController); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneerController); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneerController(APioneerController&&); \
	NO_API APioneerController(const APioneerController&); \
public:


#define Game_Source_Game_PioneerController_h_17_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneerController(APioneerController&&); \
	NO_API APioneerController(const APioneerController&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneerController); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneerController); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APioneerController)


#define Game_Source_Game_PioneerController_h_17_PRIVATE_PROPERTY_OFFSET
#define Game_Source_Game_PioneerController_h_14_PROLOG
#define Game_Source_Game_PioneerController_h_17_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_PioneerController_h_17_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_PioneerController_h_17_RPC_WRAPPERS \
	Game_Source_Game_PioneerController_h_17_INCLASS \
	Game_Source_Game_PioneerController_h_17_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Game_Source_Game_PioneerController_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_PioneerController_h_17_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_PioneerController_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	Game_Source_Game_PioneerController_h_17_INCLASS_NO_PURE_DECLS \
	Game_Source_Game_PioneerController_h_17_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAME_API UClass* StaticClass<class APioneerController>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Game_Source_Game_PioneerController_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
