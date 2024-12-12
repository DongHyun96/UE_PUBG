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
	/// 해당 아이템(객체)를 얼마나 인벤에 넣을 수 있는지 계산해서 넣는다. -> 일부만 넣는 경우에는 인벤에 넣을 때 Spawn을 통해 새로운 객체를 만들어 
	/// 갯수를 설정해주고 원본의 갯수도 수정. (원본stack = 수정된 원본의 stack + 생성객체의 stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool MoveToInven(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 해당 아이템(객체)를 얼마나 버릴 것인지 계산해서 버린다.(이것은 후에 나눠버리기 기능을 위해 uint8정도를 받아 사용하면 좋을듯.)
	/// ->일부만 버리는 경우 아이템(객체)를 새로 생성해서 원본과 수량을 조정해야 한다. (원본stack = 수정된 원본의 stack + 생성객체의 stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool MoveToAround(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 아이템을 슬롯으로 이동.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns>true면 이동 성공, false면 실패</returns>
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
