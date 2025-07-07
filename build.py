import os
import sys
import subprocess
from pathlib import Path
import platform
import shutil

def is_windows():
    return platform.system() == "Windows"

def is_linux():
    return platform.system() == "Linux"

def get_make_config():
    make_cmd = shutil.which("make")
    if is_windows():
        make_cmd = shutil.which("make") or shutil.which("wsl make")

    makefile_path = script_dir / "Makefile"
    if not makefile_path.is_file():
        print(f" No Makefile found at {makefile_path}. Did Premake generate it correctly?")
        sys.exit(1)
    return make_cmd

def clean_build(script_dir):
    make_cmd = get_make_config(script_dir)
    print("Running clean.")
    try:
        subprocess.run([make_cmd, "clean"], check=True)
        print("✅ Clean successful.")
    except subprocess.CalledProcessError:
        print("❌ Clean failed.")
        sys.exit(1)

def build_project(script_dir, build_cmd):
    build_script = script_dir / "scripts" / "build.lua"

    # Choose premake executable based on OS
    if is_windows():
        premake_exe = script_dir / "scripts" / "premake5_win.exe"
    elif is_linux():
        premake_exe = script_dir / "scripts" / "premake5_lin"
    else:
        print("Unsupported OS.")
        sys.exit(1)

    # Check if the premake executable exists
    if not premake_exe.is_file():
        print(f"Error: Premake executable not found at {premake_exe}")
        sys.exit(1)

    # Construct the command
    cmd = [
        str(premake_exe),
        f"--file={build_script}",
        "gmake"
    ]

    print("Generating project files...")
    try:
        subprocess.run(cmd, check=True)
        print("✅ Project files generated successfully.")
    except subprocess.CalledProcessError:
        print("❌ Error generating project files.")
        sys.exit(1)

    make_cmd = get_make_config(script_dir)
    print("Build make.")
    try:
        if build_cmd == "NONE":
            subprocess.run([make_cmd], check=True)
        else:
            subprocess.run([make_cmd, build_cmd], check=True)
        print("✅ Build successful.")
    except subprocess.CalledProcessError:
        print("❌ Build failed.")
        sys.exit(1)

def main():
    script_dir = Path(__file__).resolve().parent
    if len(sys.argv) > 1:
        if sys.argv[1].lower() == "clean":
            clean_build(script_dir)
        elif sys.argv[1].lower() == "debug" or sys.argv[1].lower() == "release":
            build_project(script_dir, sys.argv[1].lower())
    else:
        build_project(script_dir, "NONE")

if __name__ == "__main__":
    main()
