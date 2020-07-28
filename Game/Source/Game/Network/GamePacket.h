#pragma once

#include "Network/NetworkComponent/NetworkHeader.h"
#include "Network/NetworkComponent/Packet.h"
#include "Network/NetworkComponent/Console.h"

#include "CoreMinimal.h"

class GAME_API cInfoOfScoreBoard
{
public:
	int Ping;
	string ID;
	string State;
	int Level;
	int Kill;
	int Death;

public:
	cInfoOfScoreBoard()
	{
		Ping = 0;
		ID = "NULL";
		State = "관전중";
		Level = 1;
		Kill = 0;
		Death = 0;
	}
	~cInfoOfScoreBoard()
	{

	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfScoreBoard& Info)
	{
		Stream << Info.Ping << ' ';
		Stream << CPacket::ReplaceCharInString(Info.ID, ' ', '_') << ' ';
		Stream << CPacket::ReplaceCharInString(Info.State, ' ', '_') << ' ';
		Stream << Info.Level << ' ';
		Stream << Info.Kill << ' ';
		Stream << Info.Death << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfScoreBoard& Info)
	{
		Stream >> Info.Ping;
		Stream >> Info.ID;
		Info.ID = CPacket::ReplaceCharInString(Info.ID, '_', ' ');
		Stream >> Info.State;
		Info.State = CPacket::ReplaceCharInString(Info.State, '_', ' ');
		Stream >> Info.Level;
		Stream >> Info.Kill;
		Stream >> Info.Death;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfScoreBoard> Ping: %d, ID: %s, State: %s, Level: %d, Kill: %d, Death: %d \n", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), Ping, ID.c_str(), State.c_str(), Level, Kill, Death);
	}

	// For Sort()
	bool operator<(cInfoOfScoreBoard& other) const
	{
		if (this->Kill == other.Kill)
			return this->Level > other.Level;
		else
			return this->Kill > other.Kill;
	}
};


class GAME_API cInfoOfSpaceShip
{
public:
	int State;

	float LocX;
	float LocY;
	float LocZ;

	bool bHiddenInGame;
	bool bSimulatePhysics;
	float ScaleOfEngineParticleSystem;
	float AccelerationZ;
	bool bEngine;

public:
	cInfoOfSpaceShip()
	{
		State = 0;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		bHiddenInGame = false;
		bSimulatePhysics = true;
		ScaleOfEngineParticleSystem = 0.010f;
		AccelerationZ = 980.0f;
		bEngine = false;
	}
	~cInfoOfSpaceShip()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfSpaceShip& Info)
	{
		Stream << Info.State << ' ';
		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';
		Stream << Info.bHiddenInGame << ' ';
		Stream << Info.bSimulatePhysics << ' ';
		Stream << Info.ScaleOfEngineParticleSystem << ' ';
		Stream << Info.AccelerationZ << ' ';
		Stream << Info.bEngine << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfSpaceShip& Info)
	{
		Stream >> Info.State;
		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;
		Stream >> Info.bHiddenInGame;
		Stream >> Info.bSimulatePhysics;
		Stream >> Info.ScaleOfEngineParticleSystem;
		Stream >> Info.AccelerationZ;
		Stream >> Info.bEngine;

		return Stream;
	}

	
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfSpaceShip> State: %d, LocX: %f, LocY: %f, LocZ: %f, bHiddenInGame: %s, bSimulatePhysics: %s, ScaleOfEngineParticleSystem: %f, AccelerationZ: %f, bEngine: %s \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), State, LocX, LocY, LocZ, (bHiddenInGame == true) ? "true" : "false", (bSimulatePhysics == true) ? "true" : "false", ScaleOfEngineParticleSystem, AccelerationZ, (bEngine == true) ? "true" : "false");
	}

	void SetInfo(int State_, FVector Location, bool bHiddenInGame_, bool bSimulatePhysics_, float ScaleOfEngineParticleSystem_, float AccelerationZ_, bool bEngine_)
	{
		this->State = State_;

		LocX = Location.X;
		LocY = Location.Y;
		LocZ = Location.Z;

		this->bHiddenInGame = bHiddenInGame_;
		this->bSimulatePhysics = bSimulatePhysics_;
		this->ScaleOfEngineParticleSystem = ScaleOfEngineParticleSystem_;
		this->AccelerationZ = AccelerationZ_;
		this->bEngine = bEngine_;
	}
};


