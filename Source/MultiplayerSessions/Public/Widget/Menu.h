// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// overring Initialize to bind our buttons
	virtual bool Initialize() override;
	
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumPlayers = 10, FString MatchType = FString("FreeForAll"), bool bIsListen = true, FString PathToLobby = FString("/Game/ThirdPerson/Lobby"));
	
protected:
	
	// called when a level is changed via clientTravel or openlevel
	virtual void NativeDestruct() override;
	
	// callback for the custom delegates on the MultiplayerSessionsSubsystem
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()	void OnStartSession(bool bWasSuccessful);
	
	
	
private:
	// our Subsytem
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
	
	// automatically references the button with the same name in the Widget Blueprint.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
	
	int32 NumOfPlayers;
	FString TypeofMatch;
	FString LobbyPath;
	
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();
	
	void MenuTearDown();
};
