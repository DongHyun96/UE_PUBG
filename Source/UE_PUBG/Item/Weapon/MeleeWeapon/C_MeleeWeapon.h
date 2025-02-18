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
	bool LegacyMoveToInven(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 해당 아이템(객체)를 얼마나 버릴 것인지 계산해서 버린다.(이것은 후에 나눠버리기 기능을 위해 uint8정도를 받아 사용하면 좋을듯.)
	/// ->일부만 버리는 경우 아이템(객체)를 새로 생성해서 원본과 수량을 조정해야 한다. (원본stack = 수정된 원본의 stack + 생성객체의 stack)
	/// </summary>
	/// <param name="Character"></param>
	/// <returns></returns>
	bool LegacyMoveToAround(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 아이템을 슬롯으로 이동.
	/// </summary>
	/// <param name="Character"></param>
	/// <returns>true면 이동 성공, false면 실패</returns>
	bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;

protected:
	//bool MoveAroundToSlot(AC_BasicCharacter* Character) override;
	//bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	//
	//bool MoveInvenToAround(AC_BasicCharacter* Character) override;
	//bool MoveInvenToSlot(AC_BasicCharacter* Character) override;
	//
	//bool MoveSlotToAround(AC_BasicCharacter* Character) override;
	//bool MoveSlotToInven(AC_BasicCharacter* Character) override;

public:

	struct FPriorityAnimMontage GetAttackMontage() const { return AttackMontage; }

	/// <summary>
	/// Enable Attack Collider collision
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnAttackBegin();

	/// <summary>
	/// Disable Attack collider collision
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnAttackEnd();
	

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
	
public:
	bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) override;

protected:
	
	static const FName HOLSTER_SOCKET_NAME; // 무기집 socket 이름
	static const FName EQUIPPED_SOCKET_NAME; // 무기가 손에 부착될 socket 이름
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	struct FPriorityAnimMontage AttackMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FPriorityAnimMontage> SheathMontages{};


protected:
	
	class UShapeComponent* AttackCollider{};

private:

	// 한 번 휘둘렀을 때 이미 Damage를 주었던 Character들
	TSet<class AC_BasicCharacter*> AttackedCharacters{};
};


