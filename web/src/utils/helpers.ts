// Helper functions ported from Helpers.cpp

import { Play, CurrentSituation } from '../types/Play';

export function booleanResult(val: number): boolean {
  return val === 1;
}

export function calculateWeight(yards: number): number {
  if (yards < 0) {
    return 0 - Math.pow(Math.abs(yards), 0.75);
  }
  return Math.pow(yards, 0.75);
}

export function timeToInt(minute: number, second: number): number {
  let returnString = String(minute);
  if (second < 10) {
    returnString += '0' + String(second);
  } else {
    returnString += String(second);
  }
  return parseInt(returnString, 10);
}

export function calculateTimeBounds(minute: number, second: number): [number, number] {
  let lowerMinute = 0;
  let lowerSecond: number;
  let upperMinute: number;
  let upperSecond: number;

  // lower bound - subtract 1:30
  if (second < 30) {
    if (minute < 2) {
      lowerMinute = 0;
    } else {
      lowerMinute = minute - 2;
    }
    lowerSecond = 60 + (second - 30);
  } else {
    lowerMinute = minute - 1;
    lowerSecond = second - 30;
  }

  // upper bound - add 1:30
  if (second >= 30) {
    if (minute > 13) {
      upperMinute = 15;
    } else {
      upperMinute = minute + 2;
    }
    upperSecond = (second + 30) - 60;
  } else {
    upperMinute = minute + 1;
    upperSecond = second + 30;
  }

  return [timeToInt(lowerMinute, lowerSecond), timeToInt(upperMinute, upperSecond)];
}

export function calculateToGoBounds(toGo: number): [number, number] {
  if (toGo === 0) {
    return [0, 0];
  }

  const lower = toGo < 2 ? 1 : toGo - 1;
  const upper = toGo > 98 ? 99 : toGo + 1;

  return [lower, upper];
}

export function calculateYardLineBounds(yardLine: number): [number, number] {
  const lower = yardLine < 5 ? 0 : yardLine - 5;
  const upper = yardLine > 94 ? 99 : yardLine + 5;

  return [lower, upper];
}

export function formatPercentage(percentage: number): string {
  if (percentage >= 10) {
    return percentage.toFixed(2);
  }
  return percentage.toFixed(2);
}

export function formatTime(minute: number, second: number): string {
  const mins = minute < 10 ? '0' + minute : String(minute);
  const secs = second < 10 ? '0' + second : String(second);
  return `${mins}:${secs}`;
}

export function generatePlayCode(play: Play | CurrentSituation): string {
  let playCode = '';

  // quarter (1-4)
  playCode += String(play.quarter);

  // down (0-4)
  playCode += String(play.down);

  // yards to go categories
  const yds2go = play.toGo;
  if (yds2go >= 1 && yds2go <= 3) {
    playCode += '0';
  } else if (yds2go >= 4 && yds2go <= 7) {
    playCode += '1';
  } else if (yds2go >= 8 && yds2go <= 13) {
    playCode += '2';
  } else if (yds2go >= 14 && yds2go <= 20) {
    playCode += '3';
  } else if (yds2go >= 21) {
    playCode += '4';
  } else {
    playCode += '0'; // default for 0
  }

  // yard line (0-9 based on tens place)
  playCode += String(Math.floor(play.yardLine / 10));

  // time categories
  const timeInSeconds = play.minutes * 60 + play.seconds;
  if (timeInSeconds <= 900 && timeInSeconds >= 451) {
    playCode += '0';
  } else if (timeInSeconds <= 450 && timeInSeconds >= 271) {
    playCode += '1';
  } else if (timeInSeconds <= 270 && timeInSeconds >= 121) {
    playCode += '2';
  } else {
    playCode += '3';
  }

  return playCode;
}

export function validateQuarter(val: number): boolean {
  return val >= 1 && val <= 4;
}

export function validateDown(val: number): boolean {
  return val >= 0 && val <= 4;
}

export function validateYardsToGo(val: number): boolean {
  return val >= 0 && val <= 99;
}

export function validateYardLine(val: number): boolean {
  return val >= 1 && val <= 99;
}

export function validateTime(minutes: number, seconds: number): boolean {
  if (minutes < 0 || minutes > 15) return false;
  if (seconds < 0 || seconds > 59) return false;
  if (minutes === 15 && seconds > 0) return false;
  return true;
}

export function parseTimeString(timeStr: string): { minutes: number; seconds: number } | null {
  const match = timeStr.match(/^(\d{1,2}):(\d{2})$/);
  if (!match) return null;

  const minutes = parseInt(match[1], 10);
  const seconds = parseInt(match[2], 10);

  if (!validateTime(minutes, seconds)) return null;

  return { minutes, seconds };
}

export function getPlayRating(play: Play): number {
  return play.firstDownWeight + play.yardsWeight + play.touchdownWeight + 
         play.interceptionWeight + play.twoPointWeight + play.fumbleWeight;
}

export function comparePlayRatings(a: Play, b: Play): number {
  return getPlayRating(b) - getPlayRating(a);
}
