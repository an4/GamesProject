// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "Networking.h"
#include "GPBuilding.h"
#include "GPFlagPickup.h"
#include "GPGameState.h"
#include "GPHUD.h"
#include "GPPlayerController.h"
#include "GPPlayerState.h"
#include "GPHealthPickup.h"
#include "GPAmmoPickup.h"
#include "GameFramework/GameMode.h"
#include "GPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPGameMode : public AGameMode
{
	GENERATED_BODY()

	double tickCount;

	bool IsClear(FVector2D ctr, FRotator rot, FVector scl);

	virtual void StartPlay() override;
	AGPGameMode(const class FObjectInitializer& ObjectInitializer);

	public:
        virtual UClass* GetDefaultPawnClassForController(AController* InController) OVERRIDE;
        /** Building class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = Building)
		TSubclassOf<class AGPBuilding> BuildingClass;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding)
		bool PathExists;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding)
			bool updated;

        UPROPERTY()
        TSubclassOf<class AGPServerPawn> ServerPawnClass;

		UPROPERTY()
		TSubclassOf<class AGPCaptureZone> CaptureZoneBPClass;

		void SpawnBuilding(FVector2D centre, float rot, FVector2D scale);

		UFUNCTION()
		void SpawnBuilding(FVector centre, FRotator rotation, FVector scale);


		UFUNCTION()
		void SpawnCaptureZone(FVector centre, FRotator rotation, int8 Team);

		void Tick(float DeltaSeconds) override;

		/** Flag Pickup class to spawn */
        UPROPERTY(EditDefaultsOnly, Category = Pickup)
        TSubclassOf<class AGPFlagPickup> FlagPickupClass;

        UFUNCTION()
        void SpawnFlag(int8 Team);

        UFUNCTION()
		void SpawnHealth();

		UFUNCTION(exec)
		void Rescan(const FString &opt);

		void Rescan();

		UFUNCTION(exec)
		void ResetBuildings();

		UFUNCTION(exec)
		void PauseGame();

		UFUNCTION(exec)
		void UnpauseGame();

        UFUNCTION()
        void SpawnAmmo();

		UFUNCTION()
		void EndGame(int8 Team);

		UFUNCTION(BlueprintCallable, Category = Pathfinding)
		void checkPathTrue();

		UFUNCTION(BlueprintCallable, Category = Pathfinding)
		void checkPathFalse();

		//////////////////////////////////
		///////// HERE BE DRAGONS ////////
		//////////////////////////////////
		enum class OCVSProtocolState { INIT, REQUEST, RECEIVE };

		OCVSProtocolState commstate = OCVSProtocolState::INIT;

		enum class ScanRequestState { NONE, SCAN, DEBUG, INTERACTIVE };
		ScanRequestState wantScan = ScanRequestState::NONE;

		void VectorFromTArray(TArray<uint8> &arr, std::vector<char> &vec);

		virtual void EndPlay(EEndPlayReason::Type reason) override;

		FSocket* ListenerSocket;
		FSocket* ConnectionSocket;
		FIPv4Endpoint RemoteAddressForConnection;

		bool StartTCPReceiver(
			const FString& YourChosenSocketName,
			const FString& TheIP,
			const int32 ThePort
			);

		FSocket* CreateTCPConnectionListener(
			const FString& YourChosenSocketName,
			const FString& TheIP,
			const int32 ThePort,
			const int32 ReceiveBufferSize = 2 * 1024 * 1024
			);

		//Timer functions, could be threads
		void TCPConnectionListener(); 	//can thread this eventually
		void TCPSocketListener();		//can thread this eventually


		//Format String IP4 to number array
		bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
};

