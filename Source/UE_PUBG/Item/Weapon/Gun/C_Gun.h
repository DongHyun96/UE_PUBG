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
	SR_SINGLE_SHOT,	// SR용 Shooting Mode
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
	float BulletSpeed{};

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
	float SprintSpeedFactor{};
};

USTRUCT(BlueprintType)
struct FAnimationMontages
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EGunState, FPriorityAnimMontage> Montages{};
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
	bool AttachToHolster(USceneComponent* InParent) override;
	bool AttachToHand(USceneComponent* InParent) override;

	void ChangeGunState(EGunState InGunState) { CurGunSlotState = InGunState; }
	bool SetAimingDown();
	bool SetAimingPress();
	bool BackToMainCamera();
	void SetIsAimPress(bool InIsAimDown) { bIsAimDown = InIsAimDown; }
	bool GetIsAimPress() const { return bIsAimDown; }
	void HandleSpringArmRotation();
	
	void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter);

	
	USkeletalMeshComponent* GetGunMesh() { return GunMesh; }
	EGunState GetCurrentWeaponState() { return CurGunSlotState; }
	TMap<EPoseState, FAnimationMontages> GetSheathMontages() { return SheathMontages; };
	TMap<EPoseState, FAnimationMontages > GetDrawMontages() { return DrawMontages; };
	FTransform GetLeftHandSocketTransform() const { return LeftHandSocketLocation; }
	EShootingMode GetCurrentShootingMode() { return CurrentShootingMode; }

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

	/// <summary>
	/// PreviewCharacter의 무기 메시를 업데이트하는 함수.
	/// </summary>
	/// <param name="InSlot"></param>
	void UpdatePreviewWeaponMesh(EWeaponSlot InSlot, FName InSocket);

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages > DrawMontages{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages> SheathMontages{};

	// 자세별 탄창 장전 Montages
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EPoseState, FAnimationMontages> ReloadMontages{};

public:
	
	/// <summary>
	/// PoseState와 GunState에 맞는(해당하는) Reload Montage 반환
	/// </summary>
	/// <returns> : PriorityAnimMontage </returns>
	FPriorityAnimMontage GetReloadMontage(EPoseState InPoseState, EGunState InGunState) const;

public:
	
	virtual bool GetIsPlayingMontagesOfAny();
	
	virtual void ChangeCurShootingMode() PURE_VIRTUAL(AC_Gun::ChangeCurShootingMode, );

	/// <summary>
	/// 탄창 재장전 실행
	/// </summary>
	/// <returns> : 제대로 실행되지 않았거나 실행될 수 없는 상황이라면 return false </returns>
	bool ExecuteMagazineReloadMontage();
	
protected: /* 무기집 Socket 이름들 */
	
	static const FName SUB_HOLSTER_SOCKET_NAME;
	static const FName MAIN_HOLSTER_SOCKET_NAME;

	static const FName SUB_HOLSTER_BAG_SOCKET_NAME;
	static const FName MAIN_HOLSTER_BAG_SOCKET_NAME;
	static const FName MAGAZINE_SOCKET_NAME;
	
protected:
	
	FName EquippedSocketName{};
	EGunState CurGunSlotState = EGunState::MAIN_GUN; // MainGun or SubGun

private:
	
	static const FName SUB_DRAW_SOCKET_NAME; // 무기가 손에 부착될 socket 이름
	bool bIsAimDown = false;
	
protected:
	//블루프린트에서 할당한 스켈레탈 메쉬를 저장하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* GunMesh;
public:
	void SetMainOrSubSlot(EGunState InState) { CurGunSlotState = InState; }

	UPROPERTY(BlueprintReadWrite)
	FTransform LeftHandSocketLocation{};

	class UCameraComponent* AimSightCamera;

	class USpringArmComponent* AimSightSpringArm{};
	int GetCurMagazineBulletCount() const { return CurMagazineBulletCount; }
	int GetMaxMagazineBulletCount() const { return MaxMagazineBulletCount; }

	void SetCurMagazineBulletCount(int InCount) { CurMagazineBulletCount = InCount; }

	UFUNCTION(BlueprintCallable)
	EBulletType GetCurBulletType() const { return GunDataRef->CurGunBulletType; }

	AC_AttachableItem* GetMagazineGameObject() const { return Magazine; }
	
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

	
	const FGunSoundData* GunSoundData = nullptr;

public:

	FName GetEQUIPPED_SOCKET_NAME() const { return EquippedSocketName; }
	
protected: // 총알 발사 관련 변수들

	// 현재 탄창에 남은 탄알 수
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CurMagazineBulletCount{};

	// 탄창 최대 장탄 수
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MaxMagazineBulletCount{};

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

private:
	
	/// <summary>
	/// Reload Montage 모션 끝난 이후, 실질적으로 탄창의 탄알 채우기 처리 함수
	/// </summary>
	/// <returns> : 제대로 Magazine이 장전되지 않았거나 장전될 수 없는 상황이라면 return false </returns>
	bool ReloadMagazine();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//class AC_Bullet* Bullet;

	//void SpawnBulletForTest();
	void SetBulletSpeed();
	bool SetBulletVelocity(FVector& OutLocation, FVector& OutVelocity, FVector& OutHitLocation, bool& OutHasHit);


	EBackPackLevel PrevBackPackLevel{};

	void SetAimSightWidget();

public:
	
	float GetBaseBulletSpreadDegree() { return GunDataRef->BaseBulletSpreadDegree; }
	
protected:
	//Aim 위젯
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* AimWidget;
	FName WidgetFilePath;
	void HandleAimWidgetShowAndHideWhileAiming();
protected:
	//탄창
	AC_AttachableItem* Magazine{};

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
	TMap<EPartsName, class AC_AttachableItem*> GetAttachableItem() const { return AttachableItem; }

	TMap<EPartsName, AC_AttachableItem*>& GetAttachableItemReference() { return AttachableItem; }

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
	void SetSightCameraSpringArmLocation(const FVector4& InLocationAndArmLength);
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
	
	/// <summary>
	/// AI 총알 발사 관련 함수 (final method) 
	/// </summary>
	bool ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter) override;

	bool ExecuteAIAttackTickTask(AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;

protected:
	
	bool CanAIAttack(AC_BasicCharacter* InTargetCharacter);

	/// <summary>
	/// Enemy AI 전용 총알 발사 처리 함수
	/// </summary>
	/// <param name="InTargetCharacter"> : Target Character </param>
	/// <returns> : 제대로 총알이 발사되었다면 return true </returns>
	virtual bool AIFireBullet(AC_BasicCharacter* InTargetCharacter) PURE_VIRTUAL(AC_Gun::AIFireBullet, return false;);
	
	float AIFireTimer = 0.0f;

public: 
	/// <summary>
	/// 각 Bone 피격 부위 별 DamageRate Getter (AR과 SR의 DamageRate가 서로 다름)
	/// </summary>
	/// <param name="BodyPart"> : BoneName 주기 </param>
	/// <returns></returns>
	virtual float GetDamageRateByBodyPart(const FName& BodyPart) PURE_VIRTUAL(AC_Gun::GetDamageRateByBodyPart, return 0.f;);

	float GetDamageBase() const { return GunDataRef->DamageBase; }
	
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
	
	/// <summary>
	///  AN_ReloadEnd Callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnReloadEnd();	

public:

	// Weapon Tutorial 진행용 Delegate
	static FTutorialStageGoalCheckerDelegate WeaponTutorialDelegate;

protected:

	// AI Enemy가 총기를 발사할 때, 발사 사이의 텀 간격 시간
	float AIAttackIntervalTime{};
	
};


