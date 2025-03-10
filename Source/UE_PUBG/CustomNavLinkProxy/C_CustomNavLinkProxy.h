// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "C_CustomNavLinkProxy.generated.h"

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



UCLASS()
class UE_PUBG_API AC_CustomNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	AC_CustomNavLinkProxy();

	void BeginPlay() override;

protected:
	
	/// <summary>
	/// SmartLink에 도달했을 때 받을 call back 함수 
	/// </summary>
	/// <param name="Agent"> : 도달한 Agent </param>
	/// <param name="Destination"> : 도달할 지점 </param>
	UFUNCTION(BlueprintCallable)
	void OnReceiveSmartLinkReached(AActor* Agent, const FVector& Destination);

private:
	/// <summary>
	/// Jump 시도
	/// </summary>
	/// <param name="Enemy"> : Jump를 시도하는 Enemy </param>
	/// <param name="Destination"> : NavLinkProxy Destination </param>
	/// <returns> : Jump를 성공하였다면 return true </returns>
	bool ExecuteJump(class AC_Enemy* Enemy, const FVector& Destination);

	/// <summary>
	/// Parkour 시도
	/// </summary>
	/// <param name="Enemy"> : Parkour를 시도하는 Enemy </param>
	/// <param name="Destination"> : NavLinkProxy Destination </param>
	/// <returns> : Parkour를 성공하였다면 return true </returns>
	bool ExecuteParkour(class AC_Enemy* Enemy, const FVector& Destination);

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EDirection, ELinkActionStrategy> DirectionActionStrategies =
	{
		{EDirection::LEFT_TO_RIGHT,	ELinkActionStrategy::DEFAULT},
		{EDirection::RIGHT_TO_LEFT,	ELinkActionStrategy::DEFAULT}
	};
};
