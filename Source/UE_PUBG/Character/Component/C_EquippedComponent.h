// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_EquippedComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_EquippedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_EquippedComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	class AC_Weapon* GetCurWeapon() const { return CurWeapon; }

protected:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	class AC_Weapon* CurWeapon{};	// 현재 손에 들고 있는 무기 (없다면 nullptr)
	
	class AC_Weapon* NextWeapon{};	// 무기 교체 시 이용, 다음으로 손에 들 무기

protected:

	class AC_Gun* MainGun{};	// 장착된 주무기 1
	class AC_Gun* SubGun{};	// 장착된 주무기 2

	class AC_MeleeWeapon* MeleeWeapon{};	// 장착된 근접무기

	class AC_ThrowingWeapon* C_ThrowingWeapon{};	// 장착된 투척류 무기

protected:

	// class UC_EquippedArmorComponent* ArmorComponent{};

};
