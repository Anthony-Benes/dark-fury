import json
import platform
import os
import sys
import subprocess
from pathlib import Path
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

def parse_premake_json(json_path):
    with open(json_path, 'r') as f:
        data = json.load(f)
    return data["projects"]

def generate_root_cmakelists(script_dir, projects):
    cmake_file = script_dir / "CMakeLists.txt"
    with open(cmake_file, "w") as f:
        f.write(f"""# Autogenerated CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(DarkFury CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(DEFINED ENV{{VULKAN_SDK}})
    set(VULKAN_SDK $ENV{{VULKAN_SDK}})
else()
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set!")
endif()

include_directories(${{VULKAN_SDK}}/include)

""")
        for project in projects:
            f.write(f'include(scripts/{project["name"]}_CMakeLists.txt)\n')

def generate_project_cmakelists(script_dir, project):
    name = project["name"]
    includes = project.get("includedirs", [])
    sources = project.get("files", [])
    links = project.get("links", [])
    cmake_file = script_dir / "scripts" / f"{name}_CMakeLists.txt"
    with open(cmake_file, "w") as f:
        f.write(f"""# CMake for {name}
add_library({name} SHARED
    {" ".join(sources)}
)

target_link_libraries({name} PRIVATE {" ".join(links)})

target_include_directories({name} PUBLIC
    ${{VULKAN_SDK}}/include
    ${{CMAKE_CURRENT_SOURCE_DIR}}/src
    {" ".join(includes)}
)
""")

def generate_cmake(script_dir):
    if is_windows():
        return
    premake_json = script_dir / "scripts" / "premake.json"
    if not premake_json.exists():
        print("❌ premake.json not generated.")
        sys.exit(1)
    print("📦 Parsing premake.json and generating CMakeLists.txt files...")
    projects = parse_premake_json(premake_json)
    generate_root_cmakelists(script_dir, projects)
    for project in projects:
        generate_project_cmakelists(script_dir, project)
    print("✅ Generated CMakeLists.txt files.")
    out_dir = script_dir / "out"
    out_dir.mkdir(exist_ok=True)
    print("⚙️  Running CMake to emit compile_commands.json...")
    subprocess.run(["cmake", "-S", ".", "-B", str(out_dir)], check=True)

def clean_build(script_dir):
    make_cmd = get_make_command(script_dir)
    print("🧹 Running make clean...")
    try:
        subprocess.run(make_cmd + ["clean"], check=True)
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

    print(f"📦 Generating project files with Premake ({premake_action})...")
    try:
        subprocess.run(premake_cmd, check=True)
        print("✅ Premake completed.")
    except subprocess.CalledProcessError:
        print("❌ Premake failed.")
        sys.exit(1)

    if build_target == "msvs2022":
        print("📝 Visual Studio project generated. No make build needed.")
        return

    if is_linux():
        generate_cmake(script_dir)

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
