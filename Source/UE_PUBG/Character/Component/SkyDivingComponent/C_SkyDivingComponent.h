// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "Components/ActorComponent.h"
#include "Singleton/C_GameInstance.h"

#include "C_SkyDivingComponent.generated.h"

enum class ELevelType : uint8;

UENUM(BlueprintType)
enum class ESkyDivingState : uint8
{
	READY,			// 비행기에 아직 있는 상태
	SKYDIVING,		
	PARACHUTING,
	LANDING,			// 착지 동작 재생
	MAX
};

// Level 종류에 따른 SKyDiving 관련 Data 구조체
struct FSkyDivingData
{
	float AltitudeZeroZ{};				// SkyDiving 상태에서 고도를 측정할 때 고도 0m 기준으로 잡을 위치
	float ParachuteDeployLimitHeight{};	// F키 interaction 없이 낙하산을 무조건 펼쳐야 할 고도 Limit
	float MaxSkyDiveJumpAltitude{};		// SkyDive Jump 최대값 (SkyDive 상태일 때, 고도 측정용 LineTrace 길이에 쓰임)
};


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_SkyDivingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_SkyDivingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	ESkyDivingState GetSkyDivingState() const { return SkyDivingState; }

	/// <summary>
	/// 현 SKyDivingState setting
	/// </summary>
	/// <param name="InSkyDivingState"></param>
	/// <returns> : 원하는 값으로 setting되지 않았다면 return false </returns>
	bool SetSkyDivingState(ESkyDivingState InSkyDivingState);

	float GetParachuteDeployLimitHeight() const { return SkyDivingDataMap[GameInstance->GetCurrentSelectedLevelType()].ParachuteDeployLimitHeight; }

	/// <summary>
	/// 현재 Character Landing Montage가 재생 중인지
	/// </summary>
	bool IsCharacterLandingMontagePlaying() const { return OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(LandingMontage.AnimMontage); }

protected: // SetSkyDivingState 내부의 Primitive Operations
	
	virtual void SetStateToSkyDivingState() PURE_VIRTUAL( UC_SkyDivingComponent::SetStateToSkyDivingState; );

	virtual void SetStateToParachutingState() PURE_VIRTUAL( UC_SkyDivingComponent::SetStateToParachutingState; );

	virtual void SetStateToLandingState() PURE_VIRTUAL( UC_SkyDivingComponent::SetStateToLandingState; );

public:

	/// <summary>
	/// Parachuting 상태일 때 물 위로 착륙했을 때 호출될 예외처리 함수
	/// </summary>
	virtual void OnCharacterLandedOnWater();

public:

	/// <summary>
	/// Deploy Parachute 몽타주 재생 끝날 때 AnimNotify callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnDeployParachuteMontageEnd();

	/// <summary>
	/// LandingMontage AnimNotify callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnLandingMontageEnd();

	/// <summary>
	/// Parachute Landing End Animotify callback 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void OnParachuteLandingMontageEnd();

private:


	/// <summary>
	/// 높이에 따른 State 전환 담당 (ex -> 일정 높이 이하면 낙하산 자동으로 펼치기)
	/// </summary>
	void HandleStateTransitionByHeight();

protected:
	
	/// <summary>
	/// 현재 높이(고도) Update
	/// </summary>
	void UpdateCurrentHeight();

	

protected:

	class AC_BasicCharacter* OwnerCharacter{};

	ESkyDivingState SkyDivingState = ESkyDivingState::READY;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UStaticMeshComponent* ParachuteBackpackStaticMeshComponent{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMeshComponent* ParachuteSkeletalMeshComponent{};

	static const FName PARABACKPACK_SOCKET_NAME;
	static const FName PARACHUTE_SOCKET_NAME;



protected: // Character AnimMontages

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage DeployParachuteMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage LandingMontage{};

protected: // Parachute Skeletal Mesh AnimMontages

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UAnimMontage* ParachuteDeployMontage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UAnimMontage* ParachuteEjectMontage{};

protected:

	// 원작 수치
	//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
	//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

	//const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

	static const TMap<ELevelType, FSkyDivingData> SkyDivingDataMap;

	class UC_GameInstance* GameInstance{};

protected:
	
	// 현재 낙하 높이(고도)
	float CurrentHeight{};

private:

	FCollisionQueryParams ParachutingHeightCheckCollisionParams{};

};


