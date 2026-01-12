import os
import re

folder_path = os.path.dirname(os.path.abspath(__file__))
new_version = "1.12.0"

version_pattern = re.compile(r"(Version:\s*)\d+\.\d+(\.\d+)?")

def update_file_header(file_path):
    with open(file_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        if version_pattern.search(line):
            lines[i] = version_pattern.sub(lambda m: m.group(1) + new_version, line)
            break  # Only update the first match

    with open(file_path, "w", encoding="utf-8") as f:
        f.writelines(lines)

def process_folder(folder):
    for root, _, files in os.walk(folder):
        for file in files:
            if file.endswith((".cpp", ".h", ".ino")):
                full_path = os.path.join(root, file)
                update_file_header(full_path)
                print(f"✅ Updated: {full_path}")

process_folder(folder_path)
