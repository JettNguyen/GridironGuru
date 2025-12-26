# Gridiron Guru

## Project Overview

Gridiron Guru aggregates twelve seasons (2013-2024) of NFL play-by-play data (538,745 snaps) to surface situational play recommendations. You can explore suggestions through the React web dashboard or run the original C++ console app for direct terminal interaction.

### Tech Stack
- React + Vite frontend for interactive analysis
- TypeScript utility layer for CSV parsing and analytics helpers
- C++ data structures (max heap and hash table)

## Running the Project Locally

### Web
Use the GitHub Pages Link in this repo

### C++ Console App
1. `cd src`
2. Configure with CMake or your IDE of choice
3. Build the `Project3` target and run it from the generated binary directory

Both modes expect the CSV files inside the top-level `files/` directory.

## Dataset Notes
- Primary file: `files/pbp2013-2024.csv`
- Total rows: **538,745**
- Includes new 2024 regular-season snaps; earlier seasons are unchanged

If you add newer seasons, update the CSV file and keep `loadPlayData` pointed at the latest path so the UI reflects the full historical sample.

## Contributors
- Jett Nguyen
- Zach Ostroff
- William Shaoul
