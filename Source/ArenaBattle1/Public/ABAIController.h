// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "AIController.h"
#include "ABAIController.generated.h"

// 네비 메시 영역 설정 -> AIController에 빙의한 폰에게 목적지를 알려줌 -> 폰이 목적지까지 움직이도록 명령 추가 
// AiController에 타이머를 설치해 3초마다 폰에게 목적지로 이동하라고 명령

UCLASS()
class ARENABATTLE1_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();
	virtual void OnPossess(APawn* InPawn) override;
	//virtual void OnUnPossess() override;

	// 블랙보드에 선언한 변수의 값을 AI 컨트롤러에서 지정하도록 멤버 변수로 추가
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;


//private:
//	void OnRepeatTimer();
//
//private:
//	FTimerHandle RepeatTimerHandle;
//	float RepeatInterval;
};
