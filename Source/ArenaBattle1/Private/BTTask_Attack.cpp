// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

// ���� �½�ũ�� ���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� �½�ũ�̱� ������, ����� InProgress�� �ϴ� ��ȯ�ϰ� ������ ������ �����ٰ� �˷���� ��.
// FinishLatentTask() : �½�ũ ���Ḧ �˸��� �Լ�. �̸� ���� ����� Tick ����� Ȱ��ȭ.

// ĳ���� �ʿ��� ��������Ʈ ���� �Ϸ� -> �׽�ũ���� ���� �Լ��� ĳ���� �� ��������Ʈ�� ��� -> Tick �Լ����� �̸� �ľ� -> FinishLatentTask �Լ� ȣ�� -> �׽�ũ ����
EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// NPC�� �÷��̾� ����
	ABCharacter->Attack();
	IsAttacking = true;
	// ĳ���� �ʿ��� ���� �ִϸ��̼��� �������� �ľ� �Ϸ�Ǹ�, IsAttacking ���¸� false�� ���� -> �׽�ũ ����
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
