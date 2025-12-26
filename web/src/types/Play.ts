export interface Play {
  gameID: number;
  gameDate: string;
  quarter: number;
  minutes: number;
  seconds: number;
  timeAsInt: number;
  offense: string;
  defense: string;
  down: number;
  toGo: number;
  yardLine: number;
  resultIsFirstDown: boolean;
  firstDownWeight: number;
  description: string;
  resultingYards: number;
  yardsWeight: number;
  formation: string;
  playType: string;
  isRush: boolean;
  isPass: boolean;
  isIncomplete: boolean;
  isTouchdown: boolean;
  touchdownWeight: number;
  passType: string;
  isSack: boolean;
  isInterception: boolean;
  interceptionWeight: number;
  isFumble: boolean;
  fumbleWeight: number;
  isTwoPointConversion: boolean;
  isTwoPointConversionSuccessful: boolean;
  twoPointWeight: number;
  rushDirection: string;
}

export interface CurrentSituation {
  quarter: number;
  down: number;
  toGo: number;
  yardLine: number;
  minutes: number;
  seconds: number;
  timeAsInt: number;
  isTwoPointConversion: boolean;
}

export interface AnalysisResult {
  totalSimilarPlays: number;
  idealPlays: IdealPlay[];
  likelihoods: Likelihood[];
  bestPlay: BestPlayInfo | null;
  noMatch: boolean;
  noGainMatch: boolean;
  gameIds?: number[];
  // enhanced analysis data
  runVsPass: RunVsPassComparison | null;
  playTypeBreakdown: PlayTypeStats[];
  riskAssessment: RiskStats | null;
  averageYards: number;
  recommendedStrategy: string;
}

export interface IdealPlay {
  playType: string;
  subType: string;
  likelihood: number;
  attempts: number;
  successRate: number;
}

// new types for enhanced analysis
export interface RunVsPassComparison {
  runAttempts: number;
  runSuccessRate: number;
  runAvgYards: number;
  runFirstDownRate: number;
  runTurnoverRate: number;
  passAttempts: number;
  passSuccessRate: number;
  passAvgYards: number;
  passFirstDownRate: number;
  passTurnoverRate: number;
  recommendation: 'run' | 'pass' | 'balanced';
}

export interface PlayTypeStats {
  playType: string;
  subType: string;
  attempts: number;
  successes: number;
  successRate: number;
  avgYards: number;
  touchdowns: number;
  turnovers: number;
  negativePlayRate: number;
}

export interface RiskStats {
  overallTurnoverRate: number;
  interceptionRate: number;
  fumbleRate: number;
  sackRate: number;
  negativePlayRate: number;
  incompleteRate: number;
}

export interface Likelihood {
  name: string;
  percentage: number;
  passingPercentage?: number;
  rushingPercentage?: number;
}

export interface BestPlayInfo {
  playType: string;
  passType: string;
  rushDirection: string;
  formation: string;
  gameDate: string;
  offense: string;
  defense: string;
  quarter: number;
  down: number;
  yardLine: number;
  toGo: number;
  minutes: number;
  seconds: number;
  resultingYards: number;
  isTouchdown: boolean;
  resultIsFirstDown: boolean;
  isTwoPointConversion: boolean;
  isTwoPointConversionSuccessful: boolean;
  isPass: boolean;
  isRush: boolean;
  description: string;
  gameID: number;
}

export function createEmptyPlay(): Play {
  return {
    gameID: 0,
    gameDate: '',
    quarter: 0,
    minutes: 0,
    seconds: 0,
    timeAsInt: 0,
    offense: '',
    defense: '',
    down: 0,
    toGo: 0,
    yardLine: 0,
    resultIsFirstDown: false,
    firstDownWeight: 0,
    description: '',
    resultingYards: 0,
    yardsWeight: 0,
    formation: '',
    playType: '',
    isRush: false,
    isPass: false,
    isIncomplete: false,
    isTouchdown: false,
    touchdownWeight: 0,
    passType: '',
    isSack: false,
    isInterception: false,
    interceptionWeight: 0,
    isFumble: false,
    fumbleWeight: 0,
    isTwoPointConversion: false,
    isTwoPointConversionSuccessful: false,
    twoPointWeight: 0,
    rushDirection: ''
  };
}
