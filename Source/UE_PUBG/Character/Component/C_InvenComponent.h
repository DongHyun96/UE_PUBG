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
/// C_InvenComponent�� 0~3�������� �����ϸ� �̴� ������ ������ ���� ��ȭ�Ѵ�.
///������ �ڵ������Ǹ� ������ �ø��µ��� ������ ������
///�ٿ�׷��̵��� ��� �ٿ�Ǵ� ������ ���Ը� �ʰ��ϴ� �κ����¶��
///�Ұ����ϴ�.
/// EquipmentSystem�� Blueprint���� ���� �ű⼭ InvenComponent�� EquippedComponent�� ����ؼ� �κ��ý��ۿ� UI�� ����.
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

	//���汳ü, ������ȹ��� �ش��Լ��� �뷮�� ������� Ȯ���Ͽ� T or F�� ��ȯ. �������Ʈ���� ��� �����ϵ��� ������� �� �� ����.
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
