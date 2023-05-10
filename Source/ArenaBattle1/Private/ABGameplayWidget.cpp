// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameplayWidget.h"
#include "ABPlayerController.h"
#include "Components/Button.h"

// UI ������ �ʱ�ȭ�ϴ� �������� �߻��ϴ� �Լ�
void UABGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// �̸����� ��ư Ž�� �� ���ε�
	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (nullptr != ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnResumeClicked);
	}

	ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
	if (nullptr != ReturnToTitleButton)
	{
		ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnReturnToTitleClicked);
	}

	RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
	if (nullptr != RetryGameButton)
	{
		RetryGameButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnRetryGameClicked);
	}
}

void UABGameplayWidget::OnResumeClicked()
{
	// GetOwningPlayer() : ���� UI �ڽ��� �����ϰ� �����ϴ� �÷��̾� ��Ʈ�ѷ��� ������ �������� �Լ�.
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != ABPlayerController);

	// RemoveFromParent() : UI �ý����� ���� ����Ʈ�� ����� �ڽ��� �����ϴ� �Լ�. 
	RemoveFromParent();
	ABPlayerController->ChangeInputMode(true);
	ABPlayerController->SetPause(false);
}

void UABGameplayWidget::OnReturnToTitleClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void UABGameplayWidget::OnRetryGameClicked()
{
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != ABPlayerController);
	ABPlayerController->RestartLevel();
}