class GAME_API cInfoOfPioneer_Socket
{
public:
	int ID;

	int SocketID;
	string NameOfID;

public:
	cInfoOfPioneer_Socket()
	{
		ID = 0;

		SocketID = 0;
		NameOfID = "AI";
	}
	~cInfoOfPioneer_Socket()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer_Socket& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.SocketID << ' ';
		Stream << CPacket::ReplaceCharInString(Info.NameOfID, ' ', '_') << ' ';
		Stream << Info.NameOfID << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer_Socket& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.SocketID;
		Stream >> Info.NameOfID;
		Info.NameOfID = CPacket::ReplaceCharInString(Info.NameOfID, '_', ' ');

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Socket> ID: %d, SocketID : %d, NameOfID: %s \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, SocketID, NameOfID.c_str());
	}
};

class GAME_API cInfoOfPioneer_Animation
{
public:
	int ID;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;

	float TargetRotX;
	float TargetRotY;
	float TargetRotZ;


	// 애니메이션 변수들
	float VelocityX;
	float VelocityY;
	float VelocityZ;

	bool bHasPistolType;
	bool bHasRifleType;
	bool bHasLauncherType;

	bool bFired;

	float BoneSpineRotX;
	float BoneSpineRotY;
	float BoneSpineRotZ;


	// 무기
	int IdxOfCurrentWeapon;

	bool bArmedWeapon;


public:
	cInfoOfPioneer_Animation()
	{
		ID = 0;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		TargetRotX = 0.0f;
		TargetRotY = 0.0f;
		TargetRotZ = 0.0f;


		VelocityX = 0.0f;
		VelocityY = 0.0f;
		VelocityZ = 0.0f;

		bHasPistolType = false;
		bHasRifleType = false;
		bHasLauncherType = false;

		bFired = false;

		BoneSpineRotX = 0.0f;
		BoneSpineRotY = 0.0f;
		BoneSpineRotZ = 0.0f;


		IdxOfCurrentWeapon = 0;

		bArmedWeapon = false;
	}
	~cInfoOfPioneer_Animation()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer_Animation& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';
		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';
		Stream << Info.TargetRotX << ' ';
		Stream << Info.TargetRotY << ' ';
		Stream << Info.TargetRotZ << ' ';

		Stream << Info.VelocityX << ' ';
		Stream << Info.VelocityY << ' ';
		Stream << Info.VelocityZ << ' ';
		Stream << Info.bHasPistolType << ' ';
		Stream << Info.bHasRifleType << ' ';
		Stream << Info.bHasLauncherType << ' ';
		Stream << Info.bFired << ' ';
		Stream << Info.BoneSpineRotX << ' ';
		Stream << Info.BoneSpineRotY << ' ';
		Stream << Info.BoneSpineRotZ << ' ';

		Stream << Info.IdxOfCurrentWeapon << ' ';
		Stream << Info.bArmedWeapon << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer_Animation& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;
		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;
		Stream >> Info.TargetRotX;
		Stream >> Info.TargetRotY;
		Stream >> Info.TargetRotZ;

		Stream >> Info.VelocityX;
		Stream >> Info.VelocityY;
		Stream >> Info.VelocityZ;
		Stream >> Info.bHasPistolType;
		Stream >> Info.bHasRifleType;
		Stream >> Info.bHasLauncherType;
		Stream >> Info.bFired;
		Stream >> Info.BoneSpineRotX;
		Stream >> Info.BoneSpineRotY;
		Stream >> Info.BoneSpineRotZ;

