// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_AttachableItem.generated.h"



/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_AttachableItem : public AC_Item
{
	GENERATED_BODY()
public:
	AC_AttachableItem();
private:
	EPartsName Name;
	class UCapsuleComponent* CapsuleCollider{};
};
