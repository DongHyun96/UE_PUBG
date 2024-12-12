// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/C_Weapon.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_MeleeWeapon : public AC_Weapon
{
	GENERATED_BODY()

public:

	AC_MeleeWeapon();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	bool AttachToHolster(class USceneComponent* InParent) override;

	bool AttachToHand(class USceneComponent* InParent) override;

	void PickUpItem(class AC_BasicCharacter* Character) override;

	bool Interaction(AC_BasicCharacter* Character) override;

	/// <summary>
	/// �ش� ������(��ü)�� �󸶳� �κ��� ���� �� �ִ��� ����ؼ� �ִ´�. -> �Ϻθ� �ִ� ��쿡�� �κ��� ���� �� Spawn�� ���� ���ο� ��ü�� ����� 
	/// ������ �������ְ� ������ ������ ����. (����stack = ������ ������ stack + ������ü�� stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool MoveToInven(AC_BasicCharacter* Character) override;

	/// <summary>
	/// �ش� ������(��ü)�� �󸶳� ���� ������ ����ؼ� ������.(�̰��� �Ŀ� ���������� ����� ���� uint8������ �޾� ����ϸ� ������.)
	/// ->�Ϻθ� ������ ��� ������(��ü)�� ���� �����ؼ� ������ ������ �����ؾ� �Ѵ�. (����stack = ������ ������ stack + ������ü�� stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool MoveToAround(AC_BasicCharacter* Character) override;

	/// <summary>
	/// �������� �������� �̵�.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns>true�� �̵� ����, false�� ����</returns>
	bool MoveToSlot(AC_BasicCharacter* Character) override;

public:

	struct FPriorityAnimMontage GetAttackMontage() const { return AttackMontage; }

public:

	UFUNCTION(BlueprintCallable)
	void SetAttackColliderEnabled(const bool& Enabled);

protected:

	UFUNCTION()
	void OnBodyColliderBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

protected:

	/// <summary>
	/// OwnerCharacter�� Pose Transition ����� ������ �� Delegate�� ���� call back�� �޴� �Լ� (���� ĳ������ slot�� ������ ���⸸ call back �� ����) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

private:

	/// <summary>
	/// <para> �ڼ� �� AnimMontage�� Play�ϰ� �ִ� ����, OwnerCharacter�� �ڼ��� �ٲ� �� </para>
	/// <para> ���� ��� ���� AnimMontage�� �����ġ ������ ���� ���� �ڼ��� Montage�� ������ġ�� �����Ͽ� ��� </para>
	/// </summary>
	/// <param name="PrevMontage"> ���� ������� ��Ÿ�� </param>
	/// <param name="NextMontage"> �ٲ� ��Ÿ�� </param>
	//void SwitchCurrentPlayingMontage(FPriorityAnimMontage CurrentMontage, FPriorityAnimMontage NextMontage);

private:

	//void InitPriorityAnimMontages();

protected:
	
	const FName HOLSTER_SOCKET_NAME = "Pan_Holster"; // ������ socket �̸�
	const FName EQUIPPED_SOCKET_NAME = "Pan_Equip"; // ���Ⱑ �տ� ������ socket �̸�
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	struct FPriorityAnimMontage AttackMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SheathMontages{};

protected:

	class UShapeComponent* AttackCollider{};

};
