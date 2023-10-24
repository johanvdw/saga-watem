# The version number.
set (SAGA_WATEM_VERSION_MAJOR 2023)
set (SAGA_WATEM_VERSION_MINOR 10)
set (SAGA_WATEM_VERSION_PATCH 23)

# get git commit hash (or dirty if git is not installed)

# load helper scripts
include(shared_cmake/scripts/load_git_variables.cmake)
include(shared_cmake/scripts/gen_version_string.cmake)

# load git variables
load_git_variables(GIT_BRANCH GIT_COUNT GIT_COMMIT)

# define version strings
gen_version_string(SAGA_WATEM_VERSION_STRING SAGA_WATEM_VERSION_FULL_STRING
	${SAGA_WATEM_VERSION_MAJOR} ${SAGA_WATEM_VERSION_MINOR} ${SAGA_WATEM_VERSION_PATCH}
	${GIT_COUNT} ${GIT_COMMIT})

message("GIT_BRANCH:         ${GIT_BRANCH}")
message("VERSON_STRING:      ${SAGA_WATEM_VERSION_STRING}")
message("VERSON_FULL_STRING: ${SAGA_WATEM_VERSION_FULL_STRING}")
