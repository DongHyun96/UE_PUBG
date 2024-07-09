// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_InvenComponent.generated.h"

enum class EBackPackLevel
{
	LV0,
	LV1,
	LV2,
	LV3
};

/// <summary>
/// C_InvenComponent는 0~3레벨까지 존재하며 이는 가방의 레벨에 따라서 변화한다.
///가방은 자동장착되며 레벨을 올리는데는 제한이 없지만
///다운그레이드의 경우 다운되는 레벨의 무게를 초과하는 인벤상태라면
///불가능하다.
/// EquipmentSystem을 Blueprint에서 만들어서 거기서 InvenComponent와 EquippedComponent를 사용해서 인벤시스템와 UI를 제작.
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_InvenComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_InvenComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	//가방교체, 아이템획득시 해당함수로 용량이 충분한지 확인하여 T or F를 반환. 블루프린트에서 사용 가능하도록 열어줘야 할 수 있음.
	UFUNCTION(BlueprintCallable)
	bool CheckVolume(uint8 volume);

	void ChackMyBackPack();

	void Interaction(class AC_Item wilditem);
protected:
	AC_BasicCharacter* OwnerCharacter{};

	uint8 maxVolume = 70;
	uint8 curVolume =  0;

	EBackPackLevel curBackPackLevel = EBackPackLevel::LV0;

protected:


	
};
