// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "Components/ActorComponent.h"

#include "C_SkyDivingComponent.generated.h"

UENUM(BlueprintType)
enum class ESkyDivingState : uint8
{
	READY,			// 비행기에 아직 있는 상태
	SKYDIVING,		
	PARACHUTING,
	LANDING,			// 착지 동작 재생
	MAX
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

	static float GetParachuteDeployLimitHeight() { return PARACHUTE_DEPLOY_LIMIT_HEIGHT; }

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

	static const float ALTITUDE_ZERO_Z;

	// 원작 수치
	//const float PARACHUTE_DEPLOY_LIMIT_HEIGHT = 43000.f;
	//const float MAX_SKYDIVE_JUMP_ALTITUDE = 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림

	static const float PARACHUTE_DEPLOY_LIMIT_HEIGHT;
	//const float MAX_SKYDIVE_JUMP_ALTITUDE		= 155000.f; // 원작 기준 8 x 8 맵 1.5km 상공 맥시멈에서 뛰어내림
	static const float MAX_SKYDIVE_JUMP_ALTITUDE;


protected:
	
	// 현재 낙하 높이(고도)
	float CurrentHeight{};

};