		Stream >> Info.IdxOfCurrentWeapon;
		Stream >> Info.bArmedWeapon;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Animation> ID: %d, RotX: %f, RotY: %f, RotZ: %f, LocX: %f, LocY: %f, LocZ: %f, TargetRotX: %f, TargetRotY: %f, TargetRotZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, RotX, RotY, RotZ, LocX, LocY, LocZ, TargetRotX, TargetRotY, TargetRotZ);
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Animation> VelocityX: %f, VelocityY: %f, VelocityZ: %f, bHasPistolType: %s, bHasRifleType : %s, bHasLauncherType: %s, bFired: %s, TargetRotX: %f, TargetRotY: %f, TargetRotZ: %f  \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), VelocityX, VelocityY, VelocityZ, (bHasPistolType == true) ? "true" : "false", (bHasRifleType == true) ? "true" : "false", (bHasLauncherType == true) ? "true" : "false", (bFired == true) ? "true" : "false", BoneSpineRotX, BoneSpineRotY, BoneSpineRotZ);
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Animation> IdxOfCurrentWeapon: %d, bArmedWeapon: %s \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), IdxOfCurrentWeapon, (bArmedWeapon == true) ? "true" : "false");
	}

	void SetActorTransform(const FTransform& Transform)
	{
		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};

class GAME_API cInfoOfPioneer_Stat
{
public:
	int ID;

	float HealthPoint;
	float MaxHealthPoint;

	float MoveSpeed;
	float AttackSpeed;

	float AttackPower;

	float SightRange;
	float DetectRange;
	float AttackRange;

	float Exp;
	int Level;


public:
	cInfoOfPioneer_Stat()
	{
		ID = 0;

		HealthPoint = 100.0f;
		MaxHealthPoint = 100.0f;

		MoveSpeed = 10.0f;
		AttackSpeed = 1.0f;

		AttackPower = 1.0f;

		SightRange = 32.0f;
		DetectRange = 32.0f;
		AttackRange = 16.0f;

		Exp = 0.0f;
		Level = 1;
	}
	~cInfoOfPioneer_Stat()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer_Stat& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.HealthPoint << ' ';
		Stream << Info.MaxHealthPoint << ' ';
		Stream << Info.MoveSpeed << ' ';
		Stream << Info.AttackSpeed << ' ';
		Stream << Info.AttackPower << ' ';
		Stream << Info.SightRange << ' ';
		Stream << Info.DetectRange << ' ';
		Stream << Info.AttackRange << ' ';

		Stream << Info.Exp << ' ';
		Stream << Info.Level << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer_Stat& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.HealthPoint;
		Stream >> Info.MaxHealthPoint;
		Stream >> Info.MoveSpeed;
		Stream >> Info.AttackSpeed;
		Stream >> Info.AttackPower;
		Stream >> Info.SightRange;
		Stream >> Info.DetectRange;
		Stream >> Info.AttackRange;

		Stream >> Info.Exp;
		Stream >> Info.Level;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Stat> ID: %d, HealthPoint: %f, MaxHealthPoint : %f, MoveSpeed: %f, AttackSpeed: %f, AttackPower: %f, SightRange: %f, DetectRange: %f, AttackRange: %f, Exp: %f, Level: %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, HealthPoint, MaxHealthPoint, MoveSpeed, AttackSpeed, AttackPower, SightRange, DetectRange, AttackRange, Exp, Level);
	}

};

// 전부 합친거
class GAME_API cInfoOfPioneer
{
public:
	int ID;

	cInfoOfPioneer_Socket Socket;
	cInfoOfPioneer_Animation Animation;
	cInfoOfPioneer_Stat Stat;

public:
	cInfoOfPioneer()
	{
		ID = 0;


	}
	cInfoOfPioneer(int ID, cInfoOfPioneer_Socket Socket, cInfoOfPioneer_Animation Animation, cInfoOfPioneer_Stat Stat)
		: ID(ID), Socket(Socket), Animation(Animation), Stat(Stat)
	{
	}
	~cInfoOfPioneer()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Socket << ' ';
		Stream << Info.Animation << ' ';
		Stream << Info.Stat << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Socket;
		Stream >> Info.Animation;
		Stream >> Info.Stat;

