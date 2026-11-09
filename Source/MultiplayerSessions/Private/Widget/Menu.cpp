// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	// bind OnClicked to our buttons
	if (IsValid(HostButton))
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}
	if (IsValid(JoinButton))
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return false;
	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	return true;
}


void UMenu::MenuSetup(int32 NumPlayers, FString MatchType, bool bIsListen, FString PathToLobby)
{
	// add to viewport, focusable, visible
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	NumOfPlayers = NumPlayers;
	TypeofMatch = MatchType;
	LobbyPath = bIsListen ? (PathToLobby + TEXT("?listen")) : PathToLobby;
	

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!IsValid(PlayerController)) return;

		// input settings: widget focused, mouse is free	
		FInputModeUIOnly InputModeUI;
		InputModeUI.SetWidgetToFocus(TakeWidget());
		InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		// turn on settings and show mouse cursor
		PlayerController->SetInputMode(InputModeUI);
		PlayerController->SetShowMouseCursor(true);
	}

	if (IsValid(MultiplayerSessionsSubsystem))
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSession.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.
		                              AddDynamic(this, &ThisClass::OnDestroySession);
	}
}

void UMenu::NativeDestruct()
{
	Super::NativeDestruct();
	MenuTearDown();
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			TEXT("Host Button Clicked"));
	}

	if (IsValid(MultiplayerSessionsSubsystem))
	{
		MultiplayerSessionsSubsystem->CreateSession(NumOfPlayers, TypeofMatch);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (!IsValid(MultiplayerSessionsSubsystem)) return;
	MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Yellow,
				FString::Printf(TEXT("Session created successfully. Traveling now...")));
		}
		
		UWorld* World = GetWorld();
		if (!IsValid(World)) return;
		World->ServerTravel(LobbyPath);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Red,
				FString::Printf(TEXT("Session creation failed")));
		}
		
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful)
{
	if (!IsValid(MultiplayerSessionsSubsystem)) return;

	// filter results
	FString SettingsValue;
	for (auto& Result : SessionsResult)
	{
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		// join our session 
		if (SettingsValue == TypeofMatch)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	
	if (!bWasSuccessful || SessionsResult.Num()==0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	// Get our online subsystem
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
	if (OnlineSubsystem)
	{
		// get our session interface
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (!SessionInterface.IsValid()) return;

		// get address and join
		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
		if (!Address.IsEmpty())
		{
			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (!IsValid(PlayerController)) return;
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		const FInputModeGameOnly InputGameMode;
		PlayerController->SetInputMode(InputGameMode);
		PlayerController->SetShowMouseCursor(false);
	}
}
