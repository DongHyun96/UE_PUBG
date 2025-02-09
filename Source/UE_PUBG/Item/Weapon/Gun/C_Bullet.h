// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_Bullet.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Bullet : public AActor
{
	GENERATED_BODY()
public:
	AC_Bullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UInstancedStaticMeshComponent* InstancedBulletMesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//class UStaticMeshComponent* BulletMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UProjectileMovementComponent* BulletProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USphereComponent* SphereCollider;

	UFUNCTION(BlueprintCallable)
	void DeactivateInstance();
	void ActivateInstance();
	bool GetIsActive() { return IsActive; }
	//void SetMeshVisiblility(bool InBool) { BulletMesh->SetVisibility(InBool);}
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector InitialVelocityNormalized{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector LineTraceHitLocation{};
	bool Fire(class AC_Gun* InOwnerGun, FVector InLocation, FVector InDirection, bool EnableGravity = true, FVector InHitLocation = FVector(0));

	void SubSteppingMovementPhysics(float SebStepDeltaTime);

	void CustomPhysics(float DeltaTime);
	float DragCoefficient;

	int TestTickCount = 0;
	float TestTimeCount = 0;

public:
	void SetInstanceNum(int InNum) { InstanceNum = InNum; }
protected:
	float Drag_Coefficient = 0.0165;
	float Air_Density = 1.225;// # kg / m ^ 3
	float Bullet_Mass = 0.004;//  # kg(5.56mm탄의 추정치)
	float Bullet_Diameter = 0.00556;// # m(5.56mm탄의 직경)
	float Cross_Sectional_Area = PI * FMath::Square(Bullet_Diameter / 2);//  # m ^ 2
	float Distance_To_Target = 100;//  # m
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool IsActive;
	FVector FireLocation;
	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	//bool bBulletIsActive() { return BulletProjectileMovement->IsActive(); }

	void CalculateTravelDistanceAndDeactivate(float DeltaTime);

	int InstanceNum;

	float InstanceLifeTime = 0.0f;

	AC_BasicCharacter* OwnerCharacter{};
public:
	virtual void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }
	UFUNCTION(BlueprintCallable)

	AC_BasicCharacter* GetOwnerCharacter() { return OwnerCharacter; }

private:
	
	/// <summary>
	/// <para> ProjectileStop시에 호출될 event (블루프린트에서 구현한 event도 이 함수와 무관하게 event 호출 시 호출됨 </para>
	/// <para> 캐릭터에 총알 피격 데미지 적용 검사 및 적용 구현 </para>
	/// </summary>
	/// <param name="ImpactResult"></param>
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

private:

	// 해당 Bullet을 발사한 총기 
	class AC_Gun* FiredGun{};

};


