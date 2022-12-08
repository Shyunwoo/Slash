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
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=false;
	bUseControllerRotationRoll=false;
	
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.f, 400.f, 0.f);

	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength=300.f;
	ViewCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
		OverlappingWeapon->Equip(GetMesh(), FName("hand_rSocket"));
		CharacterState=ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem=nullptr;
		EquippedWeapon=OverlappingWeapon;
	}
	else
	{	
		if(CanDisarm())
		{
			PlayEquipMontage();
			CharacterState=ECharacterState::ECS_Unequipped;
			ActionState=EActionState::EAS_EquippingWeapon;
		}
		else if(CanArm())
		{
			PlayUnEquipMontage();
			CharacterState=ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState=EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::Attack()
{
	if(CanAttack())
	{
		PlayAttackMontage();
		ActionState=EActionState::EAS_Attacking;
	}

}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance&&AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection=FMath::RandRange(0,2);
		FName SectionName = FName();
		switch(Selection)
		{
		case 0:
			SectionName=FName("Combo1");
		break;
		case 1:
			SectionName=FName("Combo2");
		break;
		case 2:
			SectionName=FName("Combo4");
		break;
		default:
		break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
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
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
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

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

