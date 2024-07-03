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

	class AC_Weapon* GetCurWeapon() const { return CurWeapon; }

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
	/// ���� Draw�� ������ �� Notify�� �ҷ��� call back �Լ�
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDrawEnd();


protected:

	class AC_BasicCharacter* OwnerCharacter{};

protected:
	UPROPERTY(VisibleAnywhere)

	class AC_Weapon* CurWeapon{};	// ���� �տ� ��� �ִ� ���� (���ٸ� nullptr)
	
	class AC_Weapon* NextWeapon{};	// ���� ��ü �� �̿�, �������� �տ� �� ����

	class AC_Weapon* PrevWeapon{};	// ������ �����߾��� ����, xŰ UnArmed, Armed ��ۿ� ���� ����

protected:

	class AC_Gun* MainGun{};	// ������ �ֹ��� 1
	class AC_Gun* SubGun{};	// ������ �ֹ��� 2

	UPROPERTY(VisibleAnywhere)
	class AC_MeleeWeapon* MeleeWeapon{};	// ������ ��������

	class AC_ThrowingWeapon* ThrowingWeapon{};	// ������ ��ô�� ����

protected:

	// class UC_EquippedArmorComponent* ArmorComponent{};

protected:

	// Weapon Test ��
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, TSubclassOf<class AC_Weapon>> WeaponClasses{};

};
