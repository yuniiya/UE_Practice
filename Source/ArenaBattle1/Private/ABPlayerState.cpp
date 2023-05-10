// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"

AABPlayerState::AABPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	GameHighScore = 0;
	Exp = 0;
	CharacterIndex = 0;
	SaveSlotName = TEXT("Player1");
}

int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 AABPlayerState::GetCharacterIndex() const
{
	return CharacterIndex;
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	// 현재 경험치 / 다음 레벨에 필요한 경험치 (MaxExp)
	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
	{
		return false;
	}
	
	bool DidLevelup = false;
	Exp = Exp + IncomeExp;

	// 다음 레벨에 필요한 경험치를 충족했다면 레벨업 후, 경험치 차감
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelup = true;
	}

	// HUD에 신호를 보내 UI를 업데이트
	OnPlayerStateChanged.Broadcast();
	// 플레이어 관련 데이터가 변경될 때마다 업데이트
	SavePlayerData();
	return DidLevelup;
}

void AABPlayerState::AddGameScore()
{
	GameScore++;
	if (GameScore >= GameHighScore)
	{
		GameHighScore = GameScore;
	}

	// 플레이어 상태가 바뀌었다면, HUD에 신호를 보내 UI를 업데이트
	OnPlayerStateChanged.Broadcast();
	SavePlayerData();
}

void AABPlayerState::InitPlayerData()
{
	// 슬롯 이름을 통해 세이브 파일 가져오기
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	// 게임을 처음 시작하면 세이브된 게임 데이터가 없으므로, 플레이어 스테이트에서 기본 세이브 데이터를 생성
	if (nullptr == ABSaveGame)
	{
		ABSaveGame = GetMutableDefault<UABSaveGame>();
	}

	SetPlayerName(ABSaveGame->PlayerName);
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	CharacterIndex = ABSaveGame->CharacterIndex;

	SavePlayerData();
}

// 플레이어 관련 데이터가 변경될 때마다 이를 저장
void AABPlayerState::SavePlayerData()
{
	// NewObject<>(): 새로운 언리얼 오브젝트를 생성하는 함수. 이 함수로 생성된 오브젝트를 더 이상 사용하지 않으면 가비지 컬렉터가 탐지해서 이를 자동으로 소멸시킴.
	// SpawnActor와 같이 월드에 엑터를 생성하는 함수도 내부에서는 NewObject를 사용.
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;
	// Select 레벨에서 고른 에셋의 인덱스 저장
	NewPlayerData->CharacterIndex = CharacterIndex;

	if (false == UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
	{
		ABLOG(Error, TEXT("SaveGame Error."))
	}
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	// GameInstance -> CharacterData -> Level
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	// 해당 레벨의 플레이어 스탯 정보 가져오기
	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
