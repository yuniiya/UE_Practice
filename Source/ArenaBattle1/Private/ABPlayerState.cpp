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

	// ���� ����ġ / ���� ������ �ʿ��� ����ġ (MaxExp)
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

	// ���� ������ �ʿ��� ����ġ�� �����ߴٸ� ������ ��, ����ġ ����
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelup = true;
	}

	// HUD�� ��ȣ�� ���� UI�� ������Ʈ
	OnPlayerStateChanged.Broadcast();
	// �÷��̾� ���� �����Ͱ� ����� ������ ������Ʈ
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

	// �÷��̾� ���°� �ٲ���ٸ�, HUD�� ��ȣ�� ���� UI�� ������Ʈ
	OnPlayerStateChanged.Broadcast();
	SavePlayerData();
}

void AABPlayerState::InitPlayerData()
{
	// ���� �̸��� ���� ���̺� ���� ��������
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	// ������ ó�� �����ϸ� ���̺�� ���� �����Ͱ� �����Ƿ�, �÷��̾� ������Ʈ���� �⺻ ���̺� �����͸� ����
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

// �÷��̾� ���� �����Ͱ� ����� ������ �̸� ����
void AABPlayerState::SavePlayerData()
{
	// NewObject<>(): ���ο� �𸮾� ������Ʈ�� �����ϴ� �Լ�. �� �Լ��� ������ ������Ʈ�� �� �̻� ������� ������ ������ �÷��Ͱ� Ž���ؼ� �̸� �ڵ����� �Ҹ��Ŵ.
	// SpawnActor�� ���� ���忡 ���͸� �����ϴ� �Լ��� ���ο����� NewObject�� ���.
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;
	// Select �������� �� ������ �ε��� ����
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

	// �ش� ������ �÷��̾� ���� ���� ��������
	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
