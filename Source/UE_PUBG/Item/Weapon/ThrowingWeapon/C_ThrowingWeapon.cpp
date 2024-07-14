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
	// TODO : ��ô���� �ɱ����� �̾Ұ� ��ŷ�� ������ �� �ٽ� �������
	// TODO : ��ô���� ���������̱��� �̾Ҵٸ� ���� ��ġ�� ���� ��ô�� �׳� �ٴڿ� ����

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
	// TODO : ���������� ������
	// ���⼭ ������ ����ź�� ���� ���� Destroy self
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();
	SetActorRotation(OwnerCharacter->GetActorRotation());

	// start - ���� ���� ī�޶� ��ġ
	FVector Start	= UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	FVector Forward = FRotationMatrix(OwnerCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	Direction = Forward;

	Direction.Normalize();

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = Direction * Speed;
	ProjectileMovement->Activate();
	//ProjectileMovement->bSimulationEnabled = true; // Update�� Ȱ��ȭ �ϴ��� ���ϴ���
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	bIsOnThrowProcess = false;

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// ���� ��ô�� ���� ���� �ٷ� �ϱ�
	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// TODO : ���濡 ��ô�� �ִ��� Ȯ���ؼ� �ٷ� ���� ���� �����ϱ�

	/*
	���� �ִ� ��ô���� ���ٸ� �ֹ���1, �ֹ���2 ������ ���� �õ�, ���ٸ� UnArmed�� ���·� ���ư���
	*/

	bool Changed = OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN);
	if (Changed) return;

	Changed = OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN);
	if (Changed) return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);
}
