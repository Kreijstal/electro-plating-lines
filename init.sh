#!/usr/bin/env bash

if [ -d "../Hadamard_product_counters" ]; then
  if git -C ../Hadamard_product_counters rev-parse; then
    echo "Directory ../Hadamard_product_counters already exists and is a Git repository."
  else
    echo "Directory ../Hadamard_product_counters already exists but is not a Git repository."
    exit 1
  fi
else
  git clone --depth=1 https://github.com/Kreijstal/Hadamard_product_counters.git ../Hadamard_product_counters
fi

make all