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

	// 박스 에셋 크기의 절반
	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		// 파티클 컴포넌트의 템플릿으로 지정
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	// 박스 메시는 루트 컴포넌트인 Trigger 좌표를 기준으로 아래와 같은 좌표에 위치
	Box->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));

	// SetCollisionProfileName() : 충돌체에 이름을 짓고, 해당 이름으로 충돌체를 구분
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
	// OnComponentBeginOverlap: 박스 컴포넌트에 오버랩 이벤트를 처리할 수 있도록 선언되어 있는 델리게이트
	// 사용자가 만든 아이템 박스의 멤버 함수를 해당 델리게이트에 바인딩하면, 오버랩 이벤트가 발생할 때마다 바인딩한 멤버 함수가 호출됨.
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

	// 아이템 박스와 캐릭터 충돌 시, 무기 엑터를 생성 후 캐릭터에게 장착
	if (nullptr != ABCharacter && nullptr != WeaponItemClass)
	{
		if (true == ABCharacter->CanSetWeapon())
		{
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			ABCharacter->SetWeapon(NewWeapon);

			// 파티클 생성
			Effect->Activate(true);
			// SetHiddenInGame : 게임 플레이 화면에서는 사라지고, 에디터 화면에는 남아있음. 
			Box->SetHiddenInGame(true, true);
			SetActorEnableCollision(false);
			// 멤버 함수에 바인딩
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

