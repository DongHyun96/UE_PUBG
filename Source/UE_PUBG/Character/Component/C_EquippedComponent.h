// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_EquippedComponent.generated.h"


UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	NONE,
	MAIN_GUN,
	SUB_GUN,
	MELEE_WEAPON,
	THROWABLE_WEAPON
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_EquippedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_EquippedComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	UFUNCTION(BlueprintCallable)
	class AC_Weapon* GetCurWeapon() const { return Weapons[CurWeaponType]; }
	
	/// <summary>
	/// 슬롯에 무기 장착하기 / 해제는 Weapon에 nullptr를 줄 것 -> 장착/해제는 이 함수를 통해서 무조건 할 것
	/// </summary>
	/// <param name="InSlot"> : 장착할 슬롯 </param>
	/// <param name="Weapon"> : 장착할 무기 객체 / 장착 해제는 nullptr </param>
	/// <returns> : 해당 slot의 이전 무기 (없었다면 return nullptr) </returns>
	UFUNCTION(BlueprintCallable)
	AC_Weapon* SetSlotWeapon(EWeaponSlot InSlot, class AC_Weapon* Weapon);

	/// <summary>
	/// 슬롯에 장착된 무기를 떨구기.
	/// </summary>
	/// <param name="InSlot"> : 떨구고 싶은 아이템이 있는 슬롯 </param>
	UFUNCTION(BlueprintCallable)
	void DetachmentWeapon(EWeaponSlot InSlot);

	void SetNextWeaponType(EWeaponSlot InNextWeaponType) { NextWeaponType = InNextWeaponType; }
	EWeaponSlot GetNextWeaponType() const { return NextWeaponType; }
	
	EWeaponSlot GetCurWeaponType() const { return CurWeaponType; }

	UFUNCTION(BlueprintCallable)
	TMap<EWeaponSlot, AC_Weapon*> GetWeapons() { return Weapons; }
public:
	/// <summary>
	/// 현재 손에 든 무기 바꾸기
	/// </summary>
	/// <param name=""> : 바꿔서 들려고 하는 무기 슬롯 종류 </param>
	/// <returns> 바꾸기 성공했다면 return true </returns>
	bool ChangeCurWeapon(EWeaponSlot InChangeTo);

	/// <summary>
	/// X키를 통한 무기 집어넣기 및 직전 무기 꺼내기
	/// </summary>
	/// <returns></returns>
	bool ToggleArmed();

public: // 특정 동작 처리에서의 들고 있는 무기 임시 처리 관련 ( ex -> 무기 들고 있는 중 파쿠르, 무기 들고 있는 중 ConsumableItem 사용)

	/// <summary>
	/// 현재 들고 있는 무기가 있다면 Sheath motion 없이 임시로 바로 Holster에 붙이기 (주의 : return true일 경우, 
	/// </summary>
	/// <returns> : 현재 들고 있는 무기(CurWeapon)가 없다면 return false</returns>
	bool TryAttachCurWeaponToHolsterWithoutSheathMotion();

	/// <summary>
	/// <para> AttachCurWeaponToHolsterWithoutSheathMotion 이후 CurWeapon 손에 다시 부착 </para>
	/// <para> 주의 : AttachCurWeaponToHolsterWithoutSheathMotion과 쌍으로 사용할 것 </para>
	/// </summary>
	/// <returns> : 현재 들고 있는 무기(CurWeapon)가 없다면 return false</returns>
	bool TryReAttachCurWeaponToHand();

public:

	/// <summary>
	/// 무기 Sheath가 끝났을 시 Notify로 불러질 call back 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnSheathEnd();

	/// <summary>
	/// 무기 Draw가 시작시 Notify로 불러질 call back 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDrawStart();


	/// <summary>
	/// 무기 Draw가 끝났을 시 Notify로 불러질 call back 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDrawEnd();

	UFUNCTION(BlueprintCallable)
	void OnReloadEnd();
	UFUNCTION(BlueprintCallable)
	void OnSniperReloadEnd();
private:

	/// <summary>
	/// Weapon Testing용 스폰 함수 - TODO(생성자 쪽 & 이 함수 지워버리기)
	/// </summary>
	void SpawnWeaponsForTesting();


protected:

	class AC_BasicCharacter* OwnerCharacter{};

protected:
	
	UPROPERTY(VisibleAnywhere)
	EWeaponSlot CurWeaponType{};

	EWeaponSlot NextWeaponType{};
	EWeaponSlot PrevWeaponType{};

protected:

	/// <summary>
	/// 슬롯 별 Weapon들
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, class AC_Weapon*> Weapons{};

protected:

	// class UC_EquippedArmorComponent* ArmorComponent{};

protected:

	// Weapon Test 용
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, TSubclassOf<class AC_Weapon>> WeaponClasses{};

protected:
	void SetMainGunOrSubGun(EWeaponSlot InSlot);

};
