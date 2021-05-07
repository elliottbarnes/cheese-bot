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
    sendIdleWorkersToMinerals();

    scoutWithIdleWorker();

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
    const int workersWanted = 6;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
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
        const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
        for (auto& unit : myUnits)
        {
            // Check the unit type, if it is an idle worker, then we want to send it somewhere
            if (unit->getType().isWorker())
            {
                scout = unit;
                scout->move(enemyBasePosition);
                BWAPI::Broodwar->printf("Scout died, new scout has been selected! Moving it to enemy position");
            }
        }
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

void StarterBot::initBuildOrder()
{
    if (countNumberOfType(BWAPI::UnitTypes::Protoss_Pylon) < 1)
    {
        if (BWAPI::Broodwar->self()->supplyTotal() >= BWAPI::UnitTypes::Protoss_Pylon.supplyRequired() && Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Pylon))
        {
            BWAPI::Broodwar->printf("Started Building Pylon at base");
        }
    }
    else if (countNumberOfType(BWAPI::UnitTypes::Protoss_Forge) < 1)
    {
        if (BWAPI::Broodwar->self()->supplyTotal() >= BWAPI::UnitTypes::Protoss_Forge.supplyRequired() && Tools::BuildBuilding(BWAPI::UnitTypes::Protoss_Forge))
        {
            BWAPI::Broodwar->printf("Started Building Forge at base");
        }
    }
}

void StarterBot::rushBuildOrder()
{
    int maxBuildRange = 32;
    if (enemyBaseFound)
    {
        if (scoutAssigned && scout->canBuild() && !scout->isConstructing()) // having trouble checking if he already has an order
        {
            if (BWAPI::Broodwar->self()->supplyTotal() >= BWAPI::UnitTypes::Protoss_Pylon.supplyRequired() && countNumberOfType(BWAPI::UnitTypes::Protoss_Forge) >= 1 && countNumberOfType(BWAPI::UnitTypes::Protoss_Pylon) < 3)
            {
                scout->build(BWAPI::UnitTypes::Protoss_Pylon, BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Pylon, scout->getTilePosition(), maxBuildRange));
                if (scout->isConstructing())
                {
                    BWAPI::Broodwar->printf("Started Building Pylon at enemy base");
                }

            }
            if (BWAPI::Broodwar->self()->supplyTotal() >= BWAPI::UnitTypes::Protoss_Forge.supplyRequired() && countNumberOfType(BWAPI::UnitTypes::Protoss_Pylon) >= 2)
            {

                scout->build(BWAPI::UnitTypes::Protoss_Photon_Cannon, BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Photon_Cannon, scout->getTilePosition(), maxBuildRange));
                /*
                if (scout->isConstructing())
                {
                    BWAPI::Broodwar->printf("Started Building Cannon at enemy base");
                }
                */
            }
        }
    }
}

void StarterBot::scoutWithIdleWorker()
{
    if (enemyBaseFound) { return; }
    else if (!scoutAssigned)
    {
        const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
        for (auto& unit : myUnits)
        {
            // Check the unit type, if it is an idle worker, then we want to send it somewhere
            if (unit->getType().isWorker() && unit->isIdle())
            {
                scout = unit;
                scoutAssigned = true;
            }
        }
    }
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
            auto command = scout->getLastCommand();
            if (command.getTargetPosition() == pos) { return; }
            scout->move(pos);
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