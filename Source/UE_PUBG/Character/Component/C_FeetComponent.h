// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/kismetSystemLibrary.h"
#include "Sound/SoundCue.h"
#include "C_FeetComponent.generated.h"


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

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetLeftSurfaceType() const {return CurrentLeftSurfaceType; }
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetRightSurfaceType() const {return CurrentRightSurfaceType; }
	
	FFeetData GetData() const { return Data; }

private:

	void Trace(const FName& InName, float& OutDistance, FRotator& OutRotation, EPhysicalSurface& OutSurfaceType);

protected:
	/// <summary>
	/// FeetComponent내부에서 발소리를 재생하려고 했는데 쉽지 않아서
	/// 애니메이션에 Notify를 사용하는 것으로 선회.
	/// -->현재 사용중.
	/// </summary>
	/// <param name="InCurSurFaceType"></param>
	/// <param name="InLocation"></param>
	/// <param name="InVolumeMultiplier"></param>
	/// <param name="bLeftFootSound"> : LeftFoot 쪽 Sound인지 </param>
	UFUNCTION(BlueprintCallable)
	void PlaySoundCue(EPhysicalSurface InCurSurFaceType, FVector InLocation, float InVolumeMultiplier, bool bLeftFootSound);

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
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> SurfaceTypeToSoundCueMap{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> LeftSurfaceTypeToSoundCueMap{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> RightSurfaceTypeToSoundCueMap{};
private:
	
	class AC_BasicCharacter* OwnerCharacter = nullptr;

	FFeetData Data{};

	EPhysicalSurface CurrentLeftSurfaceType  = SurfaceType_Default;
	EPhysicalSurface CurrentRightSurfaceType = SurfaceType_Default;

	float LastFootstepTime = 0.0f;

	bool bWasOnGroundLastFrame = false;

	float AccumulatedFootstepTime = 0.f;
	
};
