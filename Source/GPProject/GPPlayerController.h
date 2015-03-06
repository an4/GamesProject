// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "Networking.h"
#include "GameFramework/PlayerController.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;

	//handles moving forward/backward
    UFUNCTION()
    void MoveForward(float Val);
    //handles strafing
    UFUNCTION()
    void MoveRight(float Val);

	//handles turning
	UFUNCTION()
	void AddControllerYawInput(float Value);
	//handles up/down look
	UFUNCTION()
	void AddControllerPitchInput(float Value);

    //sets jump flag when key is pressed
    UFUNCTION()
    void OnStartJump();
    //clears jump flag when key is released
    UFUNCTION()
	void OnStopJump();

	//handles firing
	void OnFire();

	//handles bomb launching
	void OnBombLaunch();

	//handles bomb detonation
	void OnBombDetonate();
	//UFUNCTION(Reliable, Server, WithValidation)
	//void ServerOnFire();

	//// Following code taken from the replication wiki.

	//bool bSomeBool;

	//void SetSomeBool(bool bNewSomeBool);

	//UFUNCTION(reliable, server, WithValidation)
	//void ServerSetSomeBool(bool bNewSomeBool);
	


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
		ClientMessage(FString(Str));
	}
	FORCEINLINE void VShow(const FString& Str)
	{
		ClientMessage(Str);
	}
	FORCEINLINE void VShow(const FString& Str, const FString& Str2)
	{
		ClientMessage(Str + FString(" ") + Str2);
	}
	FORCEINLINE void VShow(FString Str, const float& Value)
	{
		Str += " ";
		Str += FString::SanitizeFloat(Value);
		ClientMessage(Str);
	}
	FORCEINLINE void VShow(FString Str, const int32& Value)
	{
		Str += " ";
		Str += FString::FromInt(Value);
		ClientMessage(Str);
	}
	FORCEINLINE void VShow(FString Str, const uint32& Value)
	{
		Str += " ";
		Str += FString::FromInt(Value);
		ClientMessage(Str);
	}
};
