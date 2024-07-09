// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Weapon/C_Weapon.h"
#include "C_Gun.generated.h"
UENUM(BlueprintType)
enum class EGunState : uint8
{
	MAIN_GUN,
	SUB_GUN
};
UCLASS()
class UE_PUBG_API AC_Gun : public AC_Weapon
{
	GENERATED_BODY()
	
public:	
	AC_Gun();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	bool AttachToHolster(class USceneComponent* InParent) override;
	bool AttachToHand(class USceneComponent* InParent) override;
	void ChangeGunState(EGunState InGunState) { CurState = InGunState; }
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* CurDrawMontageSub{}; // 현재 무기 위치에 해당하는 무기 뽑기 Anim montage

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* CurSheathMontageSub{}; // 현재 무기 위치에 해당하는 무기 집어넣기 Anim montage
private:
	const FName HOLSTER_SOCKET_NAME = "Pan_Holster"; // 무기집 socket 이름
	const FName EQUIPPED_SOCKET_NAME = "Pan_Equip"; // 무기가 손에 부착될 socket 이름
	EGunState CurState = EGunState::MAIN_GUN;
};
