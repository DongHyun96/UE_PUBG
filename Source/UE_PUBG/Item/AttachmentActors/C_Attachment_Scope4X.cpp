// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AttachmentActors/C_Attachment_Scope4X.h"
#include "Item/Weapon/Gun/C_Gun.h"

AC_Attachment_Scope4X::AC_Attachment_Scope4X()
{
	PrimaryActorTick.bCanEverTick = true;
	PartName       = EPartsName::SCOPE;
	AttachmentName = EAttachmentNames::SCOPE4;
}

void AC_Attachment_Scope4X::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Attachment_Scope4X::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Attachment_Scope4X::UseStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[AttachmentName]);
	CurrentGun->SetScopeCameraMode(AttachmentName);
	return true;
}
