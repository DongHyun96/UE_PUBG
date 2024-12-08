// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Attachment/C_AttachableItem.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UMG.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Components/PanelWidget.h"
#include "Components/NamedSlotInterface.h"
#include "Utility/C_Util.h"

#include "Components/CanvasPanelSlot.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Component/C_CrosshairWidgetComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "Item/Attachment/C_AttachableItem.h"

#include "Item/Weapon/Gun/C_Bullet.h"
AC_AttachableItem::AC_AttachableItem()
{

}

void AC_AttachableItem::BeginPlay()
{
	Super::BeginPlay();
	ItemDatas.ItemName = "Test Item Name";
	AttachableItemMesh = FindComponentByClass<USkeletalMeshComponent>();
	CapsuleCollider = FindComponentByClass<UCapsuleComponent>();
	if (IsValid(AttachableItemMesh))
	{
		AttachableItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UC_Util::Print("Found mesh!!!!!!", FColor::Blue);
	}
}

void AC_AttachableItem::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);
}

void AC_AttachableItem::SetIsAttached(bool InIsAttachedToWeapon)
{
	if (InIsAttachedToWeapon)
	{
		if (IsValid(CapsuleCollider))
			CapsuleCollider->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void AC_AttachableItem::SetMeshVisibility(bool InIsVisible)
{
	AttachableItemMesh->SetHiddenInGame(!InIsVisible);
	//CapsuleCollider->SetVisibility(InIsVisible);
	//AttachableItemMesh->SetVisibility(InIsVisible);
	//UC_Util::Print("jasyhdfbuiayersyhgbfbaueysdhyfbgasodudyfgbasdudlkyfgbaeruoyfgbbaweuoyfgbeqaouryfgvbwaeuikyrf");
}

