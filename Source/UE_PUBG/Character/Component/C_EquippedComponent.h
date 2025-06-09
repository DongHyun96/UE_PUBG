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

	void SetCurWeaponTypeToNone() { CurWeaponType = EWeaponSlot::NONE; }

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
	/// HandState Gun인 경우(현재 총기를 들고 있는 상태인 경우), Gun slot끼리 Swap하는 상황에 대한 SetSlotWeapon 예외처리 함수
	/// </summary>
	/// <returns> Slot끼리 Swap이 제대로 이루어졌다면 return true </returns>
	bool SwapSlotsWhileGunHandState();

	void SetNextWeaponType(EWeaponSlot InNextWeaponType) { NextWeaponType = InNextWeaponType; }
	EWeaponSlot GetNextWeaponType() const { return NextWeaponType; }
	
	EWeaponSlot GetCurWeaponType() const { return CurWeaponType; }

	TMap<EWeaponSlot, AC_Weapon*> GetWeapons() { return Weapons; }

	bool GetIsCurrentlyChangingWeapon() const { return bIsCurrentlyChangingWeapon; }
	void SetIsCurrentlyChangingWeapon(bool InIsCurrentlyChangingWeapon) { bIsCurrentlyChangingWeapon = InIsCurrentlyChangingWeapon; }
	
public:
	/// <summary>
	/// 현재 손에 든 무기 바꾸기
	/// </summary>
	/// <param name=""> : 바꿔서 들려고 하는 무기 슬롯 종류 </param>
	/// <returns> 바꾸기 성공했다면 return true </returns>
	UFUNCTION(BlueprintCallable)
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
	UFUNCTION(BlueprintCallable)
	bool TryAttachCurWeaponToHolsterWithoutSheathMotion();

	/// <summary>
	/// <para> AttachCurWeaponToHolsterWithoutSheathMotion 이후 CurWeapon 손에 다시 부착 </para>
	/// <para> 주의 : AttachCurWeaponToHolsterWithoutSheathMotion과 쌍으로 사용할 것 </para>
	/// </summary>
	/// <returns> : 현재 들고 있는 무기(CurWeapon)가 없다면 return false</returns>
	UFUNCTION(BlueprintCallable)
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

	/// <summary>
	/// 각 무기에 부착된 Attachment -> Param으로 들어온 CollisionParams ignoreActors에 추가
	/// </summary>
	/// <param name="CollisionParams"> : Param </param>
	void AddAttachedPartsActorsToIgnoreActors(FCollisionQueryParams& CollisionParams);

protected:

	class AC_BasicCharacter* OwnerCharacter{};

protected:
	
	UPROPERTY(VisibleAnywhere)
	EWeaponSlot CurWeaponType{};

	EWeaponSlot NextWeaponType{};
	EWeaponSlot PrevWeaponType{};

protected:

	/// <summary>
	/// 슬롯 별 Weapon들, 주의 : SetSlotWeapon시에 SetActorEnableCollision(false)처리가 들어감
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, class AC_Weapon*> Weapons{};

protected:
	void SetMainGunOrSubGun(EWeaponSlot InSlot);

private:

	// 현재 Weapon을 바꾸는 중인지 check
	bool bIsCurrentlyChangingWeapon{};

};


