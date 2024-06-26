// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Weapon.generated.h"

UCLASS()
class UE_PUBG_API AC_Weapon : public AActor
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

public: /* 각 Key에 따른 무기 작동 */

	/// <summary>
	/// B Key weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteBKey();

	/// <summary>
	/// R Key weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteRKey();

	/// <summary>
	/// MLB Started weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMlb_Started();

	/// <summary>
	/// MLB OnGoing weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMlb_OnGoing();

	/// <summary>
	/// MLB Completed weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMlb_Completed();

	/// <summary>
	/// MRB started weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMrb_Started();

	/// <summary>
	/// MRB OnGoing weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMrb_OnGoing();

	/// <summary>
	/// MRB Completed weapon method
	/// </summary>
	/// <returns> 실행 중 실패하거나, 매칭되는 액션이 없을 때 return false </returns>
	bool ExecuteMrb_Completed();

public:

	void SetOwnerCharacter(class C_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

protected:

	// 무기에 따른 각 버튼에 대한 strategy 알고리즘 객체
	class II_WeaponButtonStrategy* WeaponButtonStrategy{};

	class C_BasicCharacter* OwnerCharacter{};

};
