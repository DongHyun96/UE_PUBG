// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/C_BasicCharacter.h"
#include "C_SwimmingComponent.h"

#include "C_PoseColliderHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_PoseColliderHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_PoseColliderHandlerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter);

	void SetColliderByPoseState(EPoseState InPoseState);

	/// <summary>
	/// Swimming ���̰� �����̴� ������ �ƴ����� ���� Collider ������Ű��
	/// </summary>
	/// <param name="IsMoving"> : true if currently moving on the water </param>
	void SetColliderBySwimmingMovingState(const bool& IsMoving);

	float GetCurrentCrawlSlopeAngleForRigControl() const { return CrawlSlopeAngle; }

public:

	/// <summary>
	/// ���� �ֺ�ȯ�濡�� �÷��̾� ĳ���Ͱ� �ڼ��� �ٲ� �� �ִ��� üũ
	/// </summary>
	/// <param name="InChangeTo"> : �ٲٷ��� �ϴ� �ڼ� </param>
	/// <returns> : �ٲ� �� �ִٸ� return true </returns>
	bool CanChangePoseOnCurrentSurroundEnvironment(EPoseState InChangeTo);

private:

	/// <summary>
	/// �ڼ� ��ȯ�� ���� Body Collider size �� Mesh ��ġ Lerp�� ���߱�
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleLerpBodySizeByPose(const float& DeltaTime);

	/// <summary>
	/// Crawl ���� ��, Crawl Collider ���������� ���� ȸ�� �����ֱ�
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleCrawlColliderRotation(const float& DeltaTime);

private:

	// TODO : Parameter�� Start�� Dest Location ��������

	/// <summary>
	/// ĳ���� ���� ��ġ�� ��絵 ���ϱ� (Radian��)
	/// </summary>
	/// <param name="ImpactDistances"> : Head Pelvis �� LineTrace Impact �Ÿ� </param>
	 /// <param name="HeightOffset"> : LineTrace Start Height(Z) Offset </param>
	/// <param name="EnableDebugLine"> : DebugLine�� �׸��� </param>
	/// <returns> : ��絵 </returns>
	float GetCrawlSlopeAngle(OUT TPair<float, float>& ImpactDistances, const float& HeightOffset = 0.f, const bool& EnableDebugLine = false);


	/// <summary>
	/// ������ HeadLocation�� PelvisLocation�� �־��� ��, �ش� ��ġ�� ��絵 ���ϱ� (Radian��)
	/// </summary>
	/// <param name="HeadLocation"> : ������ Head Location LineTrace ������ </param>
	/// <param name="PelvisLocation"> : ������ Pelvis Location LineTrace ������ </param>
	/// <param name="ImpactDistances"> : Head Pelvis �� LineTrace Impact �Ÿ� </param>
	/// <param name="EnableDebugLine"> : DebugLine�� �׸��� </param>
	/// <returns> : ��絵 </returns>
	float GetCrawlSlopeAngle(const FVector& HeadStartLocation, const FVector& PelvisStartLocation, OUT TPair<float, float>& ImpactDistances, const bool& EnableDebugLine = false);

	/// <summary>
	/// ĳ���� ���� ��ġ�� ��絵 ���ϱ�
	/// </summary>
	/// <param name="ImpactDistances"> : Head Pelvis �� LineTrace Impact �Ÿ� </param>
	 /// <param name="HeightOffset"> : LineTrace Start Height(Z) Offset </param>
	/// <param name="EnableDebugLine"> : DebugLine�� �׸��� </param>
	/// <returns> : ��絵 </returns>
	float GetCrawlSlopeDegree(OUT TPair<float, float>& ImpactDistances, const float& HeightOffset = 0.f, const bool& EnableDebugLine = false);

//public:

	/// <summary>
	/// ������ HeadLocation�� PelvisLocation�� �־��� ��, �ش� ��ġ�� ��絵 ���ϱ�
	/// </summary>
	/// <param name="HeadLocation"> : ������ Head Location LineTrace ������ </param>
	/// <param name="PelvisLocation"> : ������ Pelvis Location LineTrace ������ </param>
	/// <param name="ImpactDistances"> : Head Pelvis �� LineTrace Impact �Ÿ� </param>
	/// <param name="EnableDebugLine"> : DebugLine�� �׸��� </param>
	/// <returns> : ��絵 </returns>
	float GetCrawlSlopeDegree(const FVector& HeadStartLocation, const FVector& PelvisStartLocation, OUT TPair<float, float>& ImpactDistances, const bool& EnableDebugLine = false);

public:

	/// <summary>
	/// �ش� ��ġ���� ĳ���Ͱ� ���帱 �� �ִ��� üũ
	/// </summary>
	/// <param name="HeadStartLocation"> : Line Trace Head ��ġ ������ </param>
	/// <param name="PelvisStartLocation"> : Line Trace Pelvis ��ġ ������ </param>
	/// <returns> : ���帱 �� �ִٸ� return true </returns>
	bool CanCrawlOnSlope(const FVector& HeadStartLocation, const FVector& PelvisStartLocation);


	/// <summary>
	/// �ش� ��ġ���� ĳ���Ͱ� ���帱 �� �ִ��� üũ
	/// </summary>
	/// <param name="SlopeDegree"></param>
	/// <param name="ImpactDistances"></param>
	/// <returns></returns>
	bool CanCrawlOnSlope(const float& SlopeDegree, TPair<float, float>& ImpactDistances);


private:
	
	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCapsuleComponent* CrawlCapsuleComponent{};

private:

	// �ڼ��� CapsuleComponent(RootComponent) Collider ���̿� Radius
	static const TMap<EPoseState, TPair<float, float>> POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS;

	// �ڼ� �� Mesh Z Pos
	static const TMap<EPoseState, float> POSE_BY_MESH_Z_POS;

	// �ڼ��� ũ�� �� ��ġ Lerp Destination
	TPair<float, float> RootColliderHeightRadiusLerpDest = POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::STAND];
	float MeshZPosLerpDest = POSE_BY_MESH_Z_POS[EPoseState::STAND];
	float PoseBySizePosLerpSpeed = 10.f;
	
	// Lerp�� �����ϴ��� üũ
	bool PoseBySizeLerpFlag{};

private: // ChangePose Sweep testing constants

	static const float SWEEP_SPHERE_RAD;
	static const float CROUCH_TO_STAND_SWEEP_DIST;
	static const float CRAWL_TO_STAND_SWEEP_DIST;
	static const float CRAWL_TO_CROUCH_SWEEP_DIST;
	
	static const float CRAWL_LINETRACE_TEST_DIST;
	static const float CRAWL_DEGREE_LIMIT;			// �� �� ���� ��絵 Limit
	static const float CRAWL_GROUND_DIST_LIMIT;

private: // Crawl Collider Rotation Lerp ����

	float CrawlColliderPitchLerpDest{};

private:

	float CrawlSlopeAngle{};

};
