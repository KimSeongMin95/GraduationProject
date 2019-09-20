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

#define Game_Source_Game_PioneerManager_h_12_RPC_WRAPPERS
#define Game_Source_Game_PioneerManager_h_12_RPC_WRAPPERS_NO_PURE_DECLS
#define Game_Source_Game_PioneerManager_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPioneerManager(); \
	friend struct Z_Construct_UClass_APioneerManager_Statics; \
public: \
	DECLARE_CLASS(APioneerManager, AActor, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneerManager)


#define Game_Source_Game_PioneerManager_h_12_INCLASS \
private: \
	static void StaticRegisterNativesAPioneerManager(); \
	friend struct Z_Construct_UClass_APioneerManager_Statics; \
public: \
	DECLARE_CLASS(APioneerManager, AActor, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Game"), NO_API) \
	DECLARE_SERIALIZER(APioneerManager)


#define Game_Source_Game_PioneerManager_h_12_STANDARD_CONSTRUCTORS \
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


#define Game_Source_Game_PioneerManager_h_12_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APioneerManager(APioneerManager&&); \
	NO_API APioneerManager(const APioneerManager&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APioneerManager); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APioneerManager); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APioneerManager)


#define Game_Source_Game_PioneerManager_h_12_PRIVATE_PROPERTY_OFFSET
#define Game_Source_Game_PioneerManager_h_9_PROLOG
#define Game_Source_Game_PioneerManager_h_12_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_PioneerManager_h_12_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_PioneerManager_h_12_RPC_WRAPPERS \
	Game_Source_Game_PioneerManager_h_12_INCLASS \
	Game_Source_Game_PioneerManager_h_12_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Game_Source_Game_PioneerManager_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Game_Source_Game_PioneerManager_h_12_PRIVATE_PROPERTY_OFFSET \
	Game_Source_Game_PioneerManager_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	Game_Source_Game_PioneerManager_h_12_INCLASS_NO_PURE_DECLS \
	Game_Source_Game_PioneerManager_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAME_API UClass* StaticClass<class APioneerManager>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Game_Source_Game_PioneerManager_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
