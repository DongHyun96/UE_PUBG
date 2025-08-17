// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_DefaultItemSpawnerComponent.generated.h"


enum class EBulletType : uint8;
enum class EWeaponSlot : uint8;
enum class EThrowableType : uint8;
enum class EEquipableItemLevel : uint8;
enum class EConsumableItemType : uint8;

/// <summary>
/// Enemy의 Default Weapon과 Item의 기본 Spawn 처리를 돕는 Component
/// </summary>
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

private:
	
	/// <summary>
	/// 기본 무기 및 기본 아이템 스폰해서 Inven 및 EquippedComponent에 setting
	/// </summary>
	void SpawnDefaultWeaponsAndItems();

public:
	
	/// <summary>
	/// 기본 Spawn 처리한 Item들 중 직접적으로 Attach되어 있는 Actor들의 HiddenInGame Toggle 처리 
	/// </summary>
	void ToggleSpawnedItemsHiddenInGame(bool InHiddenInGame);

private:

	void SpawnWeapons(const FActorSpawnParameters& Param);
	void SpawnEquipableItems(const FActorSpawnParameters& Param);
	void SpawnConsumableItems(const FActorSpawnParameters& Param);
	void SpawnBullets(const FActorSpawnParameters& Param);
	
public:

	void SetOwnerEnemy(class AC_Enemy* InOwnerEnemy) { OwnerEnemy = InOwnerEnemy; }

	TSubclassOf<class AC_Helmet> GetHelmetClass(EEquipableItemLevel InHelmetLevel) { return HelmetClasses[InHelmetLevel]; }
	TSubclassOf<class AC_ConsumableItem> GetConsumableItemClass(EConsumableItemType InConsumableItemType) { return ConsumableItemClasses[InConsumableItemType]; }
	TSubclassOf<class AC_ThrowingWeapon> GetThrowableClass(EThrowableType InThrowableType) { return ThrowableClasses[InThrowableType]; }

private:

	AC_Enemy* OwnerEnemy{};

protected: // Weapon 관련 Classes
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EWeaponSlot, TSubclassOf<class AC_Weapon>> WeaponClasses{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EThrowableType, TSubclassOf<AC_ThrowingWeapon>> ThrowableClasses{};

protected: // Equipable 관련 Classes

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class AC_Vest> VestClass{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EEquipableItemLevel, TSubclassOf<AC_Helmet>> HelmetClasses{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EEquipableItemLevel, TSubclassOf<class AC_BackPack>> BackPackClasses{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EConsumableItemType, TSubclassOf<AC_ConsumableItem>> ConsumableItemClasses{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EBulletType, TSubclassOf<class AC_Item_Bullet>> BulletClasses{};
};
