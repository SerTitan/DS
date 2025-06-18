#!/usr/bin/env bash
set -e

echo "Сборка проекта…"
cmake -S . -B build
cmake --build build

echo "Настройка переменных окружения…"
containers_cnt=100
groups=5
containers_in_group=$(( containers_cnt / groups ))
containers_cnt=$((containers_in_group * groups))
musthave_neighbors=$((containers_cnt - 1))

export containers_cnt groups containers_in_group musthave_neighbors

echo "Запуск контейнеров ($containers_cnt)…"
docker compose up -d --build --scale node=$containers_cnt "$@"

echo "Ожидание watchdog…"
WATCHDOG=$(docker ps -a --format '{{.Names}}' | grep watchdog)
while docker ps --filter "name=$WATCHDOG" --format '{{.Status}}' | grep -q ^Up; do
    sleep 1
done

echo "Копирование результатов…"
rm -rf logs
docker cp "$WATCHDOG":/logs ./logs
cp ./logs/stats.csv . || echo "stats.csv не найден"
cp ./logs/plot.png  . || echo "plot.png не найден"


echo "Готово!"
