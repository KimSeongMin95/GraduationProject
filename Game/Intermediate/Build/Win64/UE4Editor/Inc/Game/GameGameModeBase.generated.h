// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAME_GameGameModeBase_generated_h
#error "GameGameModeBase.generated.h already included, missing '#pragma once' in GameGameModeBase.h"
#endif
#define GAME_GameGameModeBase_generated_h

#define Game_Source_Game_GameGameModeBase_h_15_RPC_WRAPPERS
#define Game_Source_Game_GameGameModeBase_h_15_RPC_WRAPPERS_NO_PURE_DECLS
#define Game_Source_Game_GameGameModeBase_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAGameGameModeBase(); \
	friend struct Z_Construct_UClass_AGameGameModeBase_Statics; \
public: \
	DECLARE_CLASS(AGameGameModeBase, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(AGameGameModeBase)


#define Game_Source_Game_GameGameModeBase_h_15_INCLASS \
private: \
	static void StaticRegisterNativesAGameGameModeBase(); \
	friend struct Z_Construct_UClass_AGameGameModeBase_Statics; \
public: \
	DECLARE_CLASS(AGameGameModeBase, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(AGameGameModeBase)


#define Game_Source_Game_GameGameModeBase_h_15_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AGameGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AGameGameModeBase) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AGameGameModeBase); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AGameGameModeBase); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AGameGameModeBase(AGameGameModeBase&&); \
	NO_API AGameGameModeBase(const AGameGameModeBase&); \
public:


#define Game_Source_Game_GameGameModeBase_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AGameGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AGameGameModeBase(AGameGameModeBase&&); \
	NO_API AGameGameModeBase(const AGameGameModeBase&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AGameGameModeBase); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AGameGameModeBase); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AGameGameModeBase)


#define Game_Source_Game_GameGameModeBase_h_15_PRIVATE_PROPERTY_OFFSET
#define Game_Source_Game_GameGameModeBase_h_12_PROLOG
#define Game_Source_Game_GameGameModeBase_h_15_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_GameGameModeBase_h_15_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_GameGameModeBase_h_15_RPC_WRAPPERS \
	Game_Source_Game_GameGameModeBase_h_15_INCLASS \
	Game_Source_Game_GameGameModeBase_h_15_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Game_Source_Game_GameGameModeBase_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_GameGameModeBase_h_15_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_GameGameModeBase_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	Game_Source_Game_GameGameModeBase_h_15_INCLASS_NO_PURE_DECLS \
	Game_Source_Game_GameGameModeBase_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAME_API UClass* StaticClass<class AGameGameModeBase>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Game_Source_Game_GameGameModeBase_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
