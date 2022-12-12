// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Slash/Components/AttributeComponent.h"
#include "Slash/HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Slash/Items/Weapons/Weapon.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	HealthBarWidget=CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing=CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius=4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement=true;
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;

	GetCharacterMovement()->MaxWalkSpeed=125.f;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	
	EnemyController=Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);

	if(PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World=GetWorld();
	if(World&&WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("b_MF_Weapon_R"), this, this);
		EquippedWeapon=DefaultWeapon;
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if(AnimInstance&&DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);

		const int32 Selection=FMath::RandRange(0,2);
		FName SectionName = FName();
		switch(Selection)
		{
		case 0:
			SectionName=FName("Death1");
			DeathPose=EDeathPose::EDP_Death1;
		break;
		case 1:
			SectionName=FName("Death2");
			DeathPose=EDeathPose::EDP_Death2;
		break;
		case 2:
			SectionName=FName("Death3");
			DeathPose=EDeathPose::EDP_Death3;
		break;
		default:
		break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}

void AEnemy::Attack()
{
	Super::Attack();

	PlayAttackMontage();
}

void AEnemy::PlayAttackMontage()
{
	Super::PlayAttackMontage();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance&&AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if(Target==nullptr) return false;

	const double DistanceToTarget = (Target->GetActorLocation()-GetActorLocation()).Size();

	return DistanceToTarget<=Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if(EnemyController==nullptr||Target==nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for(AActor* Target:PatrolTargets)
	{
		if(Target!=PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets=ValidTargets.Num();
	if(NumPatrolTargets>0)
	{
		const int32 TargetSelection=FMath::RandRange(0, NumPatrolTargets-1);
		
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::CheckCombatTarget()
{
	if(!InTargetRange(CombatTarget, CombatRadius))
	{
		CombatTarget=nullptr;

		if(HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState=EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed=125.f;
		MoveToTarget(PatrolTarget);
	}
	else if(!InTargetRange(CombatTarget, AttackRadius) && EnemyState!=EEnemyState::EES_Chasing)
	{
		EnemyState=EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed=300.f;
		MoveToTarget(CombatTarget);
	}
	else if(InTargetRange(CombatTarget, AttackRadius) && EnemyState!= EEnemyState::EES_Attacking)
	{
		EnemyState=EEnemyState::EES_Attacking;

		Attack();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if(InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget=ChoosePatrolTarget();

		const float WaitTime=FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if(EnemyState==EEnemyState::EES_Chasing) return;

	if(SeenPawn->ActorHasTag(FName("SlashCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed=300.f;
		CombatTarget=SeenPawn;

		if(EnemyState!=EEnemyState::EES_Attacking)
		{
			EnemyState=EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(EnemyState>EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
	
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	if(Attributes&&Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint, FRotator::ZeroRotator, FVector(0.1f, 0.1f, 0.1f));
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if(Attributes&&HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	CombatTarget = EventInstigator->GetPawn();
	EnemyState=EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed=300.f;
	MoveToTarget(CombatTarget);

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}	
}

