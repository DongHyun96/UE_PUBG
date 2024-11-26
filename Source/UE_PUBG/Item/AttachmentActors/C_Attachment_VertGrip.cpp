// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/AttachmentActors/C_Attachment_VertGrip.h"
#include "Item/AttachmentActors/C_Attachment_Scope4X.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Utility/C_Util.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Components/StaticMeshComponent.h"


AC_Attachment_VertGrip::AC_Attachment_VertGrip()
{
	PrimaryActorTick.bCanEverTick = true;
	PartName       = EPartsName::GRIP;
	AttachmentName = EAttachmentNames::VERTGRIP;
	AttachmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");

}

void AC_Attachment_VertGrip::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Attachment_VertGrip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Attachment_VertGrip::UseAttachStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;

	CurrentGun->SetRecoilMultiplierGripVert(0.7f);
	return true;
}

bool AC_Attachment_VertGrip::UseDetachStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;
	CurrentGun->SetRecoilMultiplierGripVert(1.0f);

	DetachRootComponentFromParent();
	SetActorHiddenInGame(true);
	return true;
}

bool AC_Attachment_VertGrip::UseMrbStrategy()
{
	return false;
}
