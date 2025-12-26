import { Play, CurrentSituation, AnalysisResult } from '../types/Play';
import { generatePlayCode } from './helpers';
import { analyzeWithHeap } from './heapAnalysis';

interface HashNode {
  play: Play;
  next: HashNode | null;
}

const PRIMES = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 
  61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 
  149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 
  229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 
  313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 
  409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499];

export class PlayHashTable {
  private capacity: number;
  private table: (HashNode | null)[];
  private count: number;

  constructor(initialCapacity: number = 500) {
    this.capacity = initialCapacity;
    this.table = new Array(initialCapacity).fill(null);
    this.count = 0;
  }

  private hashFunc(playCode: string): number {
    const hashCode = parseInt(playCode, 10) || 0;
    
    // find largest prime less than capacity
    let previous = PRIMES[0];
    for (const prime of PRIMES) {
      if (prime < this.capacity) {
        previous = prime;
      } else {
        break;
      }
    }

    return hashCode % previous;
  }

  insert(play: Play): void {
    // skip unwanted play types (matching C++ logic)
    if (!play.playType || 
        play.playType === 'NO PLAY' || 
        play.playType === 'TIMEOUT' ||
        play.playType === 'KICK OFF' || 
        play.playType === 'PUNT' || 
        play.playType === 'EXTRA POINT' ||
        play.playType === 'QB KNEEL') {
      return;
    }

    const playCode = generatePlayCode(play);
    const index = this.hashFunc(playCode);

    const newNode: HashNode = { play, next: this.table[index] };
    this.table[index] = newNode;
    this.count++;

    // check load factor
    const loadFactor = 0.7;
    if (this.count / this.capacity > loadFactor) {
      this.rehash();
    }
  }

  private rehash(): void {
    const oldTable = this.table;
    this.capacity = this.capacity * 2;
    this.table = new Array(this.capacity).fill(null);
    this.count = 0;

    for (const head of oldTable) {
      let current = head;
      while (current !== null) {
        const playCode = generatePlayCode(current.play);
        const index = this.hashFunc(playCode);
        const newNode: HashNode = { play: current.play, next: this.table[index] };
        this.table[index] = newNode;
        this.count++;
        current = current.next;
      }
    }
  }

  getSimilarPlays(situation: CurrentSituation): Play[] {
    const playCode = generatePlayCode(situation as any);
    const index = this.hashFunc(playCode);

    const plays: Play[] = [];
    let current = this.table[index];

    while (current !== null) {
      plays.push(current.play);
      current = current.next;
    }

    return plays;
  }

  static buildFromPlays(plays: Play[]): PlayHashTable {
    const table = new PlayHashTable(500);
    for (const play of plays) {
      table.insert(play);
    }
    return table;
  }
}

export function analyzeWithHashTable(
  currentSituation: CurrentSituation,
  hashTable: PlayHashTable
): AnalysisResult {
  // get plays from hash bucket
  const similarPlays = hashTable.getSimilarPlays(currentSituation);

  // use the same analysis logic as the heap method
  // (the hash table just provides faster lookup for similar situations)
  return analyzeWithHeap(currentSituation, similarPlays);
}
