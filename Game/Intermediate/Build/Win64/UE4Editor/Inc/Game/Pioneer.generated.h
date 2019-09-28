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

#define Game_Source_Game_Pioneer_h_24_RPC_WRAPPERS
#define Game_Source_Game_Pioneer_h_24_RPC_WRAPPERS_NO_PURE_DECLS
#define Game_Source_Game_Pioneer_h_24_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPioneer(); \
	friend struct Z_Construct_UClass_APioneer_Statics; \
public: \
	DECLARE_CLASS(APioneer, ACharacter, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneer)


#define Game_Source_Game_Pioneer_h_24_INCLASS \
private: \
	static void StaticRegisterNativesAPioneer(); \
	friend struct Z_Construct_UClass_APioneer_Statics; \
public: \
	DECLARE_CLASS(APioneer, ACharacter, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneer)


#define Game_Source_Game_Pioneer_h_24_STANDARD_CONSTRUCTORS \
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


#define Game_Source_Game_Pioneer_h_24_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneer(APioneer&&); \
	NO_API APioneer(const APioneer&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneer); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneer); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APioneer)


#define Game_Source_Game_Pioneer_h_24_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__StaticMeshComponent() { return STRUCT_OFFSET(APioneer, StaticMeshComponent); } \
	FORCEINLINE static uint32 __PPO__CameraBoom() { return STRUCT_OFFSET(APioneer, CameraBoom); } \
	FORCEINLINE static uint32 __PPO__TopDownCameraComponent() { return STRUCT_OFFSET(APioneer, TopDownCameraComponent); } \
	FORCEINLINE static uint32 __PPO__CursorToWorld() { return STRUCT_OFFSET(APioneer, CursorToWorld); }


#define Game_Source_Game_Pioneer_h_21_PROLOG
#define Game_Source_Game_Pioneer_h_24_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_Pioneer_h_24_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_Pioneer_h_24_RPC_WRAPPERS \
	Game_Source_Game_Pioneer_h_24_INCLASS \
	Game_Source_Game_Pioneer_h_24_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Game_Source_Game_Pioneer_h_24_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_Pioneer_h_24_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_Pioneer_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	Game_Source_Game_Pioneer_h_24_INCLASS_NO_PURE_DECLS \
	Game_Source_Game_Pioneer_h_24_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAME_API UClass* StaticClass<class APioneer>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Game_Source_Game_Pioneer_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