		return Stream;
	}
	
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		Socket.PrintInfo();
		Animation.PrintInfo();
		Stat.PrintInfo();
	}
};

class GAME_API cInfoOfProjectile
{
public:
	int ID; // Pioneer::ID;

	int Numbering;

	float ScaleX;
	float ScaleY;
	float ScaleZ;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;

public:
	cInfoOfProjectile()
	{
		ID = 0;

		Numbering = 0;

		ScaleX = 0.0f;
		ScaleY = 0.0f;
		ScaleZ = 0.0f;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;
	}
	~cInfoOfProjectile()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfProjectile& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Numbering << ' ';

		Stream << Info.ScaleX << ' ';
		Stream << Info.ScaleY << ' ';
		Stream << Info.ScaleZ << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';

		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfProjectile& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Numbering;

		Stream >> Info.ScaleX;
		Stream >> Info.ScaleY;
		Stream >> Info.ScaleZ;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;

		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfProjectile> ID: %d, Numbering : %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, Numbering);
		CONSOLE_LOG("%s%s<cInfoOfProjectile> ScaleX: %f, ScaleY : %f, ScaleZ: %f, RotX: %f, RotY : %f, RotZ: %f, LocX: %f, LocY : %f, LocZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ScaleX, ScaleY, ScaleZ, RotX, RotY, RotZ, LocX, LocY, LocZ);
	}

	void SetActorTransform(const FTransform& Transform)
	{
		ScaleX = Transform.GetScale3D().X;
		ScaleY = Transform.GetScale3D().Y;
		ScaleZ = Transform.GetScale3D().Z;

		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;

		transform.SetScale3D(FVector(ScaleX, ScaleY, ScaleZ));
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};

class GAME_API cInfoOfResources
{
public:
	float NumOfMineral;
	float NumOfOrganic;
	float NumOfEnergy;

public:
	cInfoOfResources()
	{
		NumOfMineral = 100.0f;
		NumOfOrganic = 50.0f;
		NumOfEnergy = 100.0f;
	}
	~cInfoOfResources()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfResources& Info)
	{
		Stream << Info.NumOfMineral << ' ';
		Stream << Info.NumOfOrganic << ' ';
		Stream << Info.NumOfEnergy << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfResources& Info)
	{
		Stream >> Info.NumOfMineral;
		Stream >> Info.NumOfOrganic;
		Stream >> Info.NumOfEnergy;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfResources> NumOfMineral: %f, NumOfOrganic: %f, NumOfEnergy: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), NumOfMineral, NumOfOrganic, NumOfEnergy);
	}
};


class GAME_API cInfoOfBuilding_Spawn
{
public:
	int ID; // BuildingManager::ID

	int Numbering; // 1~8

	float NeedMineral;
	float NeedOrganicMatter;

	float ScaleX;
	float ScaleY;
	float ScaleZ;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;


	// 터렛에서만 사용
	int IdxOfUnderWall;

public:
	cInfoOfBuilding_Spawn()
	{
		ID = 0;

		Numbering = 0;

		NeedMineral = 0.0f;
		NeedOrganicMatter = 0.0f;

		ScaleX = 0.0f;
		ScaleY = 0.0f;
		ScaleZ = 0.0f;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		IdxOfUnderWall = 0;
	}
	~cInfoOfBuilding_Spawn()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfBuilding_Spawn& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Numbering << ' ';

		Stream << Info.NeedMineral << ' ';
		Stream << Info.NeedOrganicMatter << ' ';

		Stream << Info.ScaleX << ' ';
		Stream << Info.ScaleY << ' ';
		Stream << Info.ScaleZ << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';

		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';

		Stream << Info.IdxOfUnderWall << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfBuilding_Spawn& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Numbering;

		Stream >> Info.NeedMineral;
		Stream >> Info.NeedOrganicMatter;

		Stream >> Info.ScaleX;
		Stream >> Info.ScaleY;
		Stream >> Info.ScaleZ;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;

		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;

