###########################################################
#
# SAMPLE_APP mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the SAMPLE_APP configuration
set(SAMPLE_APP_MISSION_CONFIG_FILE_LIST
  sample_app_fcncodes.h
  sample_app_interface_cfg.h
  sample_app_mission_cfg.h
  sample_app_perfids.h
  sample_app_msg.h
  sample_app_msgdefs.h
  sample_app_msgstruct.h
  sample_app_tbl.h
  sample_app_tbldefs.h
  sample_app_tblstruct.h
  sample_app_topicids.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(SAMPLE_APP_CFGFILE_SRC_sample_app_interface_cfg "sample_app_eds_designparameters.h")
  set(SAMPLE_APP_CFGFILE_SRC_sample_app_tbldefs       "sample_app_eds_typedefs.h")
  set(SAMPLE_APP_CFGFILE_SRC_sample_app_tblstruct     "sample_app_eds_typedefs.h")
  set(SAMPLE_APP_CFGFILE_SRC_sample_app_msgdefs       "sample_app_eds_typedefs.h")
  set(SAMPLE_APP_CFGFILE_SRC_sample_app_msgstruct     "sample_app_eds_typedefs.h")
  set(SAMPLE_APP_CFGFILE_SRC_sample_app_fcncodes      "sample_app_eds_cc.h")

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(SAMPLE_APP_CFGFILE ${SAMPLE_APP_MISSION_CONFIG_FILE_LIST})
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
