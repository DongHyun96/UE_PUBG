// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Weapon/C_Weapon.h"
#include "Containers/Map.h"
#include "Containers/Array.h"
#include "Templates/Tuple.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/ItemBullet/C_Item_Bullet.h"
#include "C_Gun.generated.h"

UENUM(BlueprintType)
enum class EGunState : uint8
{
	MAIN_GUN,
	SUB_GUN
};

UENUM(BlueprintType)
enum class EGunType : uint8
{
	AR,
	SR,
	MAX
};

UENUM(BlueprintType)
enum class EShootingMode : uint8
{
	SEMI_AUTO,
	FULL_AUTO,
	BURST,
	SINGLE_SHOT
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

	bool LegacyMoveToAround(AC_BasicCharacter* Character) override;

	bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;

	//AC_Item* SpawnItem(AC_BasicCharacter* Character) override;

	void PickUpItem(AC_BasicCharacter* Character) override;

	void CheckBackPackLevelChange();

	void DropItem(AC_BasicCharacter* Character) override;
protected:
	/// <summary>
	/// OwnerCharacter�� Pose Transition ����� ������ �� Delegate�� ���� call back�� �޴� �Լ� (���� ĳ������ slot�� ������ ���⸸ call back �� ����) 
	/// </summary>
	void OnOwnerCharacterPoseTransitionFin() override;

	void CheckPlayerIsRunning();



public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages > DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages> SheathMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages> ReloadMontages{};

	virtual bool GetIsPlayingMontagesOfAny();
	bool GetCanGunAction();
	void ChangeCurShootingMode();
	virtual bool ExecuteReloadMontage();
	bool bIsSniperReload = false;
	void SetIsSniperReload(bool InIsSniperReload) { bIsSniperReload = InIsSniperReload; }
	class UCanvasPanelSlot* AimImage;
	float MilitaryOperationArea;
	FVector2D PanelSize;
protected:
	const FName SUB_HOLSTER_SOCKET_NAME = "SubGunSocket_NoBag"; // ������ socket �̸�
	const FName MAIN_HOLSTER_SOCKET_NAME = "MainGunSocket_NoBag"; // ������ socket �̸�

	const FName SUB_HOLSTER_BAG_SOCKET_NAME = "SubGunSocket_Bag"; // ������ socket �̸�
	const FName MAIN_HOLSTER_BAG_SOCKET_NAME = "MainGunSocket_Bag"; // ������ socket �̸�
	const FName MAGAZINE_SOCKET_NAME = "Magazine_Socket";


	FName EQUIPPED_SOCKET_NAME; 
	//const FName EQUIPPED_SOCKET_NAME = "Rifle_Equip"; // ���Ⱑ �տ� ������ socket �̸�
	const FName SUB_DRAW_SOCKET_NAME = "DrawRifleSocket"; // ���Ⱑ �տ� ������ socket �̸�
	EGunState CurState = EGunState::MAIN_GUN;
	bool bIsAimDown = false;
protected:
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
	int GetCurBulletCount() { return CurBulletCount; }
	int GetMaxBulletCount() { return MaxBulletCount; }
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EShootingMode CurrentShootingMode = EShootingMode::FULL_AUTO;

	float RecoilMultiplierByGripVert = 1;
	float RecoilMultiplierByGripHorizon = 1;
	float RecoilMultiplierMuzzleVert = 1;
	float RecoilMultiplierMuzzleHorizon = 1;
private:
	//bool MoveSlotToAround(AC_BasicCharacter* Character) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	bool MoveAroundToSlot(AC_BasicCharacter* Character) override;

public:
	void SetRecoilMultiplierGripVert(float InValue)      { RecoilMultiplierByGripVert    = InValue; }
	void SetRecoilMultiplierGripHorizon(float InValue)   { RecoilMultiplierByGripHorizon = InValue; }
	void SetRecoilMultiplierMuzzleVert(float InValue)    { RecoilMultiplierMuzzleVert    = InValue; }
	void SetRecoilMultiplierMuzzleHorizon(float InValue) { RecoilMultiplierMuzzleHorizon = InValue; }
	FVector2D GetRecoilFactors();
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBulletType CurGunBulletType;

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
	void BackTo_RightHand();
protected:
	EGunType CurGunType = EGunType::MAX;
public:
	EGunType GetGunType() { return CurGunType; }
	UTexture2D* GetDragIcon() { return DragIcon; }
protected:
	FVector2D IronSightWindowLocation{};
	
protected:
	/// <summary>
	/// MainGun�� Slot�� �����Ǵ� Icon�� Drag Ȥ�� �ٴڿ� �������� ���� Icon�� �ٸ�.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DragIcon")
	UTexture2D* DragIcon = nullptr; 

};
