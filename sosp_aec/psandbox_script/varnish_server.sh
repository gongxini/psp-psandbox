#!/bin/bash



if [[ $1 == 1 ]]; then
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_cfg_varnish.yml --label test
else
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_darc_cfg_varnish.yml --label test
fi

