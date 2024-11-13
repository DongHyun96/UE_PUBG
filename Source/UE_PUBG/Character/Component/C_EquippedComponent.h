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
	/// ���Կ� ���� �����ϱ� / ������ Weapon�� nullptr�� �� �� -> ����/������ �� �Լ��� ���ؼ� ������ �� ��
	/// </summary>
	/// <param name="InSlot"> : ������ ���� </param>
	/// <param name="Weapon"> : ������ ���� ��ü / ���� ������ nullptr </param>
	/// <returns> : �ش� slot�� ���� ���� (�����ٸ� return nullptr) </returns>
	UFUNCTION(BlueprintCallable)
	AC_Weapon* SetSlotWeapon(EWeaponSlot InSlot, class AC_Weapon* Weapon);

	/// <summary>
	/// ���Կ� ������ ���⸦ ������.
	/// </summary>
	/// <param name="InSlot"> : ������ ���� �������� �ִ� ���� </param>
	UFUNCTION(BlueprintCallable)
	void DetachmentWeapon(EWeaponSlot InSlot);

	void SetNextWeaponType(EWeaponSlot InNextWeaponType) { NextWeaponType = InNextWeaponType; }
	EWeaponSlot GetNextWeaponType() const { return NextWeaponType; }
	
	EWeaponSlot GetCurWeaponType() const { return CurWeaponType; }

	UFUNCTION(BlueprintCallable)
	TMap<EWeaponSlot, AC_Weapon*> GetWeapons() { return Weapons; }
public:
	/// <summary>
	/// ���� �տ� �� ���� �ٲٱ�
	/// </summary>
	/// <param name=""> : �ٲ㼭 ����� �ϴ� ���� ���� ���� </param>
	/// <returns> �ٲٱ� �����ߴٸ� return true </returns>
	bool ChangeCurWeapon(EWeaponSlot InChangeTo);

	/// <summary>
	/// XŰ�� ���� ���� ����ֱ� �� ���� ���� ������
	/// </summary>
	/// <returns></returns>
	bool ToggleArmed();

public:

	/// <summary>
	/// ���� Sheath�� ������ �� Notify�� �ҷ��� call back �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnSheathEnd();

	/// <summary>
	/// ���� Draw�� ���۽� Notify�� �ҷ��� call back �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDrawStart();


	/// <summary>
	/// ���� Draw�� ������ �� Notify�� �ҷ��� call back �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDrawEnd();

	UFUNCTION(BlueprintCallable)
	void OnReloadEnd();

private:

	/// <summary>
	/// Weapon Testing�� ���� �Լ� - TODO(������ �� & �� �Լ� ����������)
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
	/// ���� �� Weapon��
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, class AC_Weapon*> Weapons{};

protected:

	// class UC_EquippedArmorComponent* ArmorComponent{};

protected:

	// Weapon Test ��
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, TSubclassOf<class AC_Weapon>> WeaponClasses{};

protected:
	void SetMainGunOrSubGun(EWeaponSlot InSlot);

};
