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

	FFeetData()
		: LeftDistance(FVector::ZeroVector)
		, RightDistance(FVector::ZeroVector)
		, RootBoneDistance(FVector::ZeroVector)
		, LeftRotation(FRotator::ZeroRotator)
		, RightRotation(FRotator::ZeroRotator)
	{}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector LeftDistance{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector RightDistance{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector RootBoneDistance{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FRotator LeftRotation{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FRotator RightRotation{};
};

// Foot Sound 관련 Descriptor
// Left와 Right foot 각각 이 Descriptor를 사용함
USTRUCT(BlueprintType)
struct FFootSoundDescriptor
{
	GENERATED_BODY()

	/// <summary>
	/// EPhysicalSurface를 바로 Key로 사용할 수 없음.
	/// EPhysicalSurface는 UEnum값이 아니라 #define으로 정의된 C++ Enum값임.
	/// 그래서 TEnumAsByte로 감싸서(캐스팅)해서 사용해야함.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> SurfaceTypeToSoundCueMap{};

	EPhysicalSurface CurrentSurfaceType = SurfaceType_Default;

	// 기존의 발자국 소리 재생은 UGameplayStatics로 바로 재생했었음
	// 현재 Water 높이에 따른 WaterFeetSoundCue의 Branch node param 처리를 위해 AudioComponent로 소리 재생 처리로 수정
	UAudioComponent* FootAudioComponent{};

	// Water Surface Type이 검출되었을 때, 깊이가 충분히 깊어 DeepWater sound로 발소리를 처리해야 하는지
	bool bIsDeepWater{};
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
	EPhysicalSurface GetLeftSurfaceType() const {return LeftFootSoundDescriptor.CurrentSurfaceType; }
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetRightSurfaceType() const {return RightFootSoundDescriptor.CurrentSurfaceType; }
	
	FFeetData GetData() const { return Data; }

private:

	void Trace(const FName& InName, float& OutDistance, FRotator& OutRotation, FFootSoundDescriptor& OutSoundDescriptor);

protected:
	/// <summary>
	/// Not in used
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

	/// <summary>
	/// Water Sound cue -> Branch Node parameter 지정 때문에 AudioComponent를 사용하여 발소리 내기 처리까지 포함
	/// </summary>
	/// <param name="InCurSurFaceType"></param>
	/// <param name="InVolumeMultiplier"></param>
	/// <param name="bLeftFootSound"></param>
	UFUNCTION(BlueprintCallable)
	void PlaySoundByAudioComponent(EPhysicalSurface InCurSurFaceType, float InVolumeMultiplier, bool bLeftFootSound);
	
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
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Footstep")
	FFootSoundDescriptor LeftFootSoundDescriptor{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Footstep")
	FFootSoundDescriptor RightFootSoundDescriptor{};
	
private:
	
	class AC_BasicCharacter* OwnerCharacter = nullptr;

	FFeetData Data{};

};
