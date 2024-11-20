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


UENUM(BlueprintType)
enum class EShootingMode : uint8
{
	SEMI_AUTO,
	FULL_AUTO,
	BURST		
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
	virtual bool AttachToHolster(class USceneComponent* InParent) override;
	virtual bool AttachToHand(class USceneComponent* InParent) override;

	virtual void ChangeGunState(EGunState InGunState) { CurState = InGunState; }
	virtual bool SetAimingDown();
	virtual bool SetAimingPress();
	virtual bool BackToMainCamera();
	virtual void SetIsAimPress(bool InIsAimDown) { bIsAimDown = InIsAimDown; }
	virtual bool GetIsAimPress() { return bIsAimDown; }
	virtual void HandleSpringArmRotation();
	virtual void GetPlayerIsAimDownOrNot();
	virtual void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter);

	USkeletalMeshComponent* GetGunMesh() { return GunMesh; }
	EGunState GetCurrentWeaponState() { return CurState; }
	TMap<EPoseState, FAnimationMontages> GetSheathMontages() { return SheathMontages; };
	TMap<EPoseState, FAnimationMontages > GetDrawMontages() { return DrawMontages; };
	FTransform GetLeftHandSocketTransform() const { return LeftHandSocketLocation; }
	EShootingMode GetCurrentShootingMode() { return CurrentShootingMode; }
	class UCameraComponent* GetGunCamera() { return AimSightCamera; }

	void PickUpItem(AC_BasicCharacter* Character) override;

protected:
	/// <summary>
	/// OwnerCharacter�� Pose Transition ����� ������ �� Delegate�� ���� call back�� �޴� �Լ� (���� ĳ������ slot�� ������ ���⸸ call back �� ����) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

	void CheckPlayerIsRunning();

	void CheckBackPackLevelChange();


public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages > DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages> SheathMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages> ReloadMontages{};

	bool GetIsPlayingMontagesOfAny();
	bool GetCanGunAction();
	void ChangeCurShootingMode();
	void ExecuteReloadMontage();
	class UCanvasPanelSlot* AimImage;
	float MilitaryOperationArea;
	FVector2D PanelSize;
private:
	const FName SUB_HOLSTER_SOCKET_NAME = "SubGunSocket_NoBag"; // ������ socket �̸�
	const FName MAIN_HOLSTER_SOCKET_NAME = "MainGunSocket_NoBag"; // ������ socket �̸�

	const FName SUB_HOLSTER_BAG_SOCKET_NAME = "SubGunSocket_Bag"; // ������ socket �̸�
	const FName MAIN_HOLSTER_BAG_SOCKET_NAME = "MainGunSocket_Bag"; // ������ socket �̸�
	const FName MAGAZINE_SOCKET_NAME = "Magazine_Socket";


	const FName EQUIPPED_SOCKET_NAME = "Rifle_Equip"; // ���Ⱑ �տ� ������ socket �̸�
	//const FName EQUIPPED_SOCKET_NAME = "Rifle_Equip"; // ���Ⱑ �տ� ������ socket �̸�
	const FName SUB_DRAW_SOCKET_NAME = "DrawRifleSocket"; // ���Ⱑ �տ� ������ socket �̸�
	EGunState CurState = EGunState::MAIN_GUN;
	bool bIsAimDown = false;
private:
	//�������Ʈ���� �Ҵ��� ���̷�Ż �޽��� �����ϴ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* GunMesh;
public:
	void SetMainOrSubSlot(EGunState InState) { CurState = InState; }
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform LeftHandSocketLocation;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bIsPlayerAimDownPress = false;

	UCameraComponent* AimSightCamera;
	class USpringArmComponent* AimSightSpringArm{};

protected:
	/// <summary>
	/// �Ѿ� �߻� ���� ������
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CurBulletCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MaxBulletCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BulletSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BulletRPM;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ReloadTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseBulletSpreadDegree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RecoilFactorVertical;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	float RecoilFactorHorizontal;

	EShootingMode CurrentShootingMode = EShootingMode::FULL_AUTO;
public:
	FVector2D GetRecoilFactors() { return FVector2D(RecoilFactorHorizontal, RecoilFactorVertical); }
	float GetBulletRPM() { return BulletRPM; }
	virtual bool FireBullet();

	virtual bool ReloadBullet();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//class AC_Bullet* Bullet;

	//void SpawnBulletForTest();
	virtual void SetBulletSpeed();
	virtual bool SetBulletDirection(FVector& OutLocation, FVector& OutDirection, FVector& OutHitLocation, bool& OutHasHit);


	EBackPackLevel PrevBackPackLevel;

	void SetAimSightWidget();

	float GetBaseBulletSpreadDegree() { return BaseBulletSpreadDegree; }
protected:
	//Aim ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* AimWidget;
	FName WidgetFilePath;
	void ShowAndHideWhileAiming();
protected:
	//źâ
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	class AC_AttachableItem* Magazine{};

	void LoadMagazine();

public:
	void SetMagazineVisibility(bool InIsVisible);
	TArray<EPartsName> GetAttachableParts() { return AttachableParts; }
	bool GetGunHasGrip();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<EPartsName> AttachableParts{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EPartsName, class UMeshComponent*> AttachedParts{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EAttachmentNames, FName> AttachmentPartsHolsterNames{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EPartsName, bool> IsPartAttached{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EPartsName, EAttachmentNames> AttachedItemName{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EAttachmentNames, FVector4> ScopeCameraLocations{};

	TMap<EPartsName, class AAttachmentActor*> AttachedItem{};

public:
	TMap<EAttachmentNames, FName> GetAttachmentPartsHolsterNames() { return AttachmentPartsHolsterNames; }
	TMap<EAttachmentNames, FVector4> GetScopeCameraLocations() { return ScopeCameraLocations; }
	void SetAttachedItems(EPartsName InPartName, AAttachmentActor* InAttachedItem) { AttachedItem[InPartName] = InAttachedItem; }
protected:
	void SetHolsterNames();
	UMeshComponent* IronSightMesh{};
public:
	void SetIronSightMeshHiddenInGame(bool bInIsHidden);
	bool GetIsPartAttached(EPartsName InAttachmentName) { return IsPartAttached[InAttachmentName]; }
	void SetIsPartAttached(EPartsName InAttachmentName, bool bInIsAttached);
	EAttachmentNames GetAttachedItemName(EPartsName InPartName) { return AttachedItemName[InPartName]; }
	void  SetAttachedItemNameInPart(EPartsName InPartName, EAttachmentNames InAttachmentName) { AttachedItemName[InPartName] = InAttachmentName; }
	void SetSightCameraSpringArmLocation(FVector4 InLocationAndArmLength);
	//For Test

	

	void SetScopeCameraMode(EAttachmentNames InAttachmentName);
};