		Stream >> Info.IdxOfUnderWall;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfBuilding_Spawn> ID: %d, Numbering : %d, NeedMineral: %f, NeedOrganicMatter: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, Numbering, NeedMineral, NeedOrganicMatter);
		CONSOLE_LOG("%s%s<cInfoOfBuilding_Spawn> ScaleX: %f, ScaleY : %f, ScaleZ: %f, RotX: %f, RotY : %f, RotZ: %f, LocX: %f, LocY : %f, LocZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ScaleX, ScaleY, ScaleZ, RotX, RotY, RotZ, LocX, LocY, LocZ);
	}

	void SetActorTransform(const FTransform& Transform)
	{
		ScaleX = Transform.GetScale3D().X;
		ScaleY = Transform.GetScale3D().Y;
		ScaleZ = Transform.GetScale3D().Z;

		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;

		transform.SetScale3D(FVector(ScaleX, ScaleY, ScaleZ));
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};


class GAME_API cInfoOfBuilding_Stat
{
public:
	int ID; // BuildingManager::ID
	int BuildingState; // EBuildingState
	float HealthPoint;

public:
	cInfoOfBuilding_Stat()
	{
		ID = 0;
		BuildingState = 1;
		HealthPoint = 100.0f;
	}
	~cInfoOfBuilding_Stat()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfBuilding_Stat& Info)
	{
		Stream << Info.ID << ' ';
		Stream << Info.BuildingState << ' ';
		Stream << Info.HealthPoint << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfBuilding_Stat& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.BuildingState;
		Stream >> Info.HealthPoint;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfBuilding_Stat> ID: %d, BuildingState : %d, HealthPoint: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, BuildingState, HealthPoint);
	}

};

class GAME_API cInfoOfBuilding
{
public:
	int ID;

	cInfoOfBuilding_Spawn Spawn;
	cInfoOfBuilding_Stat Stat;

public:
	cInfoOfBuilding()
	{
		ID = 0;


	}
	cInfoOfBuilding(int ID, cInfoOfBuilding_Spawn Spawn, cInfoOfBuilding_Stat Stat)
		: ID(ID), Spawn(Spawn), Stat(Stat)
	{
	}
	~cInfoOfBuilding()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfBuilding& Info)
	{
		Stream << Info.ID << ' ';
		Stream << Info.Spawn << ' ';
		Stream << Info.Stat << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfBuilding& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.Spawn;
		Stream >> Info.Stat;

		return Stream;
	}
	
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		Spawn.PrintInfo();
		Stat.PrintInfo();
	}

};


class GAME_API cInfoOfEnemy_Spawn
{
public:
	int ID;
	int EnemyType;

public:
	cInfoOfEnemy_Spawn()
	{
		ID = 0;
		EnemyType = 0;
	}
	~cInfoOfEnemy_Spawn()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy_Spawn& Info)
	{
		Stream << Info.ID << ' ';
		Stream << Info.EnemyType << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy_Spawn& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.EnemyType;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Spawn> ID: %d, EnemyType : %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, EnemyType);
	}
};

class GAME_API cInfoOfEnemy_Animation
{
public:
	int ID;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;

	float TargetRotX;
	float TargetRotY;
	float TargetRotZ;

	// 애니메이션 변수들
	float VelocityX;
	float VelocityY;
	float VelocityZ;

