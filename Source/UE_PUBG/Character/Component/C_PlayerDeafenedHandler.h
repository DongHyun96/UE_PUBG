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
	
	UFUNCTION()
	void OnCameraWaterBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

private:

	AC_Player* OwnerPlayer{};
	class UCameraComponent* PlayerMainCamera{};

	float DeafenedTime{};
	bool  bDeafened{};
	
	UAudioComponent* UnderWaterAudioComponent{};
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundMix* DeafenedMix{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundBase* UnderWaterSFX{};	

private:
	
	UShapeComponent* CameraWaterCheckingCollider{};
	float CameraWaterEnteredZ{}; // 카메라가 물에 담긴 순간의 해당 물의 z값

	bool bIsCameraUnderWater{};
	bool bPendingCheckingCameraUnderWaterOnTickComponent{};
};


