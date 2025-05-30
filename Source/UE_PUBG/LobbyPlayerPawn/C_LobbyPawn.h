// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "C_LobbyPawn.generated.h"

UCLASS()
class UE_PUBG_API AC_LobbyPawn : public APawn
{
	GENERATED_BODY()

public:
	
	AC_LobbyPawn();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/// <summary>
	/// 위치 Lerp 처리 
	/// </summary>
	void HandleLerpLocation(float DeltaTime);

public:
	
	/// <summary>
	/// Log-In -> MainLobby 진입 시, 위치 이동 처리 
	/// </summary>
	void SetLocationLerpDestinationToMainLobby() { CurrentXLocationLerpDest = MainLobbyXLocation; }

	UFUNCTION(BlueprintImplementableEvent)
	void InitCharacterNameTag();

private:

	const float LogInXLocation		= -100.f; 
	const float MainLobbyXLocation	= 639.733324f;

	float CurrentXLocationLerpDest = MainLobbyXLocation;

protected:
	
	UPROPERTY()
	class UCameraComponent* CameraComponent{};

	// Camera Shake
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass{};

};
