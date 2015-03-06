// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "Networking.h"
#include "GPBuilding.h"
#include "GPFlagPickup.h"
#include "GameFramework/GameMode.h"
#include "KinectInterface.h"
#include "GPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPGameMode : public AGameMode
{
	GENERATED_BODY()

	double tickCount;

	bool IsClear(FVector2D centre, FRotator rotation, FVector scale);

	virtual void StartPlay() override;
	AGPGameMode(const class FObjectInitializer& ObjectInitializer);

	//KinectInterface *kinect = NULL;

	public:
		/** Building class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = Building)
		TSubclassOf<class AGPBuilding> BuildingClass;

		void SpawnBuilding(FVector2D a, FVector2D b);

		UFUNCTION()
		void SpawnBuilding(FVector centre, FRotator rotation, FVector scale);

		void Tick(float DeltaSeconds) override;

		/** Flag Pickup class to spawn */
        UPROPERTY(EditDefaultsOnly, Category = Pickup)
        TSubclassOf<class AGPFlagPickup> FlagPickupClass;

        UFUNCTION()
        void SpawnFlag();

		//////////////////////////////////
		///////// HERE BE DRAGONS ////////
		//////////////////////////////////
		int commstate = 0;

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

		//Rama's StringFromBinaryArray
		FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);

		//~~~ VShow ~~~
		FORCEINLINE void VShow(const TCHAR* Str)
		{
			//ClientMessage(FString(Str));
		}
		FORCEINLINE void VShow(const FString& Str)
		{
			//ClientMessage(Str);
		}
		FORCEINLINE void VShow(const FString& Str, const FString& Str2)
		{
			//ClientMessage(Str + FString(" ") + Str2);
		}
		FORCEINLINE void VShow(FString Str, const float& Value)
		{
			Str += " ";
			Str += FString::SanitizeFloat(Value);
			//ClientMessage(Str);
		}
		FORCEINLINE void VShow(FString Str, const int32& Value)
		{
			Str += " ";
			Str += FString::FromInt(Value);
			//ClientMessage(Str);
		}
		FORCEINLINE void VShow(FString Str, const uint32& Value)
		{
			Str += " ";
			Str += FString::FromInt(Value);
			//ClientMessage(Str);
		}
};

