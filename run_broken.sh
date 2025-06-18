#!/usr/bin/env bash
set -euo pipefail

TOTAL_NODES=100
GROUPS=5
RUN_SECS=60
MODES=gossip
BROKEN_LIST=(10 20 30 40 50 60 70 80 90)

cmake -S . -B build && cmake --build build -j"$(nproc)"

stop_random_nodes () {
  local KILL=$1
  docker ps --format '{{.Names}}' |
  grep '^ds-node' |
  shuf            | head -n "$KILL" |
  xargs -r docker kill &>/dev/null
}

one_round () {
  local MODE=$1  BROKEN=$2  TAG="${MODE}_broken${BROKEN}"
  echo -e "\n$TAG"

  docker compose down -v --remove-orphans --timeout 1 &>/dev/null || true

  export MODE STARTER=1
  export containers_cnt=$TOTAL_NODES groups=$GROUPS
  export musthave_neighbors=$(( TOTAL_NODES - 1 ))
  export LOSS_RATE=0

  docker compose up -d --build --scale node=$TOTAL_NODES
  echo "кластер поднят, даём ему 5 с на «раскачивание»…"
  sleep 5

  echo "отключаем $BROKEN случайных контейнеров (docker kill)"
  stop_random_nodes "$BROKEN"

  sleep "$RUN_SECS"

  mkdir -p results
  WD=$(docker ps -a --format '{{.Names}}' | grep watchdog)
  docker cp "$WD":/logs/stats.csv "results/$TAG.csv" 2>/dev/null \
        && echo "   ↳ results/$TAG.csv"
}

plot_one_mode () { local MODE=$1
  gnuplot -e "ALG='${MODE}'" plot_broken_nodes.gnuplot
  echo "results/broken_nodes_${MODE}.png готов"
}

for MODE in "${MODES[@]}"; do
  for BROKEN in "${BROKEN_LIST[@]}"; do
    one_round "$MODE" "$BROKEN"
  done
  plot_one_mode "$MODE"
done

echo -e "\nВсё готово — смотрите ./results/"
