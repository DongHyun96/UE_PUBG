// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_DefaultItemSpawnerComponent.generated.h"


enum class EWeaponSlot : uint8;
enum class EThrowableType : uint8;
enum class EEquipableItemLevel : uint8;
enum class EConsumableItemType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_DefaultItemSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_DefaultItemSpawnerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// 기본 무기 및 기본 아이템 비행기 탑승 시 가지고 시작
	/// </summary>
	void SpawnDefaultWeaponsAndItems();

private:

	void SpawnWeapons(const FActorSpawnParameters& Param);
	void SpawnEquipableItems(const FActorSpawnParameters& Param);
	void SpawnConsumableItems(const FActorSpawnParameters& Param);
	void SpawnBullets(const FActorSpawnParameters& Param);
	
public:

	void SetOwnerEnemy(class AC_Enemy* InOwnerEnemy) { OwnerEnemy = InOwnerEnemy; }

private:

	class AC_Enemy* OwnerEnemy{};

protected: // Weapon 관련 Classes
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EWeaponSlot, TSubclassOf<class AC_Weapon>> WeaponClasses{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EThrowableType, TSubclassOf<class AC_ThrowingWeapon>> ThrowableClasses{};

protected: // Equipable 관련 Classes

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class AC_Vest> VestClass{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EEquipableItemLevel, TSubclassOf<class AC_Helmet>> HelmetClasses{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EEquipableItemLevel, TSubclassOf<class AC_BackPack>> BackPackClasses{};

protected: // TODO : 탄, 총기 부착물, 회복 아이템 등등 스폰

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EConsumableItemType, TSubclassOf<class AC_ConsumableItem>> ConsumableItemClasses{};
};
