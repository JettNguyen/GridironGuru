import Papa from 'papaparse';
import { Play, createEmptyPlay } from '../types/Play';
import { booleanResult, calculateWeight, timeToInt } from './helpers';

const DATA_URL = 'https://raw.githubusercontent.com/JettNguyen/GridironGuru/master/files/pbp2013-2024.csv';

const ESTIMATED_TOTAL = 538000;

export interface LoadProgress {
  loaded: number;
  total: number;
  phase: 'downloading' | 'parsing' | 'complete';
}

export async function loadPlayData(
  onProgress?: (progress: LoadProgress) => void
): Promise<Play[]> {
  const plays: Play[] = [];

  return new Promise((resolve, reject) => {
    onProgress?.({ loaded: 0, total: 100, phase: 'downloading' });

    Papa.parse(DATA_URL, {
      download: true,
      header: false,
      skipEmptyLines: true,
      step: (row) => {
        const data = row.data as string[];
        
        // skip header row
        if (data[0] === 'GameId' || data[0] === 'gameId') {
          return;
        }

        try {
          const play = parsePlayRow(data);
          if (play) {
            plays.push(play);
          }
        } catch (err) {
          // skip malformed rows silently
        }

        // progress update every 10k plays
        if (plays.length % 10000 === 0) {
          onProgress?.({ 
            loaded: plays.length, 
            total: ESTIMATED_TOTAL, 
            phase: 'parsing' 
          });
        }
      },
      complete: () => {
        onProgress?.({ loaded: plays.length, total: plays.length, phase: 'complete' });
        resolve(plays);
      },
      error: (error) => {
        reject(new Error(`Failed to load data: ${error.message}`));
      }
    });
  });
}

function parsePlayRow(data: string[]): Play | null {
  if (data.length < 24) return null;

  const play = createEmptyPlay();

  try {
    play.gameID = parseInt(data[0], 10) || 0;
    play.gameDate = data[1] || '';
    play.quarter = parseInt(data[2], 10) || 0;
    play.minutes = parseInt(data[3], 10) || 0;
    play.seconds = parseInt(data[4], 10) || 0;
    play.timeAsInt = timeToInt(play.minutes, play.seconds);
    play.offense = data[5] || '';
    play.defense = data[6] || '';
    play.down = parseInt(data[7], 10) || 0;
    play.toGo = parseInt(data[8], 10) || 0;
    play.yardLine = parseInt(data[9], 10) || 0;
    play.resultIsFirstDown = booleanResult(parseInt(data[10], 10) || 0);
    
    if (play.resultIsFirstDown) {
      play.firstDownWeight = 10.0;
    }

    play.description = data[11] || '';
    play.resultingYards = parseInt(data[12], 10) || 0;
    play.yardsWeight = calculateWeight(play.resultingYards);
    play.formation = data[13] || '';
    play.playType = data[14] || '';
    play.isRush = booleanResult(parseInt(data[15], 10) || 0);
    play.isPass = booleanResult(parseInt(data[16], 10) || 0);
    play.isIncomplete = booleanResult(parseInt(data[17], 10) || 0);
    play.isTouchdown = booleanResult(parseInt(data[18], 10) || 0);

    if (play.isTouchdown) {
      play.touchdownWeight = 10.0;
    }

    play.passType = data[19] || '';
    play.isSack = booleanResult(parseInt(data[20], 10) || 0);
    play.isInterception = booleanResult(parseInt(data[21], 10) || 0);

    if (play.isInterception) {
      play.interceptionWeight = -100.0;
    }

    play.isFumble = booleanResult(parseInt(data[22], 10) || 0);

    if (play.isFumble) {
      play.fumbleWeight = -1000.0;
    }

    play.isTwoPointConversion = booleanResult(parseInt(data[23], 10) || 0);
    play.isTwoPointConversionSuccessful = booleanResult(parseInt(data[24], 10) || 0);

    if (play.isTwoPointConversionSuccessful) {
      play.twoPointWeight = 5.0;
    }

    play.rushDirection = data[25] || '';

    return play;
  } catch {
    return null;
  }
}
