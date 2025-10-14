import os
import re

# 🔧 Configuration
folder_path = "/path/to/your/folder"
new_version = "2.0"
version_pattern = re.compile(r"(//|/\*)\s*File Version:\s*\d+\.\d+(\s*\*/)?")

def update_file_version(file_path):
    with open(file_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        if version_pattern.search(line):
            lines[i] = version_pattern.sub(r"\1 File Version: " + new_version + r"\2", line)
            break  # Only update the first match

    with open(file_path, "w", encoding="utf-8") as f:
        f.writelines(lines)

def process_folder(folder):
    for root, _, files in os.walk(folder):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".h"):
                full_path = os.path.join(root, file)
                update_file_version(full_path)
                print(f"Updated: {full_path}")

# 🚀 Run the script
process_folder(folder_path)
