// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.h"
#include "ABGameState.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if (UI_MENU_C.Succeeded())
	{
		MenuWidgetClass = UI_MENU_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
	{
		ResultWidgetClass = UI_RESULT_C.Class;
	}

}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* InPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(InPawn);
}

UABHUDWidget* AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const
{
	// �÷��̾� ��Ʈ�ѷ��� ���� �÷��̾� ������Ʈ ���� ������Ʈ
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::AddGameScore() const
{
	// �÷��̾� ��Ʈ�ѷ��� ���� �÷��̾� ������Ʈ ���� ������Ʈ
	ABPlayerState->AddGameScore();
}

void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (true == bGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		// MŰ�� ������ �ý��� UI�� ����� ���
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::ShowResultUI()
{
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// �⺻ �Է� ������ ���� ���
	ChangeInputMode(true);

	// �⺻ ���� ���� �� ȭ�鿡 ����
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	// ��� ���� ����
	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

	// HUD ������ �÷��̾� ������Ʈ ����
	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChanged.Broadcast();
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// ������Ʈ ���ÿ��� ������ Ű�� �÷��̾� ��Ʈ�ѷ����� �ν��ϵ��� �ϴ� �ڵ�
	// �÷��̾� ��Ʈ�ѷ����� ������ ó���ؾ� ������ ���� ������� �Է��� ó���� �� �����Ƿ�, ���� �ý��۰� ���õ� �Է��� ó���� ���� �÷��̾� ��Ʈ�ѷ����� ó��
	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause);
}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	// SetPause() : �÷��̾� ��Ʈ�ѷ��� �Լ���, �÷��̸� �Ͻ� ����
	SetPause(true);
	ChangeInputMode(false);

}
