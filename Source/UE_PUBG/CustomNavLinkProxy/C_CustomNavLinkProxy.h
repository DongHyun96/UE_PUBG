// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "AITypes.h"
#include "Character/C_AnimBasicCharacter.h"
#include "C_CustomNavLinkProxy.generated.h"

enum class EPoseState : uint8;

namespace EPathFollowingResult
{
	enum Type : int;
}

/// <summary>
/// NavLinkProxy 통로를 거칠 때, 캐릭터가 취할 Action
/// </summary>
UENUM(BlueprintType)
enum class ELinkActionStrategy : uint8
{
	DEFAULT,	
	JUMP,	 
	PARKOUR,
	MAX
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	MAX
};

USTRUCT(BlueprintType)
struct FLinkedStartDestPoseState : public FTableRowBase
{
	GENERATED_BODY()

	FLinkedStartDestPoseState() {}

	FLinkedStartDestPoseState(EPoseState _StartPointPoseState, EPoseState _DestPointPoseState)
		: StartPointPoseState(_StartPointPoseState), DestPointPoseState(_DestPointPoseState) {}

public:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPoseState StartPointPoseState{}; // 출발 지점에서 취해야 할 PoseState
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPoseState DestPointPoseState{};  // 도착 지점에서 취해야 할 PoseState
};

USTRUCT(BlueprintType)
struct FJumpDescriptor : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	// Descriptor에 Setting된 Jump 속도를 쓸 것인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseJumpDescriptorSettings{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EDirection, float> DirectionVelocityZMap =
	{
		{EDirection::LEFT_TO_RIGHT, 420.f},
		{EDirection::RIGHT_TO_LEFT, 420.f},
	};
};


UCLASS()
class UE_PUBG_API AC_CustomNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()
public:
	AC_CustomNavLinkProxy();

protected:
	void BeginPlay() override;
	
private:

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/// <summary>
	/// static Action Strategies 초기화 시도 (이미 초기화 되어있다면 x)
	/// </summary>
	bool TryInitLinkActionStrategies();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/// <summary>
	/// 이 NavLinkProxy에 도달한 Enemy에 대한 PoseState 전환 담당
	/// </summary>
	void HandleEnteredEnemiesDestArrival();

protected:
	
	/// <summary>
	/// SmartLink에 도달했을 때(첫 시작점(Left든 Right든)) 받을 call back 함수
	/// </summary>
	/// <param name="Agent"> : 도달한 Agent </param>
	/// <param name="Destination"> : 도달할 지점 </param>
	UFUNCTION(BlueprintCallable)
	void OnReceiveSmartLinkReached(AActor* Agent, const FVector& Destination);

	UFUNCTION(BlueprintCallable)
	FVector GetLeftPointWorldLocation() const { return GetTransform().TransformPosition(PointLinks[0].Left); }

	UFUNCTION(BlueprintCallable)
	FVector GetRightPointWorldLocation() const { return GetTransform().TransformPosition(PointLinks[0].Right); }

public:

	bool IsUsingJumpDescriptorSettings() const { return JumpDescriptor.bUseJumpDescriptorSettings; }
	float GetJumpVelocityZ(EDirection Direction) const {return JumpDescriptor.DirectionVelocityZMap[Direction];}
	
protected:

	// 각 방향에 적용할 Strategy 종류 지정 (Default일 경우, 기본 NavLinkProxy기능을 따름)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EDirection, ELinkActionStrategy> DirectionActionStrategies =
	{
		{EDirection::LEFT_TO_RIGHT,	ELinkActionStrategy::DEFAULT},
		{EDirection::RIGHT_TO_LEFT,	ELinkActionStrategy::DEFAULT}
	};

protected:

	// 각 방향으로 향할 때 출발 지점에서 취할 자세와 도착 지점에서 취해야 하는 자세 지정
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EDirection, FLinkedStartDestPoseState> DirectionPoseStates =
	{
		{EDirection::LEFT_TO_RIGHT, {EPoseState::STAND, EPoseState::STAND}},
		{EDirection::RIGHT_TO_LEFT, {EPoseState::STAND, EPoseState::STAND}},
	};

private:
	// 생성된 총 CustomNavLinkProxy 개수 / NavLinkProxyActionStrategy 해제 시 사용 예정
	static int CustomNavLinkProxyCount;
	static TMap<ELinkActionStrategy, class II_NavLinkProxyActionStrategy*> LinkActionStrategies;

private:

	// 도착 지점 인원 확인 Map (Enemy & CurDirection)
	// TMap<class AC_Enemy*, EDirection> LeftSideDestEnemies{};
	// TMap<class AC_Enemy*, EDirection> RightSideDestEnemies{};

	// 이 링크의 시작점에 도달한 Enemy들	
	TMap<EDirection, TSet<class AC_Enemy*>> LinkEnteredEnemies =
	{
		{EDirection::LEFT_TO_RIGHT, {}},
		{EDirection::RIGHT_TO_LEFT, {}}
	};

	TMap<EDirection, FVector2D> DestinationXY =
	{
		{EDirection::LEFT_TO_RIGHT, {}},
		{EDirection::RIGHT_TO_LEFT, {}}
	};

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FJumpDescriptor JumpDescriptor{};

protected: // LeftPoint와 RightPoint를 근처의 NavLinkProxy위의 점으로 조정할지

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAdjustLeftPointToNearestNavLinkProxyPoint = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAdjustRightPointToNearestNavLinkProxyPoint = true;
	
};
