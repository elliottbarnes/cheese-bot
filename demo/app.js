import { decisions, SCENARIOS } from "./engine.js";
const $ = (id) => document.getElementById(id),
  ctx = $("schematic").getContext("2d");
const descriptions = [
  ["homePylon", "Home pylon", "No pylon is counted yet."],
  ["homeForge", "Home forge", "A pylon exists, but no forge does."],
  [
    "proxyPylon",
    "Forward pylon",
    "Enemy known, scout ready, forge present, fewer than three pylons.",
  ],
  ["cannon", "Photon cannon", "Enemy known, scout ready, at least two pylons."],
  [
    "trainWorker",
    "Train worker",
    "Fewer than six workers are owned. Depot availability is checked by BWAPI.",
  ],
];
let selected = 0;
function read() {
  const count = (id) => {
    const value = Math.max(
      0,
      Math.min(20, Math.trunc(Number($(id).value) || 0)),
    );
    $(id).value = value;
    return value;
  };
  return {
    workers: count("workers"),
    pylons: count("pylons"),
    forges: $("forge").checked ? 1 : 0,
    enemyFound: $("enemy").checked,
    scoutReady: $("scout").checked,
  };
}
function snapshot(index) {
  selected = index;
  const s = SCENARIOS[index];
  $("workers").value = s.workers;
  $("pylons").value = s.pylons;
  $("forge").checked = !!s.forges;
  $("enemy").checked = s.enemyFound;
  $("scout").checked = s.scoutReady;
  render();
}
function text(value, x, y, size = 13, color = "#aab5c6") {
  ctx.fillStyle = color;
  ctx.font = `${size}px system-ui`;
  ctx.textAlign = "center";
  ctx.fillText(value, x, y);
}
function polygon(x, y, r, sides, color) {
  ctx.beginPath();
  for (let i = 0; i < sides; i++) {
    const angle = (Math.PI * 2 * i) / sides - Math.PI / 2;
    const px = x + Math.cos(angle) * r,
      py = y + Math.sin(angle) * r;
    if (i === 0) ctx.moveTo(px, py);
    else ctx.lineTo(px, py);
  }
  ctx.closePath();
  ctx.strokeStyle = color;
  ctx.lineWidth = 2;
  ctx.stroke();
}
function render() {
  const s = read(),
    plan = decisions(s),
    requests = Object.entries(plan).filter(([, v]) => v).length;
  $("scenario-title").textContent =
    selected < 0 ? "Your world state" : SCENARIOS[selected].title;
  $("status").textContent =
    selected < 0
      ? `${requests} ${requests === 1 ? "request" : "requests"} enabled by this snapshot. Adjust a count or condition to inspect the rules.`
      : SCENARIOS[selected].detail;
  document
    .querySelectorAll("[data-step]")
    .forEach((b) =>
      b.setAttribute(
        "aria-pressed",
        String(Number(b.dataset.step) === selected),
      ),
    );
  $("decisions").replaceChildren(
    ...descriptions.map(([key, title, detail]) => {
      const card = document.createElement("article");
      card.className = `decision${plan[key] ? " active" : ""}`;
      const flag = document.createElement("span");
      flag.textContent = plan[key] ? "REQUEST ENABLED" : "GATE CLOSED";
      const heading = document.createElement("strong");
      heading.textContent = title;
      const p = document.createElement("p");
      p.textContent = detail;
      card.append(flag, heading, p);
      return card;
    }),
  );
  const mobile = $("schematic").clientWidth < 500;
  const width = mobile ? 400 : 800;
  const hx = mobile ? 77 : 155,
    sx = mobile ? 198 : 395,
    ex = mobile ? 320 : 638;
  $("schematic").width = width;
  $("schematic").height = 370;
  ctx.fillStyle = "#10151e";
  ctx.fillRect(0, 0, width, 370);
  ctx.fillStyle = "#26303b";
  for (let x = 20; x < width; x += 32)
    for (let y = 20; y < 370; y += 32) ctx.fillRect(x, y, 1, 1);
  ctx.strokeStyle = "#3c4653";
  ctx.setLineDash([5, 7]);
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(hx + (mobile ? 42 : 85), 185);
  ctx.lineTo(ex - (mobile ? 42 : 68), 185);
  ctx.stroke();
  ctx.setLineDash([]);
  polygon(hx, 177, mobile ? 28 : 49, 6, "#91bfff");
  text("HOME", hx, 180, mobile ? 11 : 15, "#edf1f6");
  text(`${s.workers} workers`, hx, 262, mobile ? 11 : 13);
  text(`${s.pylons} pylons · ${s.forges} forge`, hx, 286, mobile ? 10 : 13);
  for (let i = 0; i < Math.min(s.workers, 10); i++) {
    const angle = (i / 10) * Math.PI * 2;
    ctx.fillStyle = "#91bfff";
    ctx.beginPath();
    ctx.arc(
      hx + Math.cos(angle) * (mobile ? 42 : 76),
      177 + Math.sin(angle) * (mobile ? 42 : 76),
      4,
      0,
      Math.PI * 2,
    );
    ctx.fill();
  }
  polygon(sx, 185, mobile ? 11 : 15, 3, s.scoutReady ? "#f0cd7e" : "#657184");
  text(
    s.scoutReady ? "SCOUT READY" : "SCOUT UNAVAILABLE",
    sx,
    232,
    mobile ? 9 : 12,
  );
  text("Scouting / forward orders", sx, 105, mobile ? 10 : 12);
  polygon(ex, 177, mobile ? 32 : 53, 4, s.enemyFound ? "#f18f92" : "#465264");
  text(
    s.enemyFound ? "ENEMY" : "UNKNOWN",
    ex,
    180,
    mobile ? 10 : 14,
    s.enemyFound ? "#f5babd" : "#8b98ab",
  );
  text(
    s.enemyFound ? "Location known" : "Still searching",
    ex,
    262,
    mobile ? 11 : 13,
  );
  if (plan.proxyPylon) {
    polygon(mobile ? 245 : 575, 315, 13, 3, "#f0cd7e");
    text("PYLON REQUEST", mobile ? 245 : 575, 345, mobile ? 9 : 11, "#f0cd7e");
  }
  if (plan.cannon) {
    polygon(mobile ? 345 : 707, 315, 13, 5, "#f0cd7e");
    text("CANNON REQUEST", mobile ? 345 : 707, 345, mobile ? 9 : 11, "#f0cd7e");
  }
  text(
    "SCHEMATIC · positions and timing are illustrative",
    width / 2,
    31,
    mobile ? 9 : 11,
    "#8391a5",
  );
}
for (const id of ["workers", "pylons", "forge", "enemy", "scout"])
  $(id).addEventListener("change", () => {
    selected = -1;
    render();
  });
for (const button of document.querySelectorAll("[data-step]"))
  button.addEventListener("click", () => snapshot(Number(button.dataset.step)));
$("reset").addEventListener("click", () => snapshot(0));
window.addEventListener("resize", render);
render();
