// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_PreviewCharacter.generated.h"

enum class EWeaponSlot : uint8;
enum class EEquipSlot : uint8;


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

	bool AttachMeleeWeaponMesh();

	bool DetachWeaponMesh(EWeaponSlot InSlot);

	bool DetachEquippedMesh(EEquipSlot InSlot);

	bool UpdateWeaponMesh(EWeaponSlot InSlot);

	bool UpdateEquippedMesh(EEquipSlot InSlot);	

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

	TMap<EWeaponSlot, USceneComponent*> WeaponMeshes{};

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
