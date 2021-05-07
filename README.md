# CannonRushBot

A Photon Cannon Rush Bot for StarCraft Broodwar

### CheeseBot Installation 

Here you will find step-by-step instructions to install and use CheeseBot

## Step 1: Install Prerequisites

CheeseBot is currently being maintained using Visual Studio 2019, which is required to open the project file and build the source code. Only use the exact versions of everything listed below.

**List of Prerequisites for Bot Compiling:**
  * [Visual Studio 2019](https://visualstudio.microsoft.com/vs/)
    * Only the C++ related packages are required to compile the bot
  * [CheeseBot](https://github.com/elliottbarnes/CannonRushBot) 
    * Source Code - clone this repo or download it as a .zip file
  * [Starcraft: Broodwar](https://starcraftbw.s3.us-east-2.amazonaws.com/Starcraft_1161.zip) (version 1.16.1)
    * Use this link to install SC, NOT the Blizzard launcher
    * Unzip this file into a folder (no installation required)
    * Don't run `StarCraft.exe` on its own, it probably won't work. You run the game with the `Chaoslauncher` program later, with instructions below
  * [BWAPI 4.4.0](https://github.com/bwapi/bwapi/releases/tag/v4.4.0) 
    * Download 4.4.0 stable Release, don't clone the GitHub repo

**Install Visual Studio**
  1. Install Visual Studio 2019 from the Microsoft website, which may take several minutes
  2. Only the C++ development options are required

**Install Starcraft**
  1. Install StarCraft: Broodwar version 1.16.1 (this is the only version that will work). 
  2. Connecting to battle.net or running any game patcher after this step will break the install, and you'll need to re-install Starcraft from scratch (Note: This refers to connecting to bnet within BW, not the modern bnet launcher program)

**Install BWAPI**
  1. Make sure you install BWAPI to a directory without any spaces in its name. 
  2. Either use the BWAPI exe installer, or extract the .7z file to the install directory
  3. Copy the files from BWAPI's StarCraft folder into your StarCraft install directory
  4. Copy the files from BWAPI's Windows folder into your StarCraft directory (required dll files)

**Windows Environment Variable**
  1. The StarterBot visual studio project makes use of a Windows Environment Variable to find the location of the BWAPI library files. This is done in so that the project can be compiled on any system without any changes to the project file itself.
  2. Open Start menu, type `env` and the option to edit environment variables appears
  3. Create a Windows Environment variable called `BWAPI_DIR`. The value of this variable should be the BWAPI install directory full path (ex: `c:\libraries\BWAPI_440`)
  
## Step 2: Compile BWAPI and UAlbertaBot

**Compile BWAPI 4.4.0**
   1. Open the `BWAPI_DIR/ExampleProjects.sln` VS solution file in VS2019
   2. For both the `BWAPILIB` and `BWAPIClient` projects: Right click on the project, choose `Properties` in the left menu, and change `Platform Toolset` in the right menu to `Visual Studio 2019 (v142)`. Be sure to do this for both `Release` and `Debug` Configuration, which are selected in the top-left corner. This step is necessary because BWAPI was originally created using VS2017. 
   3. Right click and `Build` the `BWAPILIB` and `BWAPIClient` projects. This will produce the required LIB files that UAlbertaBot will link against. Be sure to build in both `Release` and `Debug` mode, selected in the toolbar above.
   4. If done correctly, this should create two folders inside the BWAPI install directory: `Release` and `Debug`. Each of these folders should now contain `BWAPILIB.lib` and `BWAPIClient.lib`, which will be linked by the UAlbertaBot project in the next step via the Environment Variable value

**Compile StarterBot**
  1. Open `starterBot.sln` in Visual Studio 2019
  2. Select `Release` or `Debug` mode. The `Debug` version will run much slower, because it keeps track of all variables while running for debugging purposes. If you want to run the bot as fast as possible, select `Release`. `Debug` mode compiles faster, and is recommended for development.
  3. Right-click and `Build` the `UAlbertaBot` project (all projects will be built)
  4. The compiled executable file will go to the `StarterBot/bin` directory
  5. Depending on whether compiled in `Release` or `Debug` mode, the exe file generated will be `UAlbertaBot.exe` or `UAlberaBot_d.exe` respectively

## Step 3: Run StarterBot and Starcraft

In order to use a BWAPI-based bot with Starcraft, you must run Starcraft through the `Chaoslauncher` program that comes with BWAPI. Chaoslauncher injects the BWAPI .dll file into Starcraft so that it can communicate with your bot. The executable file for Chaoslauncher is `Chaoslauncher.exe` and is located in `BWAPI_DIR/Chaoslauncher/`. Chaoslauncher can be run from anywhere on your system, so it is fine to leave it in the BWAPI directory, but I recommend creating a shortcut to it on your taskbar since you will be using it quite often. There is also a multi-instance version which allows you to run multiple simultaneous instances of Starcraft.

1. Ensure `Chaoslauncher.exe` and `Starcraft.exe` are set up to run as Administrator. Right click the .exe file, select `Properties` > `Compatibility` > check `Run as Administrator`
2. Run `BWAPI_DIR/Chaoslauncher/Chaoslauncher.exe` 
4. Ensure that the `BWAPI 4.4.0 Injector Release` and `W-MODE` options are checked in Chaoslauncher
5. Run `StarterBot` one of two ways: 1) run from VS (by clicking `Local Windows Debugger` above)
6. The bot should now be running in its own console window, printing `Game table mapping not found` once per second. This will continue until Starcraft is run with BWAPI, at which point the bot will automatically connect to Starcraft and run
7. Click `Start` in Chaoslauncher to run StarCraft, StarterBot will connect when the game starts
8. The first time you do step 7, Chaoslauncher may complain that it doesn't know where Starcraft is installed. Click the `Settings` tab on top of the window and edit the `Installpath` directory.
9. Inside StarCraft, start a game using `Single Player` > `Expansion` > `Play Custom`, select any standard multiplayer map and use `Melee` settings to start. Note: The bot will not work with more than one opponent or in the single player campaign.

## Step 4. Development Cycle

Once you have everything up and running the first time, I recommend the following workflow:

1. Keep Chaoslauncher open while developing, there is no reason to close it
2. Edit the `BWAPI.ini` file to `AUTO_MENU` on so that you don't have to select Starcraft settings each time the game is run (see below)
3. After you change the code in VS, simply run the bot by clicking the `Local Windows Debugger` button next to the green arrow in VS, and then click 'Start' in Chaoslauncher. This will first run UAlbertaBot, and then launch Starcraft
4. Finally, both Starcraft and UAB need to be shut down before you can compile the bot again. You can do this by just closing the Starcraft window, which will automatically stop the `StarterBot.exe` process.

Dev Note: Manually navigating the Starcraft menus becomes very tedious when developing the bot and having to run it multiple times. You can configure the `BWAPI.ini` file to use the `AUTO_MENU` feature, which will use your input settings to automatically select which map and race to play for you. I recommend getting this to work before starting bot development. To edit the `BWAPI.ini` file, first select `BWAPI 4.4.0 Injector [RELEASE]` inside Chaoslauncher and then click the `Config` button on the right. This will open the `Starcraft/bwapi-data/BWAPI.ini` file in notepad.

Full details on the BWAPI config file [available here](https://github.com/bwapi/bwapi/wiki/Configuration)
