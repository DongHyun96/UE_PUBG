// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Character/Component/C_EquippedComponent.h"

AC_ThrowingWeapon::AC_ThrowingWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_ThrowingWeaponStrategy>("ThrowingWeaponStrategy");

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Collider = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//RootComponent = Collider;
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(OwnerCharacter))
	{
		CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
		CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
		CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];
	}
}

bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	// TODO : 투척류를 핀까지만 뽑았고 쿠킹을 안했을 시 다시 집어넣음
	// TODO : 투척류를 안전손잡이까지 뽑았다면 현재 위치에 현재 투척류 그냥 바닥에 떨굼

	SetActorHiddenInGame(true);

	ProjectileMovement->Deactivate();

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	//this->SetHidden(false);

	SetActorHiddenInGame(false);

	ProjectileMovement->Deactivate();

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);
	
	// Self init
	bIsCharging = false;
	bIsOnThrowProcess = false;

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
}

void AC_ThrowingWeapon::OnThrowReadyLoop()
{
	if (!bIsCharging) OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	// TODO : 실질적으로 던지기
	// 여기서 던져진 수류탄은 터진 이후 Destroy self
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();
	SetActorRotation(OwnerCharacter->GetActorRotation());

	// start - 현재 쓰는 카메라 위치
	FVector Start	= UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	FVector Forward = FRotationMatrix(OwnerCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	Direction = Forward;

	Direction.Normalize();

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = Direction * Speed;
	ProjectileMovement->Activate();
	//ProjectileMovement->bSimulationEnabled = true; // Update를 활성화 하는지 안하는지
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	bIsOnThrowProcess = false;

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// 현재 투척류 장착 해제 바로 하기
	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// TODO : 가방에 투척류 있는지 확인해서 바로 다음 무기 장착하기

	/*
	남아 있는 투척류가 없다면 주무기1, 주무기2 순으로 장착 시도, 없다면 UnArmed인 상태로 돌아가기
	*/

	bool Changed = OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN);
	if (Changed) return;

	Changed = OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN);
	if (Changed) return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);
}
