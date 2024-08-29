// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField/C_MagneticWall.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Utility/C_Util.h"

AC_MagneticWall::AC_MagneticWall()
{
	PrimaryActorTick.bCanEverTick = true;

	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	RootComponent = PlaneMesh;

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneAsset(TEXT("/Game/StarterContent/Shapes/Shape_Plane.Shape_Plane"));
	//if (PlaneAsset.Succeeded()) PlaneMesh->SetStaticMesh(PlaneAsset.Object);

	//PlaneMesh->SetWorldScale3D(FVector(100.f, 1.f, 1.f));
	//PlaneMesh->SetWorldRotation(FRotator(0.f, 90.f, 0.f));

	PlaneMesh->SetRelativeScale3D(FVector(100.f, 1.f, 1.f));
	PlaneMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f)); // y축 회전으로 나옴

	//PlaneMesh->GetMaterial(0)->
}

void AC_MagneticWall::BeginPlay()
{
	Super::BeginPlay();
}

void AC_MagneticWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMaterialVTiling();
}

void AC_MagneticWall::UpdateMaterialVTiling()
{
	if (UMaterialInstanceDynamic* MatInstance = PlaneMesh->CreateAndSetMaterialInstanceDynamic(0))
		MatInstance->SetScalarParameterValue(FName("VTiling"), GetActorScale3D().Y);
}

