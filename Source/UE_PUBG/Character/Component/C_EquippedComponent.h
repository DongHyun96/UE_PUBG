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

};
