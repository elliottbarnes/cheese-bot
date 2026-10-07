import test from "node:test";
import assert from "node:assert/strict";
import { mkdirSync } from "node:fs";
import { execFileSync } from "node:child_process";
import { decisions, SCENARIOS } from "../demo/engine.js";
test("browser policy matches the actual C++ policy across 160 world snapshots", () => {
  mkdirSync("build", { recursive: true });
  execFileSync(process.env.CXX || "c++", [
    "-std=c++17",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-pedantic",
    "tests/policy.cpp",
    "-o",
    "build/policy-test",
  ]);
  execFileSync("build/policy-test");
  const rows = execFileSync("build/policy-test", ["--table"], {
    encoding: "utf8",
  })
    .trim()
    .split("\n");
  assert.equal(rows.length, 160);
  for (const row of rows) {
    const [workers, pylons, forges, enemyFound, scoutReady, ...expected] = row
      .split(",")
      .map(Number);
    assert.deepEqual(
      Object.values(
        decisions({
          workers,
          pylons,
          forges,
          enemyFound: !!enemyFound,
          scoutReady: !!scoutReady,
        }),
      ).map(Number),
      expected,
      row,
    );
  }
});
test("two pylons intentionally permits overlapping requests, while three closes the pylon gate", () => {
  assert.deepEqual(decisions(SCENARIOS[3]), {
    homePylon: false,
    homeForge: false,
    proxyPylon: true,
    cannon: true,
    trainWorker: false,
  });
  assert.equal(decisions(SCENARIOS[4]).proxyPylon, false);
});
test("enemy discovery and scout readiness independently gate every forward request", () => {
  for (const [enemyFound, scoutReady] of [
    [false, true],
    [true, false],
    [false, false],
  ]) {
    const p = decisions({ ...SCENARIOS[3], enemyFound, scoutReady });
    assert.equal(p.proxyPylon, false);
    assert.equal(p.cannon, false);
  }
});
test("invalid world counts are rejected rather than silently corrupting the schematic", () => {
  for (const workers of [-1, NaN, 2.5, 21])
    assert.throws(() => decisions({ ...SCENARIOS[0], workers }), RangeError);
});
