// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");		// 노드 생성
	Interval = 1.f;					// TickNode함수 호출 주기	
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
	float DetectRadius = 600.f;	// 반경 6미터 감지

	// TArray : 언리얼 엔진의 자료 구조. 목록 관리에 적합.
	TArray<FOverlapResult> OverlapResults;
	// FCollisionQueryParams : 충돌 기능에 전달되는 인자를 정의해둔 구조체. 
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	// OverlapMultiByChannel() : 반경 내 다른 NPC가 있는 경우도 가정해 반경 내 모든 캐릭터를 감지하는 함수. 
	// 감지된 모든 캐릭터 정보는 OverlapResults로 전달. 
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,								// 회전값 -> 기본
		ECollisionChannel::ECC_GameTraceChannel12,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	// NPC가 캐릭터를 감지한 뒤에는 사용자가 조종하는 캐릭터인지 구분해야 함. 
	// IsPlayerController() : 캐릭터를 조종하는 컨트롤러가 플레이어 컨트롤러인지 파악할 수 있도록 하는 함수
	if (true == bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			// 충돌한 캐릭터
			AABCharacter* ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor());
			if (nullptr != ABCharacter)
			{
				// 충돌한 캐릭터의 컨트롤러가 플레이어 컨트롤러라면, 해당 캐릭터 블랙보드의 Target 오브젝트 값을 플레이어 캐릭터로 지정
				if (true == ABCharacter->GetController()->IsPlayerController())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);

					return;
				}
				else
				{
					// 플레이어 컨트롤러가 아니라면, Target 값은 nullptr
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
				}
			}
		}
	}
}
