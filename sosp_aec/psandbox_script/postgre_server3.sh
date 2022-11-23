#!/bin/bash

postgres -D $PSANDBOX_POSTGRES_DIR/data/ --config-file=$PSANDBOX_POSTGRES_DIR/data/postgresql.conf &
sleep 5
$POSTGRES_SYSBENCH_DIR/bin/sysbench --pgsql-db=postgres --pgsql-user=$(whoami) --tables=1 --table-size=1000000 --threads=1 $POSTGRES_SYSBENCH_DIR/share/sysbench/oltp_update_index.lua cleanup >> /dev/null
$POSTGRES_SYSBENCH_DIR/bin/sysbench --pgsql-db=postgres --pgsql-user=$(whoami) --tables=1 --table-size=1000000 --threads=1 $POSTGRES_SYSBENCH_DIR/share/sysbench/oltp_update_index.lua prepare >> /dev/null

if [[ $1 == 1 ]]; then
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_postgre_cfg.yml --label test
else
  sudo ${PSP_DIR}/build/src/c++/apps/app/psp-app --cfg ${PSP_DIR}/sosp_aec/configs/base_psp_darc_postgre_cfg.yml --label test
fi
