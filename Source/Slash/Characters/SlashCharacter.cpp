// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Slash/Items/Item.h"
#include "Slash/Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
 
ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=false;
	bUseControllerRotationRoll=false;
	
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength=300.f;
	ViewCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));	
}

void ASlashCharacter::MoveForward(float Value)
{
	if(ActionState!=EActionState::EAS_Unoccupied)return;
	if(Controller&&(Value!=0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if(ActionState!=EActionState::EAS_Unoccupied)return;
	if(Controller&&(Value!=0.f))
	{
		const FRotator ControlRotation=GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	if(Controller&&(Value!=0.f))
	{
		AddControllerYawInput(Value);
	}
}

void ASlashCharacter::LookUp(float Value)
{
	if(Controller&&(Value!=0.f))
	{
		AddControllerPitchInput(Value);
	}
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if(OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else
	{	
		if(CanDisarm())
		{
			Disarm();
		}
		else if(CanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if(CanAttack())
	{
		PlayAttackMontage();
		ActionState=EActionState::EAS_Attacking;
	}

}

void ASlashCharacter::EquipWeapon(class AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("hand_rSocket"), this, this);
	CharacterState=ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem=nullptr;
	EquippedWeapon=Weapon;
}

void ASlashCharacter::AttackEnd()
{
	ActionState=EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState==EActionState::EAS_Unoccupied && CharacterState!=ECharacterState::ECS_Unequipped;	
}

void ASlashCharacter::PlayEquipMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance&&EquipMontage)
	{

		AnimInstance->Montage_Play(EquipMontage);	
	}
}

void ASlashCharacter::PlayUnEquipMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance&&UnEquipMontage)
	{
		AnimInstance->Montage_Play(UnEquipMontage);
	}
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState==EActionState::EAS_Unoccupied&&CharacterState!=ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState==EActionState::EAS_Unoccupied&&CharacterState==ECharacterState::ECS_Unequipped&&EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage();
	CharacterState=ECharacterState::ECS_Unequipped;
	ActionState=EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayUnEquipMontage();
	CharacterState=ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState=EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("hand_rSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState=EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState=EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"),this,&ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"),this,&ASlashCharacter::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionState=EActionState::EAS_HitReaction;
}

float ASlashCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	return DamageAmount;
}