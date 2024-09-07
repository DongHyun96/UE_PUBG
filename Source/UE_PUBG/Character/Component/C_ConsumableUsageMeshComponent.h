// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_ConsumableUsageMeshComponent.generated.h"


UENUM(BlueprintType)
enum class EConsumableUsageMeshType : uint8
{
	BANDAGE,
	SYRINGE, // �ֻ��
	DRINK,
	PAIN_KILLER,
	MAX
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_ConsumableUsageMeshComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_ConsumableUsageMeshComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// ������ ��� �� Ȱ��ȭ �� Usage Mesh visible �Ѱ� ����
	/// </summary>
	/// <param name="MeshTye"> : Toggle��ų Mesh Ÿ�� </param>
	/// <param name="Visible"> : Visible true or false </param>
	void ToggleMeshUsageVisible(EConsumableUsageMeshType MeshType, bool Visible);

private:

	/// <summary>
	/// Enum���� �����Ǵ� FString �� return
	/// </summary>
	FString GetMeshTypeName(EConsumableUsageMeshType Type);

private:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EConsumableUsageMeshType, class UStaticMeshComponent*> UsageMeshMap{};

private:

	//const FName HOLSTER_SOCKET_NAME = "Pan_Holster"; // ������ socket �̸�
	const TMap<EConsumableUsageMeshType, FName> SocketNames =
	{
		{EConsumableUsageMeshType::BANDAGE,		"Bandage_ArmSocket"},
		{EConsumableUsageMeshType::SYRINGE,		"SyringeSocket"},
		{EConsumableUsageMeshType::DRINK,		"DrinkUsageSocket"},
		{EConsumableUsageMeshType::PAIN_KILLER, "PainKillerUsageSocket"},

	};
};
