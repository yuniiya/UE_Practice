// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE1_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AABGameMode();
	
public:
	// 로그인 완료 시(플레이어가 게임에 입장 시) 호출되는 이벤트 함수
	// 플레이어가 조종할 폰 생성 + 플레이어 컨트롤러가 폰에 빙의하는 작업이 이루어짐.
	virtual void PostLogin(APlayerController* NewPlayer) override;

};
