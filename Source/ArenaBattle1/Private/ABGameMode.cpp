// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	// 사용자가 제작한 폰의 클래스 정보를 게임모드의 DefaultPawn으로 지정 
	DefaultPawnClass = AABCharacter::StaticClass();

	// 사용자가 제작한 플레이어 컨트롤러를 디폴트로 지정
	PlayerControllerClass = AABPlayerController::StaticClass();

	// 플레이어 컨트롤러가 초기화될 때, ABPlayerState 클래스의 인스턴스를 생성하고 그 포인터 값을 플레이어 컨트롤러의 PlayerState 속성에 저장
	PlayerStateClass = AABPlayerState::StaticClass();

	// 게임 스테이트 등록
	GameStateClass = AABGameState::StaticClass();

	// 게임 미션 달성 조건
	ScoreToClear = 2;

	// 블루 프린트 에셋의 클래스 정보 가져와서 폰의 기본 에셋으로 지정
	/*static ConstructorHelpers::FClassFinder<APawn> BP_PAWN_C(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));

	if (BP_PAWN_C.Succeeded())
	{
		DefaultPawnClass = BP_PAWN_C.Class;
	}*/
}

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// 언리얼 앤진의 GameState를 멤버 변수로 저장
	ABGameState = Cast<AABGameState>(GameState);
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("Post Login Begin"));
	Super::PostLogin(NewPlayer);
	ABLOG(Warning, TEXT("Post Login End"));

	// PostLogin -> 플레이어 컨트롤러의 구성을 완료하는 시점. 따라서 이 때 PlayerState의 초기화도 완료하기.
	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData();
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
{
	// GetPlayerControllerIterator() : 현재 게임에 참여 중인 플레이어 컨트롤러의 목록 얻어오기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// 플레이어 컨트롤러의 포인터 가져오기
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		// 피격을 가한 플레이어 컨트롤러라면(인자로 받아온 플레이어 컨트롤러)
		if (nullptr != ABPlayerController && ScoredPlayer == ABPlayerController)
		{
			ABPlayerController->AddGameScore();
			break;
		}
	}
	// 전체 스코어(게임 스테이트의 스코어)
	ABGameState->AddGameScore();

	// 게임 달성 조건을 채울 경우
	if (GetScore() >= ScoreToClear)
	{
		ABGameState->SetGameCleared();

		// 모든 폰의 사운드, 애니메이션 등 여러 행동 Off
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
			if (nullptr != ABPlayerController)
			{
				ABPlayerController->ShowResultUI();
			}
		}
	}
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}
