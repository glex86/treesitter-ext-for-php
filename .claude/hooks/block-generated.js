// PreToolUse: refuse edits to generated build files.
let s = "";
process.stdin.on("data", (d) => (s += d)).on("end", () => {
  let p = "";
  try { p = (JSON.parse(s).tool_input || {}).file_path || ""; } catch {}
  p = p.split("\\").join("/");
  const generated = [
    /\/(configure|configure\.ac~|config\.status|config\.h|config\.h\.in|config\.log|config\.nice|libtool|Makefile|Makefile\.objects|Makefile\.fragments|run-tests\.php)$/,
    /\/treesitter_arginfo\.h$/,
    /\/(autom4te\.cache|\.libs|modules|build)\//,
    /\/treesitter\.(dep|la|lo)$/,
  ];
  if (generated.some((r) => r.test(p))) {
    const hint = /arginfo/.test(p)
      ? "Edit treesitter.stub.php and run the /regen-arginfo skill instead."
      : "This is a generated build artifact; edit config.m4 / sources and rebuild instead.";
    console.error("Blocked: " + p + " is generated. " + hint);
    process.exit(2);
  }
});
