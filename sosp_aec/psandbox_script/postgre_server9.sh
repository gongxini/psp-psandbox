#!/bin/bash

postgres -D $PSANDBOX_POSTGRES_DIR/data/ --config-file=$PSANDBOX_POSTGRES_DIR/data/postgresql.conf &
sleep 5
cp gendata.pl $PSANDBOX_POSTGRES_DIR
cd $PSANDBOX_POSTGRES_DIR && ./gendata6.pl
cd -
./sosp_aec/psandbox_script/create6.sh

if [[ $1 == 1 ]]; then
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_postgre_cfg.yml --label test
else
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_darc_postgre_cfg.yml --label test
fi
