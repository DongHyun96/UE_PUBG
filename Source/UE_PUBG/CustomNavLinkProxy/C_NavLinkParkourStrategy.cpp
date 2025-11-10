// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_NavLinkParkourStrategy.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_ParkourComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

bool UC_NavLinkParkourStrategy::ExecuteStartPointAction(AC_CustomNavLinkProxy* CustomNavLinkProxy, AC_Enemy* EnemyAgent,
                                                        const FVector& StartPoint, const EDirection& Direction)
{
	return EnemyAgent->GetParkourComponent()->TryExecuteParkourAction();
}
