// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Game/PathFinding.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePathFinding() {}
// Cross Module References
	GAME_API UEnum* Z_Construct_UEnum_Game_PathFindAlgorithm();
	UPackage* Z_Construct_UPackage__Script_Game();
// End Cross Module References
	static UEnum* PathFindAlgorithm_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_Game_PathFindAlgorithm, Z_Construct_UPackage__Script_Game(), TEXT("PathFindAlgorithm"));
		}
		return Singleton;
	}
	template<> GAME_API UEnum* StaticEnum<PathFindAlgorithm>()
	{
		return PathFindAlgorithm_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_PathFindAlgorithm(PathFindAlgorithm_StaticEnum, TEXT("/Script/Game"), TEXT("PathFindAlgorithm"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_Game_PathFindAlgorithm_Hash() { return 1058975193U; }
	UEnum* Z_Construct_UEnum_Game_PathFindAlgorithm()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_Game();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("PathFindAlgorithm"), 0, Get_Z_Construct_UEnum_Game_PathFindAlgorithm_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "PFA_NaveMesh", (int64)PFA_NaveMesh },
				{ "PFA_AStar", (int64)PFA_AStar },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "ModuleRelativePath", "PathFinding.h" },
				{ "PFA_AStar.Name", "PFA_AStar" },
				{ "PFA_NaveMesh.Comment", "/*\n\x09?\xf0\xb8\xae\xbe\xf3\xbf\xa1\xbc? NavMeshBoundsVulume?? ?\xcc\xbf??? ?????? ???????\xcc\xbc? ?\xcb\xb0??????? ?????\xd5\xb4\xcf\xb4?.\n\x09NavMeshBoundsVolume?? ?????? ??\xc4\xa1?? ???\xc2\xb7? P\xc5\xb0?? ?????? NavMesh?? \xc4\xbf???\xcf\xb4? ?????? \xc7\xa5??/???????\xcf\xb4?.\n\x09*/" },
				{ "PFA_NaveMesh.Name", "PFA_NaveMesh" },
				{ "PFA_NaveMesh.ToolTip", "?\xf0\xb8\xae\xbe\xf3\xbf\xa1\xbc? NavMeshBoundsVulume?? ?\xcc\xbf??? ?????? ???????\xcc\xbc? ?\xcb\xb0??????? ?????\xd5\xb4\xcf\xb4?.\nNavMeshBoundsVolume?? ?????? ??\xc4\xa1?? ???\xc2\xb7? P\xc5\xb0?? ?????? NavMesh?? \xc4\xbf???\xcf\xb4? ?????? \xc7\xa5??/???????\xcf\xb4?." },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_Game,
				nullptr,
				"PathFindAlgorithm",
				"PathFindAlgorithm",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				RF_Public|RF_Transient|RF_MarkAsNative,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				(uint8)UEnum::ECppForm::Regular,
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
