// Mirrors the pure C++ DecisionPolicy.h; tests compare all 160 bounded snapshots.
export function decisions(state) {
  for (const key of ["workers", "pylons", "forges"])
    if (!Number.isInteger(state[key]) || state[key] < 0 || state[key] > 20)
      throw new RangeError(`Invalid ${key}`);
  const rush = Boolean(state.enemyFound && state.scoutReady);
  return {
    homePylon: state.pylons < 1,
    homeForge: state.pylons >= 1 && state.forges < 1,
    proxyPylon: rush && state.forges >= 1 && state.pylons < 3,
    cannon: rush && state.pylons >= 2,
    trainWorker: state.workers < 6,
  };
}
export const SCENARIOS = [
  {
    title: "The opening",
    detail:
      "Four workers, no structures. Request a home pylon and keep training toward six workers.",
    workers: 4,
    pylons: 0,
    forges: 0,
    enemyFound: false,
    scoutReady: true,
  },
  {
    title: "The foundation",
    detail:
      "A pylon exists. The next home request is a forge while scouting continues.",
    workers: 5,
    pylons: 1,
    forges: 0,
    enemyFound: false,
    scoutReady: true,
  },
  {
    title: "Forward power",
    detail:
      "Enemy located, forge present, scout available. Request a pylon near the scout.",
    workers: 6,
    pylons: 1,
    forges: 1,
    enemyFound: true,
    scoutReady: true,
  },
  {
    title: "Overlapping requests",
    detail:
      "At two pylons, the source can request both another forward pylon and a cannon in one frame.",
    workers: 6,
    pylons: 2,
    forges: 1,
    enemyFound: true,
    scoutReady: true,
  },
  {
    title: "Cannon pressure",
    detail:
      "At three pylons the forward-pylon gate closes. Cannon requests continue while the scout is ready.",
    workers: 6,
    pylons: 3,
    forges: 1,
    enemyFound: true,
    scoutReady: true,
  },
];
