// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UE_PUBG/Item/C_Item.h"
#include "GameFramework/Actor.h"
#include "C_Weapon.generated.h"

UCLASS(Abstract)
class UE_PUBG_API AC_Weapon : public AC_Item
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Weapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public: /* �� Key�� ���� ���� �۵� */

	/// <summary>
	/// B Key weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteBKey();

	/// <summary>
	/// R Key weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteRKey();

	/// <summary>
	/// MLB Started weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteMlb_Started();

	/// <summary>
	/// MLB OnGoing weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteMlb_OnGoing();

	/// <summary>
	/// MLB Completed weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteMlb_Completed();

	/// <summary>
	/// MRB started weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteMrb_Started();

	/// <summary>
	/// MRB OnGoing weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteMrb_OnGoing();

	/// <summary>
	/// MRB Completed weapon method
	/// </summary>
	/// <returns> ���� �� �����ϰų�, ��Ī�Ǵ� �׼��� ���� �� return false </returns>
	bool ExecuteMrb_Completed();

public:

	/// <summary>
	/// �������� ���� ���̱�
	/// </summary>
	/// <returns> ���̱⸦ �����ϰų� ���� �������� ���ٸ� return false </returns>
	virtual bool AttachToHolster(class USceneComponent* InParent) PURE_VIRTUAL(AC_Weapon::AttachToHolster, return false;);

	/// <summary>
	/// �տ� �����ϱ�
	/// </summary>
	/// <returns> ���⸦ �տ� ������ ���Ͽ��ٸ� return false </returns>
	virtual bool AttachToHand(class USceneComponent* InParent) PURE_VIRTUAL(AC_Weapon::AttachToHolster, return false;);


public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	class UAnimMontage* GetCurDrawMontage() const { return CurDrawMontage; }
	class UAnimMontage* GetCurSheathMontage() const { return CurSheathMontage; }

protected:

	// ���⿡ ���� �� ��ư�� ���� strategy �˰��� ��ü
	class II_WeaponButtonStrategy* WeaponButtonStrategy{};

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* CurDrawMontage{}; // ���� ���� ��ġ�� �ش��ϴ� ���� �̱� Anim montage

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* CurSheathMontage{}; // ���� ���� ��ġ�� �ش��ϴ� ���� ����ֱ� Anim montage

};
