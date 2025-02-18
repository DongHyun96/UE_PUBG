// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_ConsumableUsageMeshComponent.generated.h"


UENUM(BlueprintType)
enum class EConsumableUsageMeshType : uint8
{
	BANDAGE,
	SYRINGE, // 주사기
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
	/// 아이템 사용 및 활성화 시 Usage Mesh visible 켜고 끄기
	/// </summary>
	/// <param name="MeshTye"> : Toggle시킬 Mesh 타입 </param>
	/// <param name="Visible"> : Visible true or false </param>
	void ToggleMeshUsageVisible(EConsumableUsageMeshType MeshType, bool Visible);

private:

	/// <summary>
	/// Enum값에 대응되는 FString 값 return
	/// </summary>
	FString GetMeshTypeName(EConsumableUsageMeshType Type);

private:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EConsumableUsageMeshType, class UStaticMeshComponent*> UsageMeshMap{};

private:

	//const FName HOLSTER_SOCKET_NAME = "Pan_Holster"; // 무기집 socket 이름
	static const TMap<EConsumableUsageMeshType, FName> SocketNames;
	
};


