import {
  readFile,
  readdir,
  mkdir,
  rm,
  copyFile,
  writeFile,
  lstat,
} from "node:fs/promises";
import { createHash } from "node:crypto";
import { execFileSync } from "node:child_process";
const files = (await readdir("demo")).sort();
const allowed = new Set([
  "index.html",
  "style.css",
  "app.js",
  "engine.js",
  "board.js",
  "favicon.svg",
]);
if (!files.includes("index.html")) throw new Error("Demo entry point missing");
for (const file of files) {
  if (!allowed.has(file) || (await lstat(`demo/${file}`)).isSymbolicLink())
    throw new Error(`Unexpected demo asset: ${file}`);
  if (file.endsWith(".js"))
    execFileSync(process.execPath, ["--check", `demo/${file}`]);
}
await rm("dist", { recursive: true, force: true });
await mkdir("dist");
const manifest = {
  sourceCommit: execFileSync("git", ["rev-parse", "HEAD"], {
    encoding: "utf8",
  }).trim(),
  files: {},
};
for (const file of files) {
  await copyFile(`demo/${file}`, `dist/${file}`);
  manifest.files[file] = createHash("sha256")
    .update(await readFile(`dist/${file}`))
    .digest("hex");
}
await writeFile("dist/build.json", JSON.stringify(manifest, null, 2) + "\n");
await writeFile("dist/.nojekyll", "");
console.log(`Verified and built ${files.length} static demo files.`);
