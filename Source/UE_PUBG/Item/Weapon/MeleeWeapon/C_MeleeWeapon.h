// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/C_Weapon.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MeleeWeapon.generated.h"

USTRUCT(BlueprintType)
struct FMeleeWeaponSoundDatas : public FTableRowBase
{
	GENERATED_BODY()

	/// <summary>
	/// Pan 피격음, 에디터에서 설정.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* ImpactSound = nullptr;

};

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
	void InitializeItem(FName NewItemCode) override;

	bool AttachToHolster(class USceneComponent* InParent) override;

	bool AttachToHand(class USceneComponent* InParent) override;

	void PickUpItem(class AC_BasicCharacter* Character) override;

	bool Interaction(AC_BasicCharacter* Character) override;

public:

	FPriorityAnimMontage GetAttackMontage() const { return AttackMontage; }

	const FMeleeWeaponSoundDatas* GetPanSoundData() const { return MeleeWeaponSoundData; }

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
	
	const FName GetHolsterSocketName() const { return HOLSTER_SOCKET_NAME; }
	const FName GetEquippedSocketName() const { return EQUIPPED_SOCKET_NAME; }
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

	const FMeleeWeaponSoundDatas* MeleeWeaponSoundData = nullptr;
private:

	// 한 번 휘둘렀을 때 이미 Damage를 주었던 Character들
	TSet<class AC_BasicCharacter*> AttackedCharacters{};
};


