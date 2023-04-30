// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"

AABGameMode::AABGameMode()
{
	// 사용자가 제작한 폰의 클래스 정보를 게임모드의 DefaultPawn으로 지정 
	//DefaultPawnClass = AABPawn::StaticClass();
	DefaultPawnClass = AABCharacter::StaticClass();

	// 사용자가 제작한 플레이어 컨트롤러를 디폴트로 지정
	PlayerControllerClass = AABPlayerController::StaticClass();

	// 블루 프린트 에셋의 클래스 정보 가져와서 폰의 기본 에셋으로 지정
	/*static ConstructorHelpers::FClassFinder<APawn> BP_PAWN_C(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));

	if (BP_PAWN_C.Succeeded())
	{
		DefaultPawnClass = BP_PAWN_C.Class;
	}*/
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("Post Login Begin"));
	Super::PostLogin(NewPlayer);
	ABLOG(Warning, TEXT("Post Login End"));
}
