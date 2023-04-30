// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

// NPC가 이동할 위치를 위해서는 PatrolPos 데이터를 생성해야 하는데, 이는 순찰할 때마다 바뀌므로 Task를 만들어 비헤이비어 트리에서 블랙보드에 값을 쓰도록 설계해야 함.
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
