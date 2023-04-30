// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TurnToTarget.generated.h"

// NPC�� �����ϸ鼭 ���ÿ� �÷��̾ ���� ȸ���ϴ� ����� ���� �������� Target���� ȸ���ϴ� �׽�ũ.
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UBTTask_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
