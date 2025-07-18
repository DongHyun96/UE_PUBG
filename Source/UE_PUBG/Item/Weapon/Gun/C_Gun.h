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
#include "TrainingLevel/Tutorial/TutorialStageChecker/C_TutorialStageChecker.h"
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
	SEMI_AUTO,		// 단발
	FULL_AUTO,		// 연사
	BURST,			// 점사
	SINGLE_SHOT,	// SR용 Shooting Mode
	MAX
};

USTRUCT(BlueprintType)
struct FGunSoundData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* FireSound = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* ReloadMagazineSound = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* LoadBulletSound = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* NullBulletSound = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* AimDownSightSound{};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* ChangeShootingModeSound{};
};


USTRUCT(BlueprintType)
struct FGunData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BulletSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BulletRPM{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ReloadTime{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseBulletSpreadDegree{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RecoilFactorVertical{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RecoilFactorHorizontal{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EGunType CurGunType = EGunType::MAX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageBase{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EBulletType CurGunBulletType = EBulletType::NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SprintSpeedFactor = 0.0f;
};

USTRUCT(BlueprintType)
struct FAnimationMontages
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EGunState, FPriorityAnimMontage> Montages;
};
UCLASS(Abstract)
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

	void InitializeItem(FName NewItemCode) override;

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

	bool Interaction(AC_BasicCharacter* Character) override;

	void CheckBackPackLevelChange();

	void DropItem(AC_BasicCharacter* Character) override;

	/// <summary>
	/// 총기 부착물을 부착하는 함수.
	/// Mesh에 대한 처리도 포함하고 있다.
	/// </summary>
	/// <param name="InPartsName">부착하려는 부위</param>
	/// <param name="InAttachableItem">부착하려는 부착물</param>
	/// <returns>교체되어 나온 부착물</returns>
	class AC_AttachableItem* SetAttachableItemSlot(EPartsName InPartsName, AC_AttachableItem* InAttachableItem);
protected:
	//bool MoveSlotToAround(AC_BasicCharacter* Character) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack) override;
	bool MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack) override;

protected:
	/// <summary>
	/// OwnerCharacter의 Pose Transition 모션이 끝났을 때 Delegate를 통해 call back을 받는 함수 (현재 캐릭터의 slot에 장착된 무기만 call back 될 예정) 
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
	virtual void ChangeCurShootingMode() PURE_VIRTUAL(AC_Gun::ChangeCurShootingMode, );
	virtual bool ExecuteReloadMontage();
	bool bIsSniperReload = false;
	void SetIsSniperReload(bool InIsSniperReload) { bIsSniperReload = InIsSniperReload; }
	class UCanvasPanelSlot* AimImage;
	float MilitaryOperationArea;
	FVector2D PanelSize;
protected:
	const FName SUB_HOLSTER_SOCKET_NAME = "SubGunSocket_NoBag"; // 무기집 socket 이름
	const FName MAIN_HOLSTER_SOCKET_NAME = "MainGunSocket_NoBag"; // 무기집 socket 이름

	const FName SUB_HOLSTER_BAG_SOCKET_NAME = "SubGunSocket_Bag"; // 무기집 socket 이름
	const FName MAIN_HOLSTER_BAG_SOCKET_NAME = "MainGunSocket_Bag"; // 무기집 socket 이름
	const FName MAGAZINE_SOCKET_NAME = "Magazine_Socket";
	
	
	FName EQUIPPED_SOCKET_NAME; 
	//const FName EQUIPPED_SOCKET_NAME = "Rifle_Equip"; // 무기가 손에 부착될 socket 이름
	const FName SUB_DRAW_SOCKET_NAME = "DrawRifleSocket"; // 무기가 손에 부착될 socket 이름
	EGunState CurState = EGunState::MAIN_GUN;
	bool bIsAimDown = false;
protected:
	//블루프린트에서 할당한 스켈레탈 메쉬를 저장하는 변수
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

	void SetCurBulletCount(int InCount) { CurBulletCount = InCount; }

	UFUNCTION(BlueprintCallable)
	EBulletType GetCurBulletType() const { return GunDataRef->CurGunBulletType; }

	/// <summary>
	/// const GunData*를 반환해주는 함수.
	/// </summary>
	/// <returns>const FGunData*</returns>
	const FGunData* GetGunData() const { return GunDataRef; }

	/// <summary>
	/// const GunSoundData*를 반환해 주는 함수.
	/// </summary>
	/// <returns></returns>
	const FGunSoundData* GetGunSoundData() const { return GunSoundData; }



protected:
	//GunData 구조체를 const 포인터 변수로 가지고 있음.
	const FGunData* GunDataRef = nullptr;

	//Gun Sound Datas, TODO : 
	const FGunSoundData* GunSoundData = nullptr;


	const FName GetSUB_HOLSTER_SOCKET_NAME()  { return SUB_HOLSTER_SOCKET_NAME; }
	const FName GetMAIN_HOLSTER_SOCKET_NAME()  { return MAIN_HOLSTER_SOCKET_NAME; }
	const FName GetSUB_HOLSTER_BAG_SOCKET_NAME()  { return SUB_HOLSTER_BAG_SOCKET_NAME; }
	const FName GetMAIN_HOLSTER_BAG_SOCKET_NAME()  { return MAIN_HOLSTER_BAG_SOCKET_NAME; }

