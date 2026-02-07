# CPack configuration
include(GNUInstallDirs)

# Install executable
install(TARGETS ${CONST_TARGET_NAME}
  RUNTIME DESTINATION bin
  COMPONENT Runtime
)

# CPack settings - must be set before include(CPack)
# Set ALL required CPack variables explicitly
set(CPACK_PROJECT_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_GENERATOR "TGZ;ZIP")

# Package file name format
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")

# Debug: print ALL CPack variables to verify they are set
message(STATUS "=== CPack Configuration ===")
message(STATUS "CPACK_PROJECT_NAME:            ${CPACK_PROJECT_NAME}")
message(STATUS "CPACK_PACKAGE_NAME:            ${CPACK_PACKAGE_NAME}")
message(STATUS "CPACK_PACKAGE_VERSION:         ${CPACK_PACKAGE_VERSION}")
message(STATUS "CPACK_GENERATOR:               ${CPACK_GENERATOR}")
message(STATUS "==========================")

include(CPack)