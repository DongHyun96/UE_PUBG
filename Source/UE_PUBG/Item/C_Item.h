
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Item.generated.h"

/// <summary>
/// AttachableItem PartsName Enum class
/// C_Gun���� ���� ���� ����
/// </summary>
UENUM(BlueprintType)
enum class EPartsName : uint8
{
	NONE,
	SCOPE,
	MUZZLE,
	GRIP,
	MAGAZINE,
	GUNSTOCK
};
/// <summary>
/// ���� ����� ��������� �ʰ� ����.
/// </summary>
USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemNums;
};
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

UENUM(BlueprintType)
enum class EItemNames : uint8
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

	virtual void Interaction(class AC_BasicCharacter* character) PURE_VIRTUAL(AC_Item::Interaction, );

	//�������� ������ �����ϰ� ���� ������ ����.
	UFUNCTION(BlueprintCallable)
	virtual void DetachmentItem();

	virtual void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	UFUNCTION(BlueprintCallable)
	uint8 GetVolume() { return Volume; }

	virtual AC_BasicCharacter* GetOwnerCharacter() { return OwnerCharacter; }
private:
	uint8 Volume;
	

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	EItemTypes MyItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	UTexture2D* ItemIcon;		

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	uint8 ItemNums;  // uint8 -> 0 ~ 255�����ۿ� �ȵ� (ź ������ ���� �� �÷���� ��)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	AC_BasicCharacter* OwnerCharacter{};
};
// Fill out your copyright notice in the Description page of Project Settings.
