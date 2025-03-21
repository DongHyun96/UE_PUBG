// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_NavLinkProxyActionStrategy.generated.h"


UINTERFACE(MinimalAPI)
class UI_NavLinkProxyActionStrategy : public UInterface
{
	GENERATED_BODY()
};

/**
 * CustomNavLinkProxy를 이용할 때 사용할 Action Strategy Interface
 * 출발지에서 목적지로 이동 & 목적지로 도달했을 때 Jump 또는 Parkour 또는 자세전환 전략에 대한 Interface
 */
class UE_PUBG_API II_NavLinkProxyActionStrategy
{
	GENERATED_BODY()

	
public:
	/// <summary>
	/// StartPoint에서 실행할 ActionStrategy
	/// </summary>
	/// <param name="CustomNavLinkProxy"> : 이 전략을 사용하는 CustomNavLink 객체 </param>
	/// <param name="EnemyAgent"> : 이 전략을 사용하는 Enemy Character </param>
	/// <param name="StartPoint"> : CustomNavLink에서의 시작 지점(Left나 Right 지점) </param>
	/// <returns> : 실행 성공하였다면 return true </returns>
	virtual bool ExecuteStartPointAction(class AC_CustomNavLinkProxy* CustomNavLinkProxy, class AC_Enemy* EnemyAgent, const FVector& StartPoint) = 0;

};
