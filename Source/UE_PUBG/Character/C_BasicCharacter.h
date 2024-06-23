// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_BasicCharacter.generated.h"

UENUM(BlueprintType)
enum class EHandState : uint8
{
	UNARMED,
	WEAWPON_MAIN,
	WEAPON_SUB,
	WEAPON_MELEE,
	WEAPON_THROWABLE
};

UENUM(BlueprintType)
enum class EPoseState : uint8
{
	STAND,
	CROUCH,
	CRAWL
};


UCLASS()
class UE_PUBG_API AC_BasicCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_BasicCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: // Getters and setters

	EHandState GetHandState() const { return HandState; }
	EPoseState GetPoseState() const { return PoseState; }

protected:

	// Current hand state
	EHandState HandState{};

	// Current post state 
	EPoseState PoseState{};

};
