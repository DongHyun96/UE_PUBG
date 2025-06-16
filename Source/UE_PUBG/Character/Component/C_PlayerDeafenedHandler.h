// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_PlayerDeafenedHandler.generated.h"


/// <summary>
/// 카메라가 물 속에 들어가 있을 경우, or 수류탄 또는 섬광탄에 피격 당했을 경우 Deafened SFX 효과 처리 실행
/// </summary>
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_PlayerDeafenedHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UC_PlayerDeafenedHandler();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

	void ExecuteDeafenedEffect(float DeafenedDuration);

private:
	
	/// <summary>
	/// Main Camera가 물 속에 들어갔다면, 물 속 SFX 처리 담당
	/// </summary>
	/// <returns> : MainCamera가 현재 물 속에 있다면 return true </returns>
	bool HandleMainCameraUnderWater();

private:

	AC_Player* OwnerPlayer{};
	class UCameraComponent* PlayerMainCamera{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundMix* DeafenedMix{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundBase* UnderWaterSFX{};	

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> CamWaterTraceChannel{};
	
private:
	
	UAudioComponent* UnderWaterAudioComponent{};
	
private:

	float DeafenedTime{};

	static FCollisionQueryParams CollisionParams;

	bool bDeafened{};
	
};