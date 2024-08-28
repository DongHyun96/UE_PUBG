// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/C_Weapon.h"
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
	/// OwnerCharacter의 Pose Transition 모션이 끝났을 때 Delegate를 통해 call back을 받는 함수 (현재 캐릭터의 slot에 장착된 무기만 call back 될 예정) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

private:

	/// <summary>
	/// <para> 자세 별 AnimMontage를 Play하고 있는 도중, OwnerCharacter의 자세가 바뀔 때 </para>
	/// <para> 현재 재생 중인 AnimMontage의 재생위치 비율에 따라 다음 자세의 Montage의 시작위치를 조정하여 재생 </para>
	/// </summary>
	/// <param name="PrevMontage"> 현재 재생중인 몽타주 </param>
	/// <param name="NextMontage"> 바꿀 몽타주 </param>
	//void SwitchCurrentPlayingMontage(FPriorityAnimMontage CurrentMontage, FPriorityAnimMontage NextMontage);

private:

	//void InitPriorityAnimMontages();

protected:
	
	const FName HOLSTER_SOCKET_NAME = "Pan_Holster"; // 무기집 socket 이름
	const FName EQUIPPED_SOCKET_NAME = "Pan_Equip"; // 무기가 손에 부착될 socket 이름
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	struct FPriorityAnimMontage AttackMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SheathMontages{};

protected:

	class UShapeComponent* AttackCollider{};

};
