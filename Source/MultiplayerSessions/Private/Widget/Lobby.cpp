// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Lobby.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "GameFramework/GameMode.h"

bool ULobby::Initialize()
{
	if (!Super::Initialize()) return false;

	if (IsValid(StartSessionButton))
	{
		StartSessionButton->OnClicked.AddDynamic(this, &ULobby::StartSessionButtonClicked);
	}


	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return false;
	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	return true;
}

void ULobby::SetupMenu()
{
	//Only works if GameMode is bDelayedStart true, but at the same time DelayedStart will pause GM to proceed with Character posses.


	UE_LOG(LogTemp, Warning, TEXT("[Lobby] SetupMenu ENTERED"));

	if (IsInViewport())	return;
	if (!IsValid(MultiplayerSessionsSubsystem)) return;

	
	const EOnlineSessionState::Type SessionState = MultiplayerSessionsSubsystem->GetSessionState();
	
	UE_LOG(LogTemp, Warning,
	       TEXT("[Lobby] SessionState=%d | Expected Pending=%d"),
	       static_cast<int32>(SessionState),
	       static_cast<int32>(EOnlineSessionState::Pending));

	if (SessionState != EOnlineSessionState::Pending)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[Lobby] ABORT: Session is not Pending"));
		return;
	}

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	APlayerController* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController))	return;
	
	FInputModeGameAndUI InputModeGameAndUI;
	InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeGameAndUI);
	PlayerController->SetShowMouseCursor(true);

	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddUniqueDynamic(this, &ThisClass::OnStartSession);
}

void ULobby::TearDownLobbyMenu()
{
	if (IsValid(MultiplayerSessionsSubsystem))
	{
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete
		                            .RemoveDynamic(this, &ThisClass::OnStartSession);
	}
	RemoveFromParent();

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		const FInputModeGameOnly InputGameMode;
		PlayerController->SetInputMode(InputGameMode);
		PlayerController->SetShowMouseCursor(false);
	}
}


void ULobby::StartSessionButtonClicked()
{
	if (IsValid(MultiplayerSessionsSubsystem))
	{
		MultiplayerSessionsSubsystem->StartSession();
	}
}

void ULobby::OnStartSession(bool bWasSuccessful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
		                                 FString::Printf(
			                                 TEXT("StartSession: %s"),
			                                 bWasSuccessful ? TEXT("true") : TEXT("false")));
	}
	if (!bWasSuccessful) return;

	if (UWorld* World = GetWorld())
	{
		if (AGameMode* GameMode = World->GetAuthGameMode<AGameMode>())
		{
			GameMode->StartMatch();
		}
	}

	TearDownLobbyMenu();
}
