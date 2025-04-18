#!/usr/bin/env bash
# Recursively find all .mtx files under a given folder, run the BCC executable,
# and write results to either a specified global CSV or to per-folder SBCC.csv files.

set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <source_folder> [path_to_executable] [output_csv]" >&2
  exit 1
fi

# Increase stack size to unlimited to avoid segmentation faults in deep recursion
ulimit -s unlimited

SRC_DIR="$1"
EXE="${2:-./bcc_mtx}"
OUTPUT_CSV="${3:-}"  # Optional global output CSV

# Verify the executable exists
if [[ ! -x "$EXE" ]]; then
  echo "Error: executable '$EXE' not found or not executable." >&2
  exit 1
fi

# Initialize global CSV if specified
if [[ -n "$OUTPUT_CSV" ]]; then
  mkdir -p "$(dirname "$OUTPUT_CSV")"
  if [[ ! -f "$OUTPUT_CSV" ]]; then
    echo "Graph,n,m,ccCount,numBCC,maxV,maxE,S_BCC" > "$OUTPUT_CSV"
  fi
  echo "Using global output CSV: $OUTPUT_CSV"
fi

echo "Searching for .mtx files under '$SRC_DIR'..."
find "$SRC_DIR" -type f -name "*.mtx" | while IFS= read -r mtx_file; do
  echo "------------------------------"
  echo "Processing: $mtx_file"

  # Determine CSV destination
  if [[ -n "$OUTPUT_CSV" ]]; then
    csv="$OUTPUT_CSV"
  else
    dir=$(dirname "$mtx_file")
    csv="$dir/SBCC.csv"
    # Initialize per-folder CSV header if missing
    if [[ ! -f "$csv" ]]; then
      echo "Graph,n,m,ccCount,numBCC,maxV,maxE,S_BCC" > "$csv"
    fi
  fi

  # Run the BCC program and append its output to the chosen CSV
  if ! "$EXE" "$mtx_file" >> "$csv"; then
    echo "Error: Failed to append results for '$mtx_file' to '$csv'" >&2
  else
    echo "Appended results to $csv."
  fi

done

echo "All done. Results in ${OUTPUT_CSV:-per-folder SBCC.csv files}."
