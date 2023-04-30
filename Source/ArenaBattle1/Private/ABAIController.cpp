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
	// 블랙 보드 에셋 가져오기
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	// 비헤이비어 트리 에셋 가져오기
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
		// 블랙보드의 HomePosKey의 걊을 InPawn->GetActorLocation()로 지정
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
//		// GetRandomPointInNavigableRadius : 이동 가능한 목적지를 랜덤으로 가져오는 함수.
//		// SimpleMoveToLocation: 목적지로 폰을 이동시키는 함수.
//		FNavLocation NextLocation;
//		if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, NextLocation))
//		{
//			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
//			ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
//		}
//	}
//}
