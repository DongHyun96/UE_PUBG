// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceCombat.h"

UC_BTServiceCombat::UC_BTServiceCombat()
{
	NodeName = "Combat";
}

void UC_BTServiceCombat::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	

}


