// PostToolUse: remind to regenerate arginfo after editing the stub.
let s = "";
process.stdin.on("data", (d) => (s += d)).on("end", () => {
  let p = "";
  try { p = (JSON.parse(s).tool_input || {}).file_path || ""; } catch {}
  if (/treesitter\.stub\.php$/.test(p.split("\\").join("/"))) {
    console.log(JSON.stringify({
      hookSpecificOutput: {
        hookEventName: "PostToolUse",
        additionalContext:
          "treesitter.stub.php changed: regenerate treesitter_arginfo.h (use the /regen-arginfo skill) and keep treesitter.c in sync.",
      },
    }));
  }
});
