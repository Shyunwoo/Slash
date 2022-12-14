// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashHUD.h"
#include "SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* Controller = World->GetFirstPlayerController();
        if(Controller&&SlahOverlayClass)
        {
            SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlahOverlayClass);
            SlashOverlay->AddToViewport();
        }
    }
}
