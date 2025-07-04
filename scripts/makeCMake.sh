#!/usr/bin/bash

# Get the script's directory (where this script is located)
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
SCRIPTS_DIR="${ROOT_DIR}/scripts"
CMAKE_FILE="${ROOT_DIR}/CMakeLists.txt"
BUILD_LUA="${SCRIPTS_DIR}/build.lua"
PREMAKE_JSON="${SCRIPTS_DIR}/premake.json"

# Ensure the scripts directory exists (for the generated files)
mkdir -p "${SCRIPTS_DIR}"

# Ensure Premake is invoked with the proper flag to generate JSON output
premake_cmd="$SCRIPT_DIR/premake5_lin --file=$SCRIPT_DIR/build.lua"
$premake_cmd gmake

# Function to fetch project names from premake.json
get_project_names_from_json() {
    # Extract project names from the JSON file using Python
    projects=$(python3 -c "import json; data = json.load(open('$PREMAKE_JSON')); print(' '.join([project['name'] for project in data['projects']]))")
    echo "$projects"
}

# Function to fetch include directories for a specific project from premake.json
get_includedirs_from_json() {
    project=$1
    includedirs=$(python3 -c "
import json
data = json.load(open('$PREMAKE_JSON'))
project = next(p for p in data['projects'] if p['name'] == '$project')
print(' '.join(project.get('includedirs', [])))
")
    echo "$includedirs"
}

# Function to fetch source files for a specific project from premake.json
get_source_files_from_json() {
    project=$1
    sources=$(python3 -c "
import json
data = json.load(open('$PREMAKE_JSON'))
project = next(p for p in data['projects'] if p['name'] == '$project')
print(' '.join(project.get('files', [])))
")
    echo "$sources"
}

# Function to fetch linked libraries for a specific project from premake.json
get_link_libraries_from_json() {
    project=$1
    links=$(python3 -c "
import json
data = json.load(open('$PREMAKE_JSON'))
project = next(p for p in data['projects'] if p['name'] == '$project')
print(' '.join(project.get('links', [])))
")
    echo "$links"
}

# Function to create the main CMakeLists.txt content
generate_main_cmakelists() {
    projects=$(get_project_names_from_json)

    cat <<EOF > "${CMAKE_FILE}"
# Minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Define the project name and language
project(DarkFury CXX)

# Specify the C++ standard version (matches Premake setup)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify Vulkan SDK include directory
if(DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK \$ENV{VULKAN_SDK})
else()
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set!")
endif()

# Include Vulkan headers
include_directories(\${VULKAN_SDK}/include)

# Add subdirectories for each project dynamically
EOF

    # Add each project as a subdirectory
    for project in $projects; do
        echo "include(scripts/${project}_CMakeLists.txt)" >> "${CMAKE_FILE}"
    done

    # Generate compile_commands.json by default
    echo "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)" >> "${CMAKE_FILE}"

    # Configure additional build options (optional)
    cat <<EOF >> "${CMAKE_FILE}"
# Configure additional build options (optional)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring for Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring for Release")
endif()

EOF
}

# Function to create the CMakeLists.txt for each project (dynamically)
generate_project_cmakelists() {
    projects=$(get_project_names_from_json)

    for project in $projects; do
        includes=$(get_includedirs_from_json "$project")
        sources=$(get_source_files_from_json "$project")
        links=$(get_link_libraries_from_json "$project")

        project_cmake_file="${SCRIPTS_DIR}/${project}_CMakeLists.txt"

        cat <<EOF > "$project_cmake_file"
# Project Setup for $project
add_library($project SHARED
    ${sources}
)

# Link libraries
target_link_libraries($project PRIVATE ${links})

# Include directories for $project
target_include_directories($project PUBLIC
    \${VULKAN_SDK}/include
    \${CMAKE_CURRENT_SOURCE_DIR}/src
    ${includes}
)
EOF

    done
}

# Generate the CMakeLists.txt files
generate_main_cmakelists
generate_project_cmakelists

echo "CMakeLists.txt files generated successfully!"
