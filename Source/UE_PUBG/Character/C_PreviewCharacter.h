// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_PreviewCharacter.generated.h"

enum class EWeaponSlot : uint8;

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

	/// <summary>
	/// Gun, MeleeWeapon의 메시를 PreviewCharacter에 부착합니다.
	/// </summary>
	/// <param name="Weapon"></param>
	/// <returns></returns>
	bool AttachWeaponMesh(class AC_Weapon* Weapon, EWeaponSlot InSlot);

	void AttachHelmetMesh(class AC_EquipableItem* Weapon);

	void AttachArmorMesh(class AC_EquipableItem* Weapon);

	void AttachBackMesh(class AC_EquipableItem* Weapon);

	bool AttachMeleeWeaponMesh(class AC_Weapon* Weapon);

	bool DetachWeaponMesh(EWeaponSlot InSlot);

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
