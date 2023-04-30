// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

// NPC�� �̵��� ��ġ�� ���ؼ��� PatrolPos �����͸� �����ؾ� �ϴµ�, �̴� ������ ������ �ٲ�Ƿ� Task�� ����� �����̺�� Ʈ������ �����忡 ���� ������ �����ؾ� ��.
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
