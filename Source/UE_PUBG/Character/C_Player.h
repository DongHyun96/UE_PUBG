// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "Character/C_BasicCharacter.h"
#include "C_Player.generated.h"


/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Player : public AC_BasicCharacter
{
	GENERATED_BODY()

public:
	AC_Player();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Walk(const struct FInputActionValue& Value);

	void Crouch();
	void Crawl();

	void OnJump();


protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UC_InputComponent* MyInputComponent{};

};
