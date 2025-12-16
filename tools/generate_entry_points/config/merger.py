import re

filename = "function_overrides.json5"


original_text = ""
# --- Step 1: load parsed data ---
with open(filename, "r") as f:
    lines = f.readlines()

enhanced_lines = [x for x in lines if re.search(r'"vk.*{.*}', x) ]
enhanced_lines = {re.match(r'\s*"([^"]+)"', x).group(1): x for x in enhanced_lines}

output_lines = []
skipping = False
for line in lines:
    if re.match(r'\s*"enhanced": {', line):
        skipping = True

    if skipping:
        if re.match(r'\s*},', line):
            skipping = False
        continue

    match = re.match(r'\s*"(vk[^"]+)",', line)
    if match:
        func_name = match.group(1)
        if func_name in enhanced_lines:
            line = enhanced_lines[func_name]
        else:
            line = line.replace(",", ": {},")
    output_lines.append(line)


# Remove old enhanced dictionaries from the file


with open(filename, "w") as f:
    f.writelines(output_lines)
