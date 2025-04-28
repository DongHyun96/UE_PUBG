// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/kismetSystemLibrary.h"
#include "Sound/SoundCue.h"
#include "C_FeetComponent.generated.h"
//#include "C:/Program Files/Epic Games/UE_5.4/Engine/Source/Runtime/PhysicsCore/Public/Chaos/ChaosEngineInterface.h"

USTRUCT(BlueprintType)
struct FFeetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector LeftDistance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector RightDistance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector RootBoneDistance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FRotator LeftRotation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FRotator RightRotation;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_FeetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_FeetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Trace(FName InName, float& OutDistance, FRotator& OutRotation);

	FFeetData GetData() { return Data; }

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType() { return CurrentSurfaceType; }

	void PlaySoundInTick(float DeltaTime);

	/// <summary>
	/// FeetComponent내부에서 발소리를 재생하려고 했는데 쉽지 않아서
	/// 애니메이션에 Notify를 사용하는 것으로 선회.
	/// </summary>
	/// <param name="InCurSurFaceTpye"></param>
	/// <param name="InLocation"></param>
	void PlaySoundCue(EPhysicalSurface InCurSurFaceTpye, FVector InLocation, float InVolumeMultiplier);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float InterpSpeed = 5.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float TraceDistance = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float OffsetDistance = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName LeftSocket = "LeftFoot";

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName RightSocket = "RightFoot";

protected:
	/// <summary>
	/// EPhysicalSurface를 바로 Key로 사용할 수 없음.
	/// EPhysicalSurface는 UEnum값이 아니라 #define으로 정의된 C++ Enum값임.
	/// 그래서 TEnumAsByte로 감싸서(캐스팅)해서 사용해야함.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> SurfaceTypeToSoundCueMap{};
private:
	class AC_BasicCharacter* OwnerCharacter = nullptr;

	FFeetData Data{};

	EPhysicalSurface CurrentSurfaceType = EPhysicalSurface::SurfaceType_Default;

	float LastFootstepTime = 0.0f;

	bool bWasOnGroundLastFrame = false;

	float AccumulatedFootstepTime = 0.f;
};
