#!/bin/sh
./sca_stop.sh
./sca_clear.sh
./sca_start.sh
printf "Scaler running...\n"
sleep 30s
./sca_stop.sh
printf "Finished.\n"