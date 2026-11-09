# MultiplayerSessions

A C++ plugin for Unreal Engine that handles creating, finding, joining, starting, and destroying multiplayer sessions through Steam. It comes with a simple Host/Join menu (`WBP_Menu`) and an optional lobby widget (`WBP_Lobby`).

> **This setup is for Unreal Engine 5.8.** Newer versions may need different plugins, network settings, or build dependencies. Check the docs for your engine version, rebuild the plugin, and test again when upgrading.

## Install

Start with a C++ project and a working Unreal C++ toolchain. A Third Person project is a handy starting point. If you're using a Blueprint-only project, add a C++ class first.

Close the editor, then run this from the folder containing your `.uproject`:

```sh
git clone https://github.com/samuelcazetta/MultiplayerSessions.git Plugins/MultiplayerSessions
```

You can also download the ZIP. Just make sure the descriptor ends up at:

```text
YourProject/Plugins/MultiplayerSessions/MultiplayerSessions.uplugin
```

If your project uses Git, you can use a submodule instead:

```sh
git submodule add https://github.com/samuelcazetta/MultiplayerSessions.git Plugins/MultiplayerSessions
```

When cloning a project that already uses the submodule, run `git submodule update --init --recursive`.

Regenerate project files if needed, then build your game's **Development Editor** target from your IDE. Open the editor and check **Edit > Plugins** for **MultiplayerSessions**, **Online Subsystem**, **Online Subsystem Steam**, and **Steam Sockets**. Enable them and restart if prompted.

## Configure Steam

These settings go in your **game project's** config files. Merge them with any existing sections to avoid duplicate entries.

In `Config/DefaultEngine.ini`:

```ini
[/Script/Engine.GameEngine]
!NetDriverDefinitions=ClearArray
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/SteamSockets.SteamSocketsNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

[OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=true
SteamDevAppId=480
bInitServerOnClient=true

[/Script/OnlineSubsystemSteam.SteamNetDriver]
NetConnectionClassName="SteamSockets.SteamSocketsNetConnection"

[ConsoleVariables]
net.CurrentHandshakeVersion=2
net.MinHandshakeVersion=2
```

`480` is Steam's shared test App ID (Spacewar). Replace it with your own App ID before releasing your game. The `ClearArray` line clears inherited network drivers, so add back any other drivers your project needs.

In `Config/DefaultGame.ini`:

```ini
[/Script/Engine.GameSession]
MaxPlayers=10
```

Pick a player limit that fits your game and keep it consistent with the menu's `NumPlayers` setting.

Following the reference setup, add Steam Sockets to your game's `.Build.cs` constructor, keeping the existing dependencies:

```csharp
PublicDependencyModuleNames.Add("SteamSockets");
```

The plugin already declares its own Steam Sockets dependency. If your game uses the plugin's C++ headers, also add `MultiplayerSessions` to your game module's dependencies.

## Set up the menu

Maps aren't included, so create these in your project:

- `Content/Maps/StartupMap` for the menu.
- `Content/Maps/Lobby` for the map players join.

Set **Game Default Map** to `StartupMap` under **Project Settings > Maps & Modes**. You can use it as the **Editor Startup Map** too.

Enable **Show Plugin Content** in the Content Browser and find `MultiplayerSessions Content/Widgets/WBP_Menu`.

In the `StartupMap` Level Blueprint, connect:

```text
Event BeginPlay -> Create Widget (WBP_Menu) -> Menu Setup
```

Use **Get Player Controller** as the widget's **Owning Player**. Set `Menu Setup` to:

- **NumPlayers:** `10`
- **MatchType:** `FreeForAll` (use the same value on host and client)
- **bIsListen:** `true`
- **PathToLobby:** `/Game/Maps/Lobby`

`MenuSetup` already adds the widget to the viewport and handles input focus, so call it once. Set the lobby path explicitly: the C++ default is `/Game/ThirdPerson/Lobby`. Asset paths use `/Game/`, with no `.umap` extension.

If you make your own widget based on `UMenu`, keep the `HostButton` and `JoinButton` names for the C++ bindings.

### Optional lobby controls

Create `WBP_Lobby` for the host's local Player Controller and call `SetupMenu` while the session is `Pending`. Its `StartSessionButton` starts the session and calls `StartMatch()` on the host's `AGameMode`.

Use **Delayed Start** if the lobby should wait for that button, and handle player spawning/possession in your game as needed. The widget doesn't include a replicated ready system or automatic travel to another gameplay map.

## Package and try it out

1. Add `StartupMap`, `Lobby`, and any gameplay maps to your packaging/cooking settings.
2. Package the project and run the same build on two computers, each with Steam open and a different Steam account.
3. Click **Host** on one machine, wait for the lobby, then click **Join** on the other.

If joining fails, check the logs, App ID, `MatchType`, and network settings. The reference guide also suggests matching Steam **Download Regions** when testing with App ID `480`. Use packaged builds to validate Steam connectivity.

The session filter currently uses `ProjectId=MPP` in `MultiplayerSessionsSubsystem.h`. Give your game its own value and keep it the same across players, especially when sharing App ID `480`. `BuildUniqueId` is fixed at `1`, so use matching builds when testing.

## Credits and docs

Setup instructions are adapted from [Druid Mechanics' Multiplayer Plugin guide](https://publish.obsidian.md/druid-mechanics/Druid+Mechanics+Patreon+Vault/Updates/Multiplayer+Plugin), with widget names adjusted for this copy. This isn't an official Druid Mechanics release.

For engine-specific details, see Epic's [Online Subsystem Steam](https://dev.epicgames.com/documentation/en-us/unreal-engine/online-subsystem-steam-interface-in-unreal-engine) and [Steam Sockets](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-steam-sockets-in-unreal-engine) docs.