	// FSM
	int State;

public:
	cInfoOfEnemy_Animation()
	{
		ID = 0;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		TargetRotX = 0.0f;
		TargetRotY = 0.0f;
		TargetRotZ = 0.0f;

		VelocityX = 0.0f;
		VelocityY = 0.0f;
		VelocityZ = 0.0f;

		State = 0;
	}
	~cInfoOfEnemy_Animation()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy_Animation& Info)
	{
		Stream << Info.ID << ' ';
		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';
		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';
		Stream << Info.TargetRotX << ' ';
		Stream << Info.TargetRotY << ' ';
		Stream << Info.TargetRotZ << ' ';
		Stream << Info.VelocityX << ' ';
		Stream << Info.VelocityY << ' ';
		Stream << Info.VelocityZ << ' ';
		Stream << Info.State << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy_Animation& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;
		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;
		Stream >> Info.TargetRotX;
		Stream >> Info.TargetRotY;
		Stream >> Info.TargetRotZ;
		Stream >> Info.VelocityX;
		Stream >> Info.VelocityY;
		Stream >> Info.VelocityZ;
		Stream >> Info.State;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Animation> ID: %d, RotX: %f, RotY: %f, RotZ: %f, LocX: %f, LocY: %f, LocZ: %f, TargetRotX: %f, TargetRotY: %f, TargetRotZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, RotX, RotY, RotZ, LocX, LocY, LocZ, TargetRotX, TargetRotY, TargetRotZ);
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Animation> VelocityX: %f, VelocityY: %f, VelocityZ: %f, State: %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), VelocityX, VelocityY, VelocityZ, State);
	}

	void SetActorTransform(const FTransform& Transform)
	{
		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};

class GAME_API cInfoOfEnemy_Stat
{
public:
	int ID;

	float HealthPoint;
	float MaxHealthPoint;

	float MoveSpeed;
	float AttackSpeed;

	float AttackPower;

	float SightRange;
	float DetectRange;
	float AttackRange;

public:
	cInfoOfEnemy_Stat()
	{
		ID = 0;

		HealthPoint = 100.0f;
		MaxHealthPoint = 100.0f;

		MoveSpeed = 10.0f;
		AttackSpeed = 1.0f;

		AttackPower = 10.0f;

		SightRange = 32.0f;
		DetectRange = 32.0f;
		AttackRange = 4.0f;
	}
	~cInfoOfEnemy_Stat()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy_Stat& Info)
	{
		Stream << Info.ID << ' ';
		Stream << Info.HealthPoint << ' ';
		Stream << Info.MaxHealthPoint << ' ';
		Stream << Info.MoveSpeed << ' ';
		Stream << Info.AttackSpeed << ' ';
		Stream << Info.AttackPower << ' ';
		Stream << Info.SightRange << ' ';
		Stream << Info.DetectRange << ' ';
		Stream << Info.AttackRange << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy_Stat& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.HealthPoint;
		Stream >> Info.MaxHealthPoint;
		Stream >> Info.MoveSpeed;
		Stream >> Info.AttackSpeed;
		Stream >> Info.AttackPower;
		Stream >> Info.SightRange;
		Stream >> Info.DetectRange;
		Stream >> Info.AttackRange;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Stat> ID: %d, HealthPoint: %f, MaxHealthPoint : %f, MoveSpeed: %f, AttackSpeed: %f, AttackPower: %f, SightRange: %f, DetectRange: %f, AttackRange: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, HealthPoint, MaxHealthPoint, MoveSpeed, AttackSpeed, AttackPower, SightRange, DetectRange, AttackRange);
	}

};

class GAME_API cInfoOfEnemy
{
public:
	int ID;

	cInfoOfEnemy_Spawn Spawn;
	cInfoOfEnemy_Animation Animation;
	cInfoOfEnemy_Stat Stat;

public:
	cInfoOfEnemy()
	{
		ID = 0;


	}
	cInfoOfEnemy(int ID, cInfoOfEnemy_Spawn Spawn, cInfoOfEnemy_Animation Animation, cInfoOfEnemy_Stat Stat)
		: ID(ID), Spawn(Spawn), Animation(Animation), Stat(Stat)
	{
	}
	~cInfoOfEnemy()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy& Info)
	{
		Stream << Info.ID << ' ';
		Stream << Info.Spawn << ' ';
		Stream << Info.Animation << ' ';
		Stream << Info.Stat << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.Spawn;
		Stream >> Info.Animation;
		Stream >> Info.Stat;

		return Stream;
	}

	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		Spawn.PrintInfo();
		Animation.PrintInfo();
		Stat.PrintInfo();
	}

};