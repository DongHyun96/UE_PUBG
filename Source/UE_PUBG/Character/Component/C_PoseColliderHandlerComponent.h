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
	/// Swimming 중이고 움직이는 중인지 아닌지에 따른 Collider 조정시키기
	/// </summary>
	/// <param name="IsMoving"> : true if currently moving on the water </param>
	void SetColliderBySwimmingMovingState(const bool& IsMoving);

	float GetCurrentCrawlSlopeAngleForRigControl() const { return CrawlSlopeAngle; }

public:

	/// <summary>
	/// 현재 주변환경에서 플레이어 캐릭터가 자세를 바꿀 수 있는지 체크
	/// </summary>
	/// <param name="InChangeTo"> : 바꾸려고 하는 자세 </param>
	/// <returns> : 바꿀 수 있다면 return true </returns>
	bool CanChangePoseOnCurrentSurroundEnvironment(EPoseState InChangeTo);

private:

	/// <summary>
	/// 자세 전환에 따른 Body Collider size 및 Mesh 위치 Lerp로 맞추기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleLerpBodySizeByPose(const float& DeltaTime);

	/// <summary>
	/// Crawl 상태 시, Crawl Collider 지형지물에 따라 회전 시켜주기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleCrawlColliderRotation(const float& DeltaTime);

private:

	// TODO : Parameter로 Start와 Dest Location 가져오기

	/// <summary>
	/// 캐릭터 현재 위치의 경사도 구하기 (Radian값)
	/// </summary>
	/// <param name="ImpactDistances"> : Head Pelvis 순 LineTrace Impact 거리 </param>
	 /// <param name="HeightOffset"> : LineTrace Start Height(Z) Offset </param>
	/// <param name="EnableDebugLine"> : DebugLine을 그릴지 </param>
	/// <returns> : 경사도 </returns>
	float GetCrawlSlopeAngle(OUT TPair<float, float>& ImpactDistances, const float& HeightOffset = 0.f, const bool& EnableDebugLine = false);


	/// <summary>
	/// 임의의 HeadLocation과 PelvisLocation이 주어질 때, 해당 위치의 경사도 구하기 (Radian값)
	/// </summary>
	/// <param name="HeadLocation"> : 임의의 Head Location LineTrace 시작점 </param>
	/// <param name="PelvisLocation"> : 임의의 Pelvis Location LineTrace 시작점 </param>
	/// <param name="ImpactDistances"> : Head Pelvis 순 LineTrace Impact 거리 </param>
	/// <param name="EnableDebugLine"> : DebugLine을 그릴지 </param>
	/// <returns> : 경사도 </returns>
	float GetCrawlSlopeAngle(const FVector& HeadStartLocation, const FVector& PelvisStartLocation, OUT TPair<float, float>& ImpactDistances, const bool& EnableDebugLine = false);

	/// <summary>
	/// 캐릭터 현재 위치의 경사도 구하기
	/// </summary>
	/// <param name="ImpactDistances"> : Head Pelvis 순 LineTrace Impact 거리 </param>
	 /// <param name="HeightOffset"> : LineTrace Start Height(Z) Offset </param>
	/// <param name="EnableDebugLine"> : DebugLine을 그릴지 </param>
	/// <returns> : 경사도 </returns>
	float GetCrawlSlopeDegree(OUT TPair<float, float>& ImpactDistances, const float& HeightOffset = 0.f, const bool& EnableDebugLine = false);

//public:

	/// <summary>
	/// 임의의 HeadLocation과 PelvisLocation이 주어질 때, 해당 위치의 경사도 구하기
	/// </summary>
	/// <param name="HeadLocation"> : 임의의 Head Location LineTrace 시작점 </param>
	/// <param name="PelvisLocation"> : 임의의 Pelvis Location LineTrace 시작점 </param>
	/// <param name="ImpactDistances"> : Head Pelvis 순 LineTrace Impact 거리 </param>
	/// <param name="EnableDebugLine"> : DebugLine을 그릴지 </param>
	/// <returns> : 경사도 </returns>
	float GetCrawlSlopeDegree(const FVector& HeadStartLocation, const FVector& PelvisStartLocation, OUT TPair<float, float>& ImpactDistances, const bool& EnableDebugLine = false);

public:

	/// <summary>
	/// 해당 위치에서 캐릭터가 엎드릴 수 있는지 체크
	/// </summary>
	/// <param name="HeadStartLocation"> : Line Trace Head 위치 시작점 </param>
	/// <param name="PelvisStartLocation"> : Line Trace Pelvis 위치 시작점 </param>
	/// <returns> : 엎드릴 수 있다면 return true </returns>
	bool CanCrawlOnSlope(const FVector& HeadStartLocation, const FVector& PelvisStartLocation);


	/// <summary>
	/// 해당 위치에서 캐릭터가 엎드릴 수 있는지 체크
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

	static const ECollisionChannel PoseCheckerChannel;

private:

	// 자세별 CapsuleComponent(RootComponent) Collider 높이와 Radius
	static const TMap<EPoseState, TPair<float, float>> POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS;

	// 자세 별 Mesh Z Pos
	static const TMap<EPoseState, float> POSE_BY_MESH_Z_POS;

	// 자세별 크기 및 위치 Lerp Destination
	TPair<float, float> RootColliderHeightRadiusLerpDest = POSE_BY_ROOTCOLLIDER_HEIGHT_RADIUS[EPoseState::STAND];
	float MeshZPosLerpDest = POSE_BY_MESH_Z_POS[EPoseState::STAND];
	float PoseBySizePosLerpSpeed = 10.f;
	
	// Lerp를 진행하는지 체크
	bool PoseBySizeLerpFlag{};

private: // ChangePose Sweep testing constants

	static const float SWEEP_SPHERE_RAD;
	static const float CROUCH_TO_STAND_SWEEP_DIST;
	static const float CRAWL_TO_STAND_SWEEP_DIST;
	static const float CRAWL_TO_CROUCH_SWEEP_DIST;
	
	static const float CRAWL_LINETRACE_TEST_DIST;
	static const float CRAWL_DEGREE_LIMIT;			// 기어갈 수 없는 경사도 Limit
	static const float CRAWL_GROUND_DIST_LIMIT;

private: // Crawl Collider Rotation Lerp 관련

	float CrawlColliderPitchLerpDest{};

private:

	float CrawlSlopeAngle{};

};


