// Fill out your copyright notice in the Description page of Project Settings.


#include "ABItemBox.h"
#include "ABCharacter.h"
#include "ABWeapon.h"

// Sets default values
AABItemBox::AABItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	// �ڽ� ���� ũ���� ����
	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		// ��ƼŬ ������Ʈ�� ���ø����� ����
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	// �ڽ� �޽ô� ��Ʈ ������Ʈ�� Trigger ��ǥ�� �������� �Ʒ��� ���� ��ǥ�� ��ġ
	Box->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));

	// SetCollisionProfileName() : �浹ü�� �̸��� ����, �ش� �̸����� �浹ü�� ����
	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AABWeapon::StaticClass();

}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABItemBox::PostInitializeComponents()
{
	// OnComponentBeginOverlap: �ڽ� ������Ʈ�� ������ �̺�Ʈ�� ó���� �� �ֵ��� ����Ǿ� �ִ� ��������Ʈ
	// ����ڰ� ���� ������ �ڽ��� ��� �Լ��� �ش� ��������Ʈ�� ���ε��ϸ�, ������ �̺�Ʈ�� �߻��� ������ ���ε��� ��� �Լ��� ȣ���.
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap);
}

// Called every frame
void AABItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);

	auto ABCharacter = Cast<AABCharacter>(OtherActor);
	ABCHECK(nullptr != ABCharacter);

	// ������ �ڽ��� ĳ���� �浹 ��, ���� ���͸� ���� �� ĳ���Ϳ��� ����
	if (nullptr != ABCharacter && nullptr != WeaponItemClass)
	{
		if (true == ABCharacter->CanSetWeapon())
		{
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			ABCharacter->SetWeapon(NewWeapon);

			// ��ƼŬ ����
			Effect->Activate(true);
			// SetHiddenInGame : ���� �÷��� ȭ�鿡���� �������, ������ ȭ�鿡�� ��������. 
			Box->SetHiddenInGame(true, true);
			SetActorEnableCollision(false);
			// ��� �Լ��� ���ε�
			Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
		}
		else
		{
			ABLOG(Warning, TEXT("%s can't equip weapon currently."), *ABCharacter->GetName());
		}
	}
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}

