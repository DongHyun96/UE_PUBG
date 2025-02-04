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

enum class EThrowableType : uint8;

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
	/// ���Կ� ���� �����ϱ� / ������ Weapon�� nullptr�� �� �� -> ����/������ �� �Լ��� ���ؼ� ������ �� ��
	/// </summary>
	/// <param name="InSlot"> : ������ ���� </param>
	/// <param name="Weapon"> : ������ ���� ��ü / ���� ������ nullptr </param>
	/// <returns> : �ش� slot�� ���� ���� (�����ٸ� return nullptr) </returns>
	UFUNCTION(BlueprintCallable)
	AC_Weapon* SetSlotWeapon(EWeaponSlot InSlot, class AC_Weapon* Weapon);

	/// <summary>
	/// HandState Gun�� ���(���� �ѱ⸦ ��� �ִ� ������ ���), Gun slot���� Swap�ϴ� ��Ȳ�� ���� SetSlotWeapon ����ó�� �Լ�
	/// </summary>
	/// <returns> Slot���� Swap�� ����� �̷�����ٸ� return true </returns>
	bool SwapSlotsWhileGunHandState();

	/// <summary>
	/// ���Կ� ������ ���⸦ ������.
	/// </summary>
	/// <param name="InSlot"> : ������ ���� �������� �ִ� ���� </param>
	UFUNCTION(BlueprintCallable)
	void DetachmentWeapon(EWeaponSlot InSlot);

	void SetNextWeaponType(EWeaponSlot InNextWeaponType) { NextWeaponType = InNextWeaponType; }
	//EWeaponSlot GetNextWeaponType() const { return NextWeaponType; }
	
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

public: // Ư�� ���� ó�������� ��� �ִ� ���� �ӽ� ó�� ���� ( ex -> ���� ��� �ִ� �� ����, ���� ��� �ִ� �� ConsumableItem ���)

	/// <summary>
	/// ���� ��� �ִ� ���Ⱑ �ִٸ� Sheath motion ���� �ӽ÷� �ٷ� Holster�� ���̱� (���� : return true�� ���, 
	/// </summary>
	/// <returns> : ���� ��� �ִ� ����(CurWeapon)�� ���ٸ� return false</returns>
	bool TryAttachCurWeaponToHolsterWithoutSheathMotion();

	/// <summary>
	/// <para> AttachCurWeaponToHolsterWithoutSheathMotion ���� CurWeapon �տ� �ٽ� ���� </para>
	/// <para> ���� : AttachCurWeaponToHolsterWithoutSheathMotion�� ������ ����� �� </para>
	/// </summary>
	/// <returns> : ���� ��� �ִ� ����(CurWeapon)�� ���ٸ� return false</returns>
	bool TryReAttachCurWeaponToHand();

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
	UFUNCTION(BlueprintCallable)
	void OnSniperReloadEnd();

	/// <summary>
	/// �� ���⿡ ������ Attachment -> Param���� ���� CollisionParams ignoreActors�� �߰�
	/// </summary>
	/// <param name="CollisionParams"> : Param </param>
	void AddAttachedPartsActorsToIgnoreActors(FCollisionQueryParams& CollisionParams);

public:

	TSubclassOf<class AC_Weapon> GetSubclassOfWeapon(EWeaponSlot WeaponSlot) const { return WeaponClasses[WeaponSlot]; }
	
	TMap<EThrowableType, TSubclassOf<class AC_ThrowingWeapon>>& GetSubclassOfThrowingWeapon() { return ThrowableClasses; }

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

	// Enemy Weapon setting ��
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EWeaponSlot, TSubclassOf<class AC_Weapon>> WeaponClasses{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EThrowableType, TSubclassOf<class AC_ThrowingWeapon>> ThrowableClasses{};

protected:
	void SetMainGunOrSubGun(EWeaponSlot InSlot);

};
