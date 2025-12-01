// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AttachmentActors/C_Attachment_Scope4X.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Utility/C_Util.h"

#include "Item/Weapon/Gun/C_Gun.h"

AC_Attachment_Scope4X::AC_Attachment_Scope4X()
{
	PrimaryActorTick.bCanEverTick = false;
	PartName       = EPartsName::SCOPE;
	AttachmentName = EAttachmentNames::SCOPE4;
	//RootComponent = AttachmentMesh;
}

void AC_Attachment_Scope4X::BeginPlay()
{
	Super::BeginPlay();
	SceneCaptureComponent = Cast<USceneCaptureComponent2D>(GetDefaultSubobjectByName(TEXT("ScopeCamera")));
	if (IsValid(SceneCaptureComponent))
	{
		//UC_Util::Print("Found Camera", FColor::Blue, 20);
		SceneCaptureComponent->SetActive(false);
	}
}

void AC_Attachment_Scope4X::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Attachment_Scope4X::UseAttachStrategy()
{
	if (!Super::UseAttachStrategy()) return false;
	
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[AttachmentName]);
	CurrentGun->SetScopeCameraMode(AttachmentName);

	return true;
}

bool AC_Attachment_Scope4X::UseDetachStrategy()
{
	if (!Super::UseDetachStrategy()) return false;
	
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[EAttachmentNames::MAX]);
	CurrentGun->SetScopeCameraMode(EAttachmentNames::MAX);
	CurrentGun->SetIronSightMeshHiddenInGame(false);
	
	DetachRootComponentFromParent();
	
	SetActorHiddenInGame(true);
	
	if (IsValid(SceneCaptureComponent))
		SceneCaptureComponent->SetActive(false);
	
	return true;
}

bool AC_Attachment_Scope4X::UseMrbStrategy()
{
	if (!GetAttachParentActor()) return false;
	
	CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun)) return false;
	if (!IsValid(SceneCaptureComponent)) return false;
	
	SceneCaptureComponent->SetActive(CurrentGun->GetIsAimPress());
	return true;
}


