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
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
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
	
	//UFUNCTION(BlueprintCallable)
	//uint16�� �ش� ��ũ�ΰ� �������� �ʴ´�. uint8, uint32�� �����Ѵ�.
	bool CheckVolume(uint16 volume);

	bool ChackMyBackPack(class AC_BackPack* backpack);

	void Interaction(class AC_Item wilditem);

	uint16 CheckBackPackVolume(uint16 backpacklevel);
	uint16 CheckBackPackVolume(EBackPackLevel backpacklevel);


public://Getter and Seter
	EBackPackLevel GetCurBackPackLevel() { return CurBackPackLevel; }
	//EBackPackLevel SetCurBackPackLevel(uint8 level) { CurBackPackLevel = (EBackPackLevel)level; }

	uint16 GetMaxVolume() { return MaxVolume; }
	uint16 GetCurVolume() { return CurVolume; }

	class AC_BackPack* GetMyBackPack() { return MyBackPack; }

	TArray<class AC_Item*> GetNearItems() { return NearItems; }

protected:
	AC_BasicCharacter* OwnerCharacter{};

	uint16 MaxVolume = 70;
	uint16 CurVolume =  0;

	EBackPackLevel CurBackPackLevel = EBackPackLevel::LV0;
	EBackPackLevel PreBackPackLevel = EBackPackLevel::LV0;

	/// <summary>
	/// �⺻ : Ui���� ���� �� �����۸���� �����ֱ� ����.
	/// �߰� : �׾����� ���� �� ������ ���.(���̳� ���氰�� ���������۵鵵 ��������ϴµ� �̰� �׾����� �ѹ��� ������ ���� ����� 2������ ����. 
	/// ù��°�� ������ ���پ��ִ°�. 
	/// �ι�°�� MyItem�� �� �ְ� �ѹ��� ������ ��.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class AC_Item*> MyItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class AC_Item*> NearItems;
protected:
	
private:
	AC_BackPack* MyBackPack {};


	
};
