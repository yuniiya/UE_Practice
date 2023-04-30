// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "NavigationSystem.h"
//#include "Blueprint/AIBlueprintHelperLibrary.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController()
{
	// �� ���� ���� ��������
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	// �����̺�� Ʈ�� ���� ��������
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AABAIController::RunAI()
{
	UBlackboardComponent* BlackboardComp = Blackboard.Get();
	if (true == UseBlackboard(BBAsset, BlackboardComp))
	{
		// �������� HomePosKey�� �r�� InPawn->GetActorLocation()�� ����
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());
		if (false == RunBehaviorTree(BTAsset))
		{
			ABLOG(Error, TEXT("AIController couldn't run behavior tree"));
		}
	}
}

void AABAIController::StopAI()
{
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr != BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

//void AABAIController::OnUnPossess()
//{
//	Super::OnUnPossess();
//	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
//}
//
//void AABAIController::OnRepeatTimer()
//{
//	auto CurrentPawn = GetPawn();
//	ABCHECK(nullptr != CurrentPawn);
//
//	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
//	if (nullptr != NavSystem)
//	{
//		// GetRandomPointInNavigableRadius : �̵� ������ �������� �������� �������� �Լ�.
//		// SimpleMoveToLocation: �������� ���� �̵���Ű�� �Լ�.
//		FNavLocation NextLocation;
//		if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, NextLocation))
//		{
//			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
//			ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
//		}
//	}
//}