	FName GetEQUIPPED_SOCKET_NAME() const { return EQUIPPED_SOCKET_NAME; }
protected:
	/// <summary>
	/// 총알 발사 관련 변수들
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CurBulletCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MaxBulletCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EShootingMode CurrentShootingMode = EShootingMode::FULL_AUTO;

	//GunData 구조체에 안넣음
	float RecoilMultiplierByGripVert = 1;
	float RecoilMultiplierByGripHorizon = 1;
	float RecoilMultiplierMuzzleVert = 1;
	float RecoilMultiplierMuzzleHorizon = 1;

public:
	void SetRecoilMultiplierGripVert(float InValue)      { RecoilMultiplierByGripVert    = InValue; }
	void SetRecoilMultiplierGripHorizon(float InValue)   { RecoilMultiplierByGripHorizon = InValue; }
	void SetRecoilMultiplierMuzzleVert(float InValue)    { RecoilMultiplierMuzzleVert    = InValue; }
	void SetRecoilMultiplierMuzzleHorizon(float InValue) { RecoilMultiplierMuzzleHorizon = InValue; }
	FVector2D GetRecoilFactors();
	float GetBulletRPM() { return GunDataRef->BulletRPM; }
	virtual bool FireBullet();

	virtual bool ReloadBullet();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//class AC_Bullet* Bullet;

	//void SpawnBulletForTest();
	virtual void SetBulletSpeed();
	virtual bool SetBulletDirection(FVector& OutLocation, FVector& OutDirection, FVector& OutHitLocation, bool& OutHasHit);


	EBackPackLevel PrevBackPackLevel;

	void SetAimSightWidget();

	float GetBaseBulletSpreadDegree() { return GunDataRef->BaseBulletSpreadDegree; }
protected:
	//Aim 위젯
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* AimWidget;
	FName WidgetFilePath;
	void ShowAndHideWhileAiming();
protected:
	//탄창
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	class AC_AttachableItem* Magazine{};

	void LoadMagazine();
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//EBulletType CurGunBulletType;
public:
	void SetMagazineVisibility(bool InIsVisible);
	TArray<EPartsName> GetAttachableParts() { return AttachableParts; }
	bool GetGunHasGrip();
protected:
	//어떤 파츠를 장착 할 수있는가
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<EPartsName> AttachableParts{};

	//안쓰는듯?
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EPartsName, class UMeshComponent*> AttachedParts{};

	//홀스터 이름
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EAttachmentNames, FName> AttachmentPartsHolsterNames{};

	//해당 부위 파츠가 장착중인지 -> 아래 AttachedItem으로 통합 예정
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EPartsName, bool> IsPartAttached{};

	//해당 부위에 어떤 파츠가 붙어있는가 -> 아래 AttachedItem으로 통합 예정
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EPartsName, EAttachmentNames> AttachedItemName{};

	//스코프 카메라 위치 정보
	UPROPERTY(BlueprintReadWrite, EditAnywhere)

	TMap<EAttachmentNames, FVector4> ScopeCameraLocations{};

	//해당 부위 장착된 파츠(AAttachmentActor) mesh 객체 
	TMap<EPartsName, class AAttachmentActor*> AttachedItem{};

	//해당 부위에 장착될 수 있는 파츠(C_AttachableItem)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EPartsName, AC_AttachableItem*> AttachableItem{};

public:

	/// <summary>
	/// 총에 직접 부착되는 부착물의 Mesh TMap을 반환한다.
	/// </summary>
	/// <returns></returns>
	TMap<EPartsName, AAttachmentActor*> GetAttachedItem() { return AttachedItem; }

	/// <summary>
	/// 인벤에서 사용하는 부착물 TMap을 반환한다.(Mesh가 아님)
	/// </summary>
	/// <returns></returns>
	TMap<EPartsName, class AC_AttachableItem*> GetAttachableItem() { return AttachableItem; }

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
	//WeaponData구조체에 넣어도 됨
	//EGunType CurGunType = EGunType::MAX;
public:
	EGunType GetGunType() { return GunDataRef->CurGunType; }

	//EGunState GetGunState() { return CurState; }
protected:
	FVector2D IronSightWindowLocation{};


public:
	//AI 총알 발사 관련 함수
	virtual bool ExecuteAIAttack(class AC_BasicCharacter* InTargetCharacter) override;
	virtual bool ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;

public: 
	/// <summary>
	/// 각 Bone 피격 부위 별 DamageRate Getter (AR과 SR의 DamageRate가 서로 다름)
	/// </summary>
	/// <param name="BodyPart"> : BoneName 주기 </param>
	/// <returns></returns>
	virtual float GetDamageRateByBodyPart(const FName& BodyPart) PURE_VIRTUAL(AC_Gun::GetDamageRateByBodyPart, return 0.f;);

	float GetDamageBase() const { return GunDataRef->DamageBase; }
protected:
	bool CanAIAttack(AC_BasicCharacter* InTargetCharacter);
	virtual bool AIFireBullet(class AC_BasicCharacter* InTargetCharacter);
	float AIFireTimer = 0.0f;
protected:
	// UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	// UCapsuleComponent* CapsuleComponent{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UParticleSystem* MuzzleFlameEffectParticle{};
public:
	UFUNCTION(BlueprintCallable)
	FName GetCurrentBulletTypeName();
public:

	virtual void SetActorHiddenInGame(bool bNewHidden) override;
	virtual void CancelReload();

public:

	// Weapon Tutorial 진행용 Delegate
	static FTutorialStageGoalCheckerDelegate WeaponTutorialDelegate;
	
};


