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

	class AC_Weapon* CurWeapon{};	// ���� �տ� ��� �ִ� ���� (���ٸ� nullptr)
	
	class AC_Weapon* NextWeapon{};	// ���� ��ü �� �̿�, �������� �տ� �� ����

protected:

	class AC_Gun* MainGun{};	// ������ �ֹ��� 1
	class AC_Gun* SubGun{};	// ������ �ֹ��� 2

	class AC_MeleeWeapon* MeleeWeapon{};	// ������ ��������

	class AC_ThrowingWeapon* C_ThrowingWeapon{};	// ������ ��ô�� ����

protected:

	// class UC_EquippedArmorComponent* ArmorComponent{};

};
