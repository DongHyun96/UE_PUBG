// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SkyDivingComponent.generated.h"

UENUM(BlueprintType)
enum class ESkyDivingState : uint8
{
	READY,			// ����⿡ ���� �ִ� ����
	SKYDIVING,		
	PARACHUTING,
	LANDING,			// ���� ���� ���
	MAX
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_SkyDivingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_SkyDivingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// SkyDiving�ÿ� Player InputComponent Move �Լ� ������ ȣ��� �Լ�
	/// </summary>
	/// <param name="MovementVector"></param>
	void HandlePlayerMovement(const FVector2D& MovementVector);

	void OnSkyMoveEnd();

public:

	void SetOwnerCharcter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	ESkyDivingState GetSkyDivingState() const { return SkyDivingState; }
	void SetSkyDivingState(ESkyDivingState InSkyDivingState);


private:

	class AC_BasicCharacter* OwnerCharacter{};

	//ESkyDivingState SkyDivingState = ESkyDivingState::LANDING;
	ESkyDivingState SkyDivingState = ESkyDivingState::READY;

};
