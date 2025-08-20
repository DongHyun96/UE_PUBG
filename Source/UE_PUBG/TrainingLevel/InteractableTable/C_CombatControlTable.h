// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_InteractableTable.h"
#include "C_CombatControlTable.generated.h"

UCLASS()
class UE_PUBG_API AC_CombatControlTable : public AC_InteractableTable
{
	GENERATED_BODY()

public:
	AC_CombatControlTable();

protected:
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

private: // IMC_CombatSpectator Actions
	
	/// <summary>
	/// <para> Player InputComponent의 F Key Delegate Callback 처리 </para>
	/// <para> 담당 기능 : Toggle Start/Stop Simulating Enemy AI fight </para>
	/// </summary>
	/// <returns> : 이 Table이 Focused되어 있고, 제대로 Interaction 처리되었다면 return true </returns>
	bool OnPlayerInputComponentFKeyDelegate();

	void OnFKey();

	/// <summary>
	/// 왼쪽 방향키 Input callback함수 / 관전자 전환에 활용
	/// </summary>
	void OnArrowLeft();

	/// <summary>
	/// 오른쪽 방향키 Input callback함수 / 관전자 전환에 활용
	/// </summary>
	void OnArrowRight();

	/// <summary>
	/// 마우스 Input callback 함수
	/// </summary>
	void OnLookInput(const struct FInputActionValue& Value);

private:

	void OnBoxColliderBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	) override;

	void OnBoxColliderEndOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex
	) override;

private:

	// CombatFieldManager가 들고있는 CombatFieldWidget
	class UC_CombatFieldWidget* CombatFieldWidget{};

protected: // Input 처리 관련

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UInputMappingContext* IMC_CombatSpectator{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UInputAction* ArrowLeftAction{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInputAction* ArrowRightAction{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInputAction* FKeyAction{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInputAction* LookAction{};	

private:
	
};
