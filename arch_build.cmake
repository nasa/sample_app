###########################################################
#
# SAMPLE_APP platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the SAMPLE_APP configuration
set(SAMPLE_APP_PLATFORM_CONFIG_FILE_LIST
  sample_app_internal_cfg.h
  sample_app_platform_cfg.h
  sample_app_perfids.h
  sample_app_msgids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(SAMPLE_APP_CFGFILE ${SAMPLE_APP_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${SAMPLE_APP_CFGFILE}" NAME_WE)
  if (DEFINED SAMPLE_APP_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${SAMPLE_APP_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${SAMPLE_APP_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${SAMPLE_APP_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
