// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Weapon/C_Weapon.h"
#include "Containers/Map.h"
#include "Containers/Array.h"
#include "Templates/Tuple.h"
#include "C_Gun.generated.h"

UENUM(BlueprintType)
enum class EGunState : uint8
{
	MAIN_GUN,
	SUB_GUN
};

USTRUCT(BlueprintType)
struct FAnimationMontages
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EGunState, FPriorityAnimMontage> Montages;
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
	bool SetAimingDown();
	bool SetAimingPress();
	bool BackToMainCamera();
	void SetIsAimPress(bool InIsAimPress) { bIsAimPress = InIsAimPress; }
	bool GetIsAimPress() { return bIsAimPress; }
	FTransform GetLeftHandSocketTransform() const { return LeftHandSocketLocation; }
	class UCameraComponent* GetGunCamera() { return AimSightCamera; }

protected:
	/// <summary>
	/// OwnerCharacter�� Pose Transition ����� ������ �� Delegate�� ���� call back�� �޴� �Լ� (���� ĳ������ slot�� ������ ���⸸ call back �� ����) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages > DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)	
	TMap<EPoseState, FAnimationMontages> SheathMontages{};

	
private:
	const FName SUB_HOLSTER_SOCKET_NAME = "SubGunSocket_NoBag"; // ������ socket �̸�
	const FName MAIN_HOLSTER_SOCKET_NAME = "MainGunSocket_NoBag"; // ������ socket �̸�
	const FName EQUIPPED_SOCKET_NAME = "Rifle_Equip"; // ���Ⱑ �տ� ������ socket �̸�
	//const FName EQUIPPED_SOCKET_NAME = "Rifle_Equip"; // ���Ⱑ �տ� ������ socket �̸�
	const FName SUB_DRAW_SOCKET_NAME = "DrawRifleSocket"; // ���Ⱑ �տ� ������ socket �̸�
	EGunState CurState = EGunState::SUB_GUN;
	bool bIsAimPress = false;

private:
	//�������Ʈ���� �Ҵ��� ���̷�Ż �޽��� �����ϴ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* GunMesh;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform LeftHandSocketLocation;

	class UCameraComponent* AimSightCamera{};

};
