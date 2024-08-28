// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Item.generated.h"
/// <summary>
/// �������� Ÿ���� �����༭ �κ��丮���� �����Ҷ� �˸´� UI�� ã�ư����� �ϰ�����.
/// NONE�� ���߻�Ȳ�� ����ؼ� ������.
/// ����Ŭ���������� �����ڿ��� �����ϰ� ����.
/// </summary>
UENUM(BlueprintType)
enum class EItemTypes : uint8
{
	NONE,
	HELMET,
	ARMORE,
	BACKPACK,
	MAINGUN,
	MELEEWEAPON,
	THROWABLE,
	ATTACHMENT,
	CONSUMPTIONITEM
};

/// <summary>
/// �Ҹ�����ۺз��� ���� ���� Ŭ����.
/// ���濡 ���� �������� ���� Ŭ����.
/// ��)ź, ������, ����, ����ź��.
/// ���濡 ���� ����ǰ ��� �ۼ��ؾ���.
/// </summary>
UCLASS(abstract)
class UE_PUBG_API AC_Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	//virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	uint8 GetVolume() { return Volume; }

	virtual void Interaction(class AC_BasicCharacter* character) PURE_VIRTUAL(AC_Item::Interaction;);



private:
	uint8 Volume;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EItemTypes MyItemType;
};
