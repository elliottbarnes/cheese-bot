// Group Members: Jacob Critch, Elliott Barnes

#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"


BWAPI::Position enemyBasePosition;
BWAPI::Unit scout, scout2;

bool enemyBaseFound = false;
bool scoutAssigned = false;
bool buildPylonNext = false;
bool enemyBuild = false;


StarterBot::StarterBot()
{

}

// Called when the bot starts!
void StarterBot::onStart()
{
    scout = nullptr;
    scout2 = nullptr;
    scoutAssigned = false;
    enemyBaseFound = false;
    buildPylonNext = false;
    enemyBuild = false;

    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(0);
    BWAPI::Broodwar->setFrameSkip(3);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    scoutWithIdleWorker();

    sendIdleWorkersToMinerals();

    initBuildOrder();

    rushBuildOrder();

    // Check if supply is low, if so, build some supply
    //buildAdditionalSupply();

    // Train more workers until we have our desired amount
    trainAdditionalWorkers();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot, and our group members' names
    drawDebugInformation();


}

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle() && unit != scout)
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    if (CheesePolicy::decide(strategyState()).trainWorker)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    if (BWAPI::Broodwar->self()->supplyUsed() >= BWAPI::Broodwar->self()->supplyTotal())
    {
        const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
        if (startedBuilding)
        {
            BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
        }
    }
}

// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Jacob Critch\nElliott Barnes");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit deadUnit)
{
    if (deadUnit == scout)
    {
        scout = nullptr;
        scoutAssigned = false; // Reassigned safely on the next frame.
    }
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{

}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text)
{
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{
    BWAPI::Broodwar->sendText("unit created: %s", unit->getType().getName().c_str());
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{

}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{

}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{

}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{

}

CheesePolicy::State StarterBot::strategyState() const
{
    const auto& units = BWAPI::Broodwar->self()->getUnits();
    return {
        Tools::CountUnitsOfType(BWAPI::Broodwar->self()->getRace().getWorker(), units),
        Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Pylon, units),
        Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Forge, units),
        enemyBaseFound,
        scoutAssigned && scout && scout->exists() && scout->canBuild() && !scout->isConstructing()
    };
}

void StarterBot::initBuildOrder()
{
    const auto plan = CheesePolicy::decide(strategyState());
    if (plan.homePylon)
    {
        if (Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Pylon))
            BWAPI::Broodwar->printf("Started Building Pylon at base");
    }
    else if (plan.homeForge)
    {
        if (Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Forge))
            BWAPI::Broodwar->printf("Started Building Forge at base");
    }
}

void StarterBot::rushBuildOrder()
{
    const auto plan = CheesePolicy::decide(strategyState());
    const int maxBuildRange = 32;
    if (plan.proxyPylon)
        scout->build(BWAPI::UnitTypes::Protoss_Pylon, BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Pylon, scout->getTilePosition(), maxBuildRange));
    // These are intentionally independent gates, as in the original strategy.
    // At two pylons both orders can be attempted; BWAPI decides whether they succeed.
    if (plan.cannon)
        scout->build(BWAPI::UnitTypes::Protoss_Photon_Cannon, BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Photon_Cannon, scout->getTilePosition(), maxBuildRange));
}

void StarterBot::scoutWithIdleWorker()
{
    if (!scoutAssigned || !scout || !scout->exists())
    {
        scout = nullptr;
        scoutAssigned = false;
        for (auto& unit : BWAPI::Broodwar->self()->getUnits())
        {
            if (unit->getType().isWorker() && unit->exists() && !unit->isConstructing())
            {
                scout = unit;
                scoutAssigned = true;
                if (enemyBaseFound) scout->move(enemyBasePosition);
                break;
            }
        }
    }
    if (!scoutAssigned || !scout || enemyBaseFound) return;

    for (auto tile : BWAPI::Broodwar->getStartLocations())
    {
        if (!BWAPI::Broodwar->isExplored(tile))
        {
            BWAPI::Position pos(tile);
            for (auto& unit : scout->getUnitsInRadius(400))
            {
                if (unit->getPlayer() == BWAPI::Broodwar->enemy())
                {
                    enemyBasePosition = pos;
                    enemyBaseFound = true;
                    scout->stop();
                    BWAPI::Broodwar->printf("Enemy base found!");
                    return;
                }
            }
            if (scout->getLastCommand().getTargetPosition() != pos) scout->move(pos);
            return;
        }
    }
}

bool StarterBot::currentlyConstructing()
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        if (unit->isBeingConstructed())
        {
            return true;
        }
    }
    return false;
}

bool StarterBot::currentlyConstructing(BWAPI::UnitType type)
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        if (unit->getType() == type && unit->isBeingConstructed())
        {
            return true;
        }
    }
    return false;
}

int StarterBot::countNumberOfType(BWAPI::UnitType type)
{
    int count = 0;
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        if (unit->getType() == type)
        {
            count += 1;
        }
    }
    return count;
}