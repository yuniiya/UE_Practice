// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

// 정찰 중 플레이어가 일정 반경 내에 있으면 이를 감지해 추격하는 기능을 담당하는 클래스.
// 언리얼 엔진이 제공하는 서비스 노드를 사용 (서비스 노드: 독립적으로 동작하지 않고, 컴포짓 노드에 부착되는 노드)
// 서비스 노드는 해당 컴포짓에 속한 테스크들이 실행되는 동안 반복적인 작업을 실행하는데 적합.
// 플레이어를 감지하는 서비스 노드를 새로 생성하고, 이를 셀렉터 컴포짓에 추가하면 비헤이비어 트리는 플레이어를 감지하는 루틴을 계속 반복함. 
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();

protected:
	// 서비스 노드는 자신이 속한 컴포짓 노드가 활성화될 경우 주기적으로 TickNode 함수를 호출.
	// 여기서 TickNode는 범위 내 캐릭터가 있는지 감지하는 기능을 담당.
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

// 1. 루트: 비헤이비어의 시작을 나타내는 노드. 한 개의 노드만 연결할 수 있음.
// 2. 컴포짓
//  - 분기가 실행되는 규칙을 정의하는 노드. 
//  - 데코레이터 노드를 추가해 블랙보드 값을 기반으로 특정 컴포짓의 실행 여부를 결정하거나, 서비스 노드를 추가해 해당 컴포짓에 속하는 테스크들이 실행되는 동안 반복적인 작업을 실행.
//  - 셀렉터, 시퀀스, 심플 페러렐 세 종류로 구분.
// 3. 테스크: AI의 행동이나 블랙보드의 값을 조정. 독립적으로 실행될 수 없고 반드시 컴포짓 노드를 거쳐야 함.
// 4. 데코레이터: 특정 컴포짓의 실행 여부를 결정하는 노드.
// 5. 서비스: 컴포짓 노드에 부착되어 실행되는 노드. 해당 컴포짓에 속한 테스크들이 실행되는 동안 반복적인 작업을 수행하는데 적합. 
