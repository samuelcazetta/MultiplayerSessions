# MultiplayerSessions

Plugin C++ para Unreal Engine com criação, busca, entrada, início e encerramento de sessões multiplayer usando a interface `IOnlineSession` do Online Subsystem. Inclui widgets de exemplo para hospedar/entrar em uma sessão e iniciar uma partida no lobby.

## Compatibilidade e mudanças entre versões

> **O setup pode mudar a cada nova versão da Unreal Engine.** Plugins necessários, classes de Net Driver, opções de `.ini`, handshake, Steamworks e dependências de compilação podem mudar, inclusive entre atualizações da mesma versão. Os exemplos abaixo são um ponto de partida para UE 5.7/5.8 baseado no guia de referência, e não uma garantia de compatibilidade com versões futuras. Ao atualizar a engine, consulte a documentação correspondente, recompile o plugin e teste uma build empacotada com dois jogadores.

Esta cópia usa `OnlineSubsystem`, `OnlineSubsystemSteam` e `SteamSockets`, declarados no `.uplugin`, além de `OnlineSubsystemUtils` no módulo C++. O código usa a API clássica de Online Subsystem; instruções para outras APIs de serviços online podem exigir adaptações.

O [guia da Druid Mechanics](https://publish.obsidian.md/druid-mechanics/Druid+Mechanics+Patreon+Vault/Updates/Multiplayer+Plugin) apresenta configurações diferentes por versão:

| Versão no guia | Configuração indicada pelo guia |
| --- | --- |
| UE 5.5 e anteriores | Online Subsystem Steam com o Net Driver legado `OnlineSubsystemSteam.SteamNetDriver`. |
| UE 5.6 | Online Subsystem Steam + Steam Sockets, com `SteamSockets.SteamSocketsNetDriver`. |
| UE 5.7 e 5.8 | Steam Sockets, configuração de conexão atualizada e variáveis de handshake descritas abaixo. O guia também pede `SteamSockets` nas dependências do módulo principal do jogo. |

Essa tabela descreve o guia original. Ela não significa que esta revisão do plugin foi testada em todas essas versões. Em particular, esta cópia já depende de Steam Sockets; usar o setup legado pode exigir mudanças no código e no descritor. Não foi executada uma compilação nem um teste multiplayer como parte da preparação deste repositório.

## Conteúdo

```text
MultiplayerSessions/
├── MultiplayerSessions.uplugin
├── Source/MultiplayerSessions/
├── Content/Widgets/
│   ├── WBP_Menu.uasset
│   └── WBP_Lobby.uasset
└── Resources/
```

- `UMultiplayerSessionsSubsystem`: subsistema de Game Instance que gerencia sessões e publica delegates com os resultados das operações.
- `UMenu` / `WBP_Menu`: exemplo de interface com Host e Join; após hospedar, viaja para o mapa configurado; ao entrar, resolve o endereço da sessão e conecta o cliente.
- `ULobby` / `WBP_Lobby`: exemplo para iniciar a sessão e chamar `StartMatch()` no Game Mode da autoridade.

Os mapas de menu, lobby e gameplay pertencem ao projeto consumidor e não estão incluídos neste repositório.

## 1. Preparar o projeto

Use um projeto C++ com uma toolchain compatível com sua versão da Unreal. No Windows, instale os componentes de desenvolvimento C++ e o Windows SDK exigidos pela engine. Um projeto Third Person pode servir de base para testar movimentação e replicação.

Se seu projeto é apenas Blueprint, adicione uma classe C++ e configure a toolchain antes de compilar este plugin a partir do código-fonte.

Feche o Unreal Editor antes de adicionar ou atualizar os arquivos do plugin.

## 2. Instalar o plugin

Na pasta que contém o `.uproject`, crie `Plugins/`, se necessário, e clone o repositório:

```sh
git clone https://github.com/samuelcazetta/MultiplayerSessions.git Plugins/MultiplayerSessions
```

Também é possível baixar o ZIP e copiar seu conteúdo para `Plugins/MultiplayerSessions`. Renomeie a pasta extraída se necessário: o arquivo deve ficar em `SeuProjeto/Plugins/MultiplayerSessions/MultiplayerSessions.uplugin`, sem uma pasta adicional entre eles.

Para manter o plugin como dependência versionada de um projeto que já usa Git, use um submódulo em vez do clone acima:

```sh
git submodule add https://github.com/samuelcazetta/MultiplayerSessions.git Plugins/MultiplayerSessions
```

Ao clonar um projeto que já possui o submódulo, inicialize-o com `git submodule update --init --recursive`. Evite manter outra cópia do mesmo plugin instalada na engine.

Gere novamente os arquivos de projeto, se necessário, e compile o target **Development Editor** do seu jogo pela IDE. `Binaries/` e `Intermediate/` são gerados localmente e não fazem parte do repositório.

No editor, em **Edit > Plugins**, confirme que estão habilitados:

- MultiplayerSessions;
- Online Subsystem;
- Online Subsystem Steam;
- Steam Sockets.

Reinicie o editor quando solicitado. Os nomes e a localização dessas opções podem variar entre versões.

## 3. Configurar Steam e rede no projeto

As configurações abaixo devem ser feitas nos arquivos do **projeto consumidor**, não na pasta do plugin. Mescle com as seções existentes, evitando configurações de drivers duplicadas ou conflitantes.

### Exemplo do guia para UE 5.7/5.8

Em `Config/DefaultEngine.ini`:

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

Esse bloco reproduz as opções de configuração da seção UE 5.7/5.8 do guia de referência. A seção de conexão e as variáveis de handshake devem ser reavaliadas para a versão exata da sua engine; não aplique esses ajustes automaticamente a versões mais novas. As páginas da Epic sobre Online Subsystem Steam e Steam Sockets também mostram configurações diferentes conforme o transporte utilizado.

`!NetDriverDefinitions=ClearArray` remove todas as definições herdadas desse array. Se seu projeto usa outros drivers, preserve/redeclare as definições necessárias ao mesclar o exemplo.

`480` é o App ID de teste compartilhado da Steam (Spacewar). Use o App ID do seu próprio jogo para distribuição. Este plugin cria sessões com `bUsesPresence=true` e `bUseLobbiesIfAvailable=true`; a Epic distingue o uso de lobbies do fluxo de sessões que exige `bInitServerOnClient`. O exemplo mantém a opção do guia, mas ela deve ser revisada conforme seu fluxo e sua versão.

Em `Config/DefaultGame.ini`, defina um limite adequado ao seu jogo, por exemplo:

```ini
[/Script/Engine.GameSession]
MaxPlayers=10
```

O guia usa `100` como exemplo. Ajuste esse limite e o parâmetro `NumPlayers` de `MenuSetup` de forma coerente; eles representam configurações diferentes e um não substitui o outro.

### Dependência no módulo principal do jogo

O guia pede, para UE 5.7, adicionar `"SteamSockets"` a `PublicDependencyModuleNames` no arquivo `Source/SeuJogo/SeuJogo.Build.cs`. Acrescente a entrada à lista existente sem substituir as outras dependências:

```csharp
PublicDependencyModuleNames.Add("SteamSockets");
```

O módulo deste plugin já declara `SteamSockets` em suas próprias dependências privadas. A linha acima corresponde à orientação adicional do guia para o módulo do jogo; confirme sua necessidade para a versão usada.

Caso seu código C++ inclua os headers públicos do plugin, declare também a dependência `"MultiplayerSessions"` no módulo consumidor e as dependências de headers online exigidas pela sua compilação.

## 4. Criar mapas e abrir o menu

1. Crie um mapa `Content/Maps/StartupMap` e outro `Content/Maps/Lobby` no projeto.
2. Em **Project Settings > Maps & Modes**, use `StartupMap` como **Game Default Map** e, se desejar, **Editor Startup Map**.
3. No Content Browser/Drawer, habilite **Show Plugin Content** e localize `MultiplayerSessions Content/Widgets`.
4. No Level Blueprint do `StartupMap`, conecte **Event BeginPlay** a **Create Widget**, selecione `WBP_Menu` e forneça o **Get Player Controller** como **Owning Player**.
5. Na instância criada, chame **Menu Setup** uma vez, com os parâmetros abaixo.

| Parâmetro C++ | Exemplo | Função |
| --- | --- | --- |
| `NumPlayers` | `10` | Número de conexões públicas anunciado pela sessão. |
| `MatchType` | `FreeForAll` | Filtro usado pelo menu para escolher uma sessão. Host e cliente devem usar o mesmo valor. |
| `bIsListen` | `true` | Adiciona `?listen` ao destino do host. |
| `PathToLobby` | `/Game/Maps/Lobby` | Caminho do mapa no projeto, sem extensão `.umap`. |

`MenuSetup` já adiciona o widget ao viewport e configura o foco e o cursor. Confira o grafo do widget ao integrá-lo para evitar chamadas duplicadas de setup.

**Nesta cópia o asset se chama `WBP_Menu`; o guia original usa `WBP_MenuWidget`.** O valor padrão no C++ para o mapa é `/Game/ThirdPerson/Lobby`, portanto informe explicitamente `/Game/Maps/Lobby` se seguir a organização acima. Caminhos de assets começam com `/Game/`, não com `/Content/`.

Ao criar seu próprio widget derivado de `UMenu`, mantenha os botões `HostButton` e `JoinButton`, exigidos pelo binding C++.

## 5. Integrar o lobby, se necessário

O menu principal permite hospedar e entrar sem usar `WBP_Lobby`. Para usar também o exemplo de início de partida:

- Crie `WBP_Lobby` com um Player Controller local como proprietário e chame `SetupMenu` no fluxo de lobby do host.
- O widget só abre quando a sessão local está em estado `Pending`.
- O botão `StartSessionButton` chama `StartSession`. Em caso de sucesso, a implementação chama `StartMatch()` em um `AGameMode` da autoridade e fecha a interface local.
- Configure o Game Mode do lobby para que a partida não comece automaticamente, por exemplo com **Delayed Start**, se seu fluxo depender desse botão. Isso também pode afetar spawn/posse de personagens e exige lógica no projeto.

O exemplo não implementa uma tela de ready-check replicada nem escolhe/viaja automaticamente para um mapa de gameplay ao iniciar a partida. Essas etapas e o gerenciamento de interface dos outros jogadores ficam a cargo do jogo.

## 6. Empacotar e testar

1. Em **Project Settings > Packaging**, inclua `StartupMap`, `Lobby` e os mapas de gameplay na lista de mapas a empacotar, ou configure uma política de cooking equivalente.
2. Compile e empacote o projeto para a plataforma de teste. A localização do comando de packaging varia por versão da Unreal.
3. Execute a mesma build em dois computadores, cada um com a Steam aberta e uma conta Steam diferente.
4. No primeiro computador, clique em **Host** e aguarde a entrada no lobby. No segundo, clique em **Join**.
5. Confirme a conexão, a replicação esperada e, se integrado, o início da partida pelo host.

O guia recomenda a mesma **Download Region** da Steam durante o teste com App ID `480`; trate isso como uma orientação de diagnóstico daquele ambiente. Testes dentro do editor não substituem a validação de Steam com builds empacotadas.

## Diagnóstico e limites desta revisão

- **Plugin não aparece:** confira a posição do `.uplugin`, a versão da engine, a compilação C++ e os logs. Não reutilize binários de outra versão da Unreal.
- **Widget não aparece:** habilite Plugin Content, verifique a classe do widget, seu proprietário e a chamada de setup.
- **Host não viaja:** confirme que o mapa existe no caminho informado e foi incluído no cooking.
- **Busca não encontra a partida:** verifique Steam, App ID, `MatchType`, configuração de rede, compatibilidade das builds e os logs de Online Subsystem/Steam Sockets.
- **Filtro de projeto:** esta revisão anuncia e busca `ProjectId=MPP`, definido em `MultiplayerSessionsSubsystem.h`. Personalize esse identificador para seu jogo, especialmente ao usar o App ID compartilhado `480`, e mantenha o mesmo valor em todos os participantes. Esse filtro não oferece autenticação ou isolamento de segurança.
- **Compatibilidade de builds:** `BuildUniqueId` está fixado em `1` no código; isso não comprova que builds diferentes são compatíveis. Use a mesma revisão nos testes e adapte a política de versionamento ao seu jogo.
- **Escopo:** o fluxo incluído é um exemplo de listen server; servidores dedicados e outras plataformas/subsistemas exigem integração e validação próprias.

## Referências e créditos

As instruções de instalação e configuração foram adaptadas do material da **Druid Mechanics**, com os nomes e o comportamento desta cópia conferidos no código. Este repositório não é uma distribuição oficial da Druid Mechanics.

- [Druid Mechanics — Multiplayer Plugin](https://publish.obsidian.md/druid-mechanics/Druid+Mechanics+Patreon+Vault/Updates/Multiplayer+Plugin) — guia de referência com diferenças entre versões da Unreal.
- [Epic — Online Subsystem Steam](https://dev.epicgames.com/documentation/en-us/unreal-engine/online-subsystem-steam-interface-in-unreal-engine) — configuração de Steam, App ID, sessões e lobbies.
- [Epic — Using Steam Sockets](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-steam-sockets-in-unreal-engine) — transporte e configuração do Net Driver.
- [Epic — Plugins in Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/plugins-in-unreal-engine) — estrutura e integração de plugins.

Ao consultar essas referências, selecione a versão correspondente à sua engine e valide o setup novamente após atualizações.
