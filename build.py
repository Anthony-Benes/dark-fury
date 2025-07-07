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

def get_make_command(script_dir):
    make_cmd = shutil.which("make")

    if is_windows() and not make_cmd:
        # Try WSL fallback
        if shutil.which("wsl"):
            return ["wsl", "make"]
        else:
            print("❌ No make or WSL found. Please install Make or WSL.")
            sys.exit(1)

    if not make_cmd:
        print("❌ No make command found.")
        sys.exit(1)

    makefile_path = script_dir / "Makefile"
    if not makefile_path.is_file():
        print(f"❌ No Makefile found at {makefile_path}. Did Premake generate it?")
        sys.exit(1)

    return [make_cmd]

def clean_build(script_dir):
    make_cmd = get_make_command(script_dir)
    print("🧹 Running make clean...")
    try:
        subprocess.run([make_cmd, "clean"], check=True)
        print("✅ Clean successful.")
    except subprocess.CalledProcessError:
        print("❌ Clean failed.")
        sys.exit(1)

def build_project(script_dir, build_target="NONE"):
    build_script = script_dir / "scripts" / "build.lua"

    # Select premake executable
    if is_windows():
        premake_exe = script_dir / "scripts" / "premake5_win.exe"
    elif is_linux():
        premake_exe = script_dir / "scripts" / "premake5_lin"
    else:
        print("❌ Unsupported OS.")
        sys.exit(1)

    if not premake_exe.is_file():
        print(f"❌ Premake executable not found at {premake_exe}")
        sys.exit(1)

    if build_target == "msvs2022":
        if not is_windows():
            print("❌ Visual Studio generation is only supported on Windows.")
            sys.exit(1)
        premake_action = "vs2022"
    else:
        premake_action = "gmake"

    # Run premake
    premake_cmd = [
        str(premake_exe),
        f"--file={build_script}",
        premake_action
    ]

    print("📦 Generating project files with Premake ({premake_action})...")
    try:
        subprocess.run(premake_cmd, check=True)
        print("✅ Premake completed.")
    except subprocess.CalledProcessError:
        print("❌ Premake failed.")
        sys.exit(1)

    if build_target == "msvs2022":
        print("📝 Visual Studio project generated. No make build needed.")
        return

    # Build with make
    make_cmd = get_make_command(script_dir)
    print("🔨 Building with make...")
    try:
        args = make_cmd if build_target == "NONE" else make_cmd + [build_target]
        subprocess.run(args, check=True)
        print("✅ Build successful.")
    except subprocess.CalledProcessError:
        print("❌ Build failed.")
        sys.exit(1)

def main():
    script_dir = Path(__file__).resolve().parent

    if len(sys.argv) > 1:
        arg = sys.argv[1].lower()
        if arg == "clean":
            clean_build(script_dir)
        elif arg in ("debug", "release", "msvs2022"):
            build_project(script_dir, arg)
        else:
            print(f"⚠️ Unknown argument: {arg}")
            print("Usage: python build.py [clean|debug|release|msvs2022]")
            sys.exit(1)
    else:
        build_project(script_dir)

if __name__ == "__main__":
    main()
