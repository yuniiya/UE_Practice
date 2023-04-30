// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/Pawn.h"
#include "GameFrameWork/FloatingPawnMovement.h"
#include "ABPawn.generated.h"

UCLASS()
class ARENABATTLE1_API AABPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AABPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 입력한 키와 실제 기능을 바인딩해주는 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	// 해당 폰에 플레이어 컨트롤러가 빙의하는 시점에 호출되는 함수
	virtual void PossessedBy(AController* NewController) override;

	// 충돌 컴포넌트 -> 움직임 담당. 보통 충돌 영역은 메시가 들어갈 정도의 크기로 설정
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UCapsuleComponent* Capsule;
			
	// 스켈레탈 메쉬  -> 에셋 렌더 및 애니메이션 담당
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	USkeletalMeshComponent* Mesh;

	// 입력에 따른 캐릭터의 움직임 설정
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UFloatingPawnMovement* Movement;

	// 3인칭 시점으로 카메라 구도를 설정할 수 있는 부가 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Camera");
	USpringArmComponent* SpringArm;

	// 카메라 컴포넌트 -> 폰에 카메라를 부착하면 카메라가 바라보는 화면을 플레이어 화면으로 전송
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisvalue);

};
