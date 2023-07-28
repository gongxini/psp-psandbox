#!/bin/bash

postgres -D $PSANDBOX_POSTGRES_DIR/data/ --config-file=$PSANDBOX_POSTGRES_DIR/data/postgresql.conf &
sleep 5
sysbench --pgsql-db=postgres --pgsql-user=root --tables=1 --table-size=1000 --threads=1 $SYSBEN_DIR/oltp_update_index.lua cleanup >> /dev/null
sysbench --pgsql-db=postgres --pgsql-user=root --tables=1 --table-size=1000 --threads=1 $SYSBEN_DIR/oltp_update_index.lua prepare >> /dev/null

if [[ $1 == 1 ]]; then
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_postgre_cfg.yml --label test
else
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_darc_postgre_cfg.yml --label test
fi
