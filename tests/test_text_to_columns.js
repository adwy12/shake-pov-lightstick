const assert = require("assert");
const { execFileSync } = require("child_process");

const output = execFileSync("node", ["tools/text_to_columns.js", "--json", "HI"], {
  encoding: "utf8",
});

const parsed = JSON.parse(output);

assert.strictEqual(parsed.text, "HI");
assert.strictEqual(parsed.columns.length, 13);
assert.deepStrictEqual(parsed.columns.slice(0, 6), [0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00]);
assert.strictEqual(parsed.hex, "7F0808087F0000417F41000000");

console.log("test_text_to_columns: PASS");
