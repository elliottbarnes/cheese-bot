# Cheese Bot

A C++ / BWAPI Protoss cannon-rush coursework bot by **Jacob Critch and Elliott Barnes**, with an interactive explanation of its decision policy.

**[Explore the decision simulator](https://elliottbarnes.github.io/cheese-bot/)** · [C++ policy](StarCraftCannonRushBot/DecisionPolicy.h) · [BWAPI integration](StarCraftCannonRushBot/StarterBot.cpp)

The browser example is a schematic simulator of decision gates. It does not execute StarCraft, BWAPI, combat, pathfinding, build placement, or a timed economy. It contains no game assets. A requested order may still fail in the real game.

## Try and test the portable part

Serve the browser example with Python 3:

```sh
python3 -m http.server 8000 --bind 127.0.0.1 --directory demo
```

Open `http://127.0.0.1:8000`. Choose an example world snapshot or change worker/pylon counts and the forge, enemy-discovery, and scout-readiness conditions. The enabled requests update immediately.

With Node.js 24 and a C++17 compiler (`c++`, or set `CXX`):

```sh
node --test
node scripts/build-demo.mjs
```

The tests compile the actual pure C++ policy and compare the browser model against it across all 160 combinations in a bounded world-state corpus. They also cover readiness gates, the worker cap, overlapping build requests, and invalid browser inputs. The policy can be tested without Node through CMake:

```sh
cmake -S . -B build/policy
cmake --build build/policy
ctest --test-dir build/policy --output-on-failure
```

## Native Windows client

The original repository did not contain the Visual Studio solution referenced by its old README. `CMakeLists.txt` now supplies an explicit native target, disabled by default. Building and running it requires **Windows, MSVC, Win32 architecture, BWAPI 4.4.0, and a legitimate compatible StarCraft: Brood War installation**. The historical target is game version 1.16.1. Obtain the game through legitimate channels; no game download is supplied here.

Use the [official BWAPI 4.4.0 release and build notes](https://github.com/bwapi/bwapi/releases/tag/v4.4.0). BWAPI's library projects must be compiled with your matching compiler and build configuration; precompiled libraries from an unrelated toolset are not interchangeable. Build `BWAPILIB` and `BWAPIClient` from that release, then configure this client with the resulting Release libraries:

```powershell
cmake -S . -B build/native -A Win32 -DBUILD_BWAPI_CLIENT=ON `
  -DBWAPI_DIR=C:/libraries/BWAPI_440 `
  -DBWAPI_LIBRARY=C:/libraries/BWAPI_440/Release/BWAPILIB.lib `
  -DBWAPI_CLIENT_LIBRARY=C:/libraries/BWAPI_440/Release/BWAPIClient.lib
cmake --build build/native --config Release --target CheeseBot
```

Adjust the example library paths to the actual outputs. Use BWAPI's documented injector/client setup for a local game, run `build/native/Release/CheeseBot.exe`, and select Protoss with one opponent. The client waits for BWAPI and then handles match events. [BWAPI configuration reference](https://github.com/bwapi/bwapi/wiki/Configuration).

The Windows/BWAPI build and game session have **not** been executed in this completion pass. A compatible Windows environment is still required to validate that integration; browser and portable-policy tests do not establish native playability or competitive performance.

## What the code does

`onFrame()` updates map information, assigns/scouts a worker before assigning idle workers to mining, requests home/forward structures, and trains toward six owned workers. The extracted policy preserves the original thresholds:

| Request       | Gate                                                                                |
| ------------- | ----------------------------------------------------------------------------------- |
| Home pylon    | No pylon counted                                                                    |
| Home forge    | Pylon exists; no forge counted                                                      |
| Forward pylon | Enemy located, scout ready, forge exists, fewer than three pylons                   |
| Photon cannon | Enemy located, scout ready, at least two pylons                                     |
| Worker        | Fewer than six workers owned; the integration also checks the depot is not training |

The two forward gates are independent. At two pylons they can both request an order in one frame. Counts include units under construction. The policy does not establish available minerals, completed tech, power coverage, valid placement, or whether an order succeeded; BWAPI applies those constraints. The browser deliberately exposes these limits.

The completion pass fixes the unassigned-scout dereference path, selects only one replacement scout after a death, resets global state between matches, and extracts the testable policy. It preserves existing group attribution, source files, and map/tool helpers. There is no new license grant.

## Verification and limits

CI builds/tests the portable C++ policy and browser example, then publishes only the explicit static `demo/` artifact to Pages after main changes. Actions are pinned to commits, ordinary verification has read-only repository permission, and Pages deployment has its own limited job permissions. `build.json` records the published source commit and file hashes.

No Windows client execution, gameplay replay, opponent matrix, pathfinding correctness claim, or win-rate benchmark is claimed. The simulator uses illustrative snapshots, not a full game engine. StarCraft is a Blizzard trademark; this is an unofficial educational project.
