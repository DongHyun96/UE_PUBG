// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_PreviewCharacter.generated.h"

enum class EWeaponSlot : uint8;
enum class EEquipSlot : uint8;
enum class EPoseState : uint8;

UCLASS()
class UE_PUBG_API AC_PreviewCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_PreviewCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetOwnerPlayer(class AC_Player* Player) { OwnerPlayer = Player; }

	void SetWeaponMesh(EWeaponSlot InSlot, class AC_Weapon* Weapon);

	/// <summary>
	/// MeleeWeaponMesh를 장착합니다.
	/// </summary>
	/// <returns></returns>
	bool AttachMeleeWeaponMesh();

	bool AttachGunMesh(EWeaponSlot InSlot, FName InSocket);

	/// <summary>
	/// InSlot에 해당하는 장착 무기 메시를 때어 냅니다.
	/// </summary>
	/// <param name="InSlot"></param>
	/// <returns></returns>
	bool DetachWeaponMesh(EWeaponSlot InSlot);

	//bool DetachCurWeapon();

	/// <summary>
	/// InSlot에 해당하는 장착 아이템 메시를 때어 냅니다..
	/// </summary>
	/// <param name="InSlot"></param>
	/// <returns></returns>
	bool DetachEquippedMesh(EEquipSlot InSlot);

	/// <summary>
	/// InSlot에 해당하는 장착 무기 메시를 업데이트합니다.
	/// </summary>
	/// <param name="InSlot"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool UpdateWeaponMesh(EWeaponSlot InSlot);

	/// <summary>
	/// InSlot에 해당하는 장착 아이템 메시를 업데이트합니다.
	/// </summary>
	/// <param name="InSlot"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	bool UpdateEquippedMesh(EEquipSlot InSlot);	

	/// <summary>
	/// 블루프린트에서 정의.
	/// HandState에 따른 Pose Update 함수.
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHandPose(EHandState InState);

	bool Update(EHandState InState,EWeaponSlot InSlot, FName InSocket);

	/// <summary>
	/// PreviewCharacter의 Gun Mesh들을 스왑하는 함수.
	/// </summary>
	/// <returns></returns>
	bool SwapSlotsWhileGunHandState();

	//UFUNCTION(BlueprintImplementableEvent)
	//void UpdateHandPose();

public:
	UFUNCTION(BlueprintCallable)
	AC_Player* GetOwnerPlayer() const { return OwnerPlayer; }

	TMap<EWeaponSlot, USceneComponent*> GetWeaponMeshes() const { return WeaponMeshes; }

	TMap<EEquipSlot, UStaticMeshComponent*> GetEquipMeshes() const { return EquipMeshes; }
protected:

	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* PreviewMainWeaponMesh{};

	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* PreviewSubWeaponMesh{};

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* PreviewHelmetMesh{};

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* PreviewArmorMesh{};

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* PreviewBackpackMesh{};

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* PreviewMeleeWeaponMesh{};

	/// <summary>
	/// WeaponMeshes는 EWeaponSlot에 따라 무기 메시를 저장하는 TMap입니다.
	/// </summary>
	TMap<EWeaponSlot, USceneComponent*> WeaponMeshes{};

	/// <summary>
	/// EquipMeshes는 EEquipSlot에 따라 장착 아이템 메시를 저장하는 TMap입니다.
	/// </summary>
	TMap<EEquipSlot, UStaticMeshComponent*> EquipMeshes{};

	class AC_Player* OwnerPlayer{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USceneCaptureComponent2D* SceneCapture{};
protected:
	// C_Gun
	const FName SUB_HOLSTER_SOCKET_NAME = "SubGunSocket_NoBag"; // 무기집 socket 이름
	const FName MAIN_HOLSTER_SOCKET_NAME = "MainGunSocket_NoBag"; // 무기집 socket 이름

	const FName SUB_HOLSTER_BAG_SOCKET_NAME = "SubGunSocket_Bag"; // 무기집 socket 이름
	const FName MAIN_HOLSTER_BAG_SOCKET_NAME = "MainGunSocket_Bag"; // 무기집 socket 이름

	USkeletalMeshComponent* previewCharacterMesh{};


};
