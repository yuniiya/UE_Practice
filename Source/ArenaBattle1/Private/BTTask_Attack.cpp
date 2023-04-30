// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

// 공격 태스크는 공격 애니메이션이 끝날 때까지 대기해야 하는 지연 태스크이기 때문에, 결과를 InProgress로 일단 반환하고 공격이 끝나면 끝났다고 알려줘야 함.
// FinishLatentTask() : 태스크 종료를 알리는 함수. 이를 위해 노드의 Tick 기능을 활성화.

// 캐릭터 쪽에서 델리게이트 설정 완료 -> 테스크에서 람다 함수를 캐릭터 쪽 델리게이트에 등록 -> Tick 함수에서 이를 파악 -> FinishLatentTask 함수 호출 -> 테스크 종료
EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// NPC가 플레이어 공격
	ABCharacter->Attack();
	IsAttacking = true;
	// 캐릭터 쪽에서 공격 애니메이션이 끝났는지 파악 완료되면, IsAttacking 상태를 false로 변경 -> 테스크 종료
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void
	{
		IsAttacking = false;
	});


	return EBTNodeResult::InProgress;

}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (false == IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
