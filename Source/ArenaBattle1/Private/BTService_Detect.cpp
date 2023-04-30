// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");		// ��� ����
	Interval = 1.f;					// TickNode�Լ� ȣ�� �ֱ�	
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)	
	{ 
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.f;	// �ݰ� 6���� ����

	// TArray : �𸮾� ������ �ڷ� ����. ��� ������ ����.
	TArray<FOverlapResult> OverlapResults;
	// FCollisionQueryParams : �浹 ��ɿ� ���޵Ǵ� ���ڸ� �����ص� ����ü. 
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	// OverlapMultiByChannel() : �ݰ� �� �ٸ� NPC�� �ִ� ��쵵 ������ �ݰ� �� ��� ĳ���͸� �����ϴ� �Լ�. 
	// ������ ��� ĳ���� ������ OverlapResults�� ����. 
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,								// ȸ���� -> �⺻
		ECollisionChannel::ECC_GameTraceChannel12,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	// NPC�� ĳ���͸� ������ �ڿ��� ����ڰ� �����ϴ� ĳ�������� �����ؾ� ��. 
	// IsPlayerController() : ĳ���͸� �����ϴ� ��Ʈ�ѷ��� �÷��̾� ��Ʈ�ѷ����� �ľ��� �� �ֵ��� �ϴ� �Լ�
	if (true == bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			// �浹�� ĳ����
			AABCharacter* ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor());
			if (nullptr != ABCharacter)
			{
				// �浹�� ĳ������ ��Ʈ�ѷ��� �÷��̾� ��Ʈ�ѷ����, �ش� ĳ���� �������� Target ������Ʈ ���� �÷��̾� ĳ���ͷ� ����
				if (true == ABCharacter->GetController()->IsPlayerController())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);

					return;
				}
				else
				{
					// �÷��̾� ��Ʈ�ѷ��� �ƴ϶��, Target ���� nullptr
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
				}
			}
		}
	}
}
