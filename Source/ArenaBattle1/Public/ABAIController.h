// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "AIController.h"
#include "ABAIController.generated.h"

// �׺� �޽� ���� ���� -> AIController�� ������ ������ �������� �˷��� -> ���� ���������� �����̵��� ��� �߰� 
// AiController�� Ÿ�̸Ӹ� ��ġ�� 3�ʸ��� ������ �������� �̵��϶�� ���

UCLASS()
class ARENABATTLE1_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();
	virtual void OnPossess(APawn* InPawn) override;
	//virtual void OnUnPossess() override;

	// �����忡 ������ ������ ���� AI ��Ʈ�ѷ����� �����ϵ��� ��� ������ �߰�
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
