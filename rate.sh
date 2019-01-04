#!/bin/sh
./sca_stop.sh
./sca_clear.sh
./sca_start.sh
printf "Scaler running...\n"
sleep 10s
./sca_stop.sh
printf "Finished.\n"o