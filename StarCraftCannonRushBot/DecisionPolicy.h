#pragma once

// Pure decision gates extracted from StarterBot. BWAPI still validates every order.
namespace CheesePolicy {
struct State {
    int workers;
    int pylons;
    int forges;
    bool enemyFound;
    bool scoutReady;
};
struct Decisions {
    bool homePylon;
    bool homeForge;
    bool proxyPylon;
    bool cannon;
    bool trainWorker;
};
inline Decisions decide(const State& state) {
    const bool rush = state.enemyFound && state.scoutReady;
    return {
        state.pylons < 1,
        state.pylons >= 1 && state.forges < 1,
        rush && state.forges >= 1 && state.pylons < 3,
        rush && state.pylons >= 2,
        state.workers < 6
    };
}
}
