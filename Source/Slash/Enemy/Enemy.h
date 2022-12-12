// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slash/Characters/BaseCharacter.h"
#include "Slash/Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void CheckCombatTarget();
	void CheckPatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState= EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere, Category=Combat)
	float DeathLifeSpan =8.f;

private:
	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	double CombatRadius=1000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius=150.f;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius=200.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float PatrolWaitMin=5.f;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float PatrolWaitMax=10.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category=Combat)
	float AttackMin=0.5f;

	UPROPERTY(EditAnywhere, Category=Combat)
	float AttackMax=1.f;

	UPROPERTY(EditAnywhere, Category=Combat)
	float PatrollingSpeed=125.f;

	UPROPERTY(EditAnywhere, Category=Combat)
	float ChasingSpeed=300.f;

private:
	void PatrolTimerFinished();

	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();

	void StartAttackTimer();
	void ClearAttackTimer();

	void SpawnDefaultWeapon();
	void InitializeEnemy();
};
