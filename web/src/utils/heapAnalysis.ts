// MaxHeap-based play analysis entry point

import {
	Play,
	CurrentSituation,
	AnalysisResult,
	IdealPlay,
	Likelihood,
	BestPlayInfo,
	RunVsPassComparison,
	PlayTypeStats,
	RiskStats
} from '../types/Play';
import {
	calculateTimeBounds,
	calculateToGoBounds,
	calculateYardLineBounds,
	comparePlayRatings
} from './helpers';

export function analyzeWithHeap(
	currentSituation: CurrentSituation,
	plays: Play[]
): AnalysisResult {
	const similarPlays = filterSimilarPlays(currentSituation, plays);

	if (similarPlays.length === 0) {
		return createEmptyResult(true, false);
	}

	const sortedPlays = [...similarPlays].sort(comparePlayRatings);
	const runVsPass = calculateRunVsPass(similarPlays);
	const playTypeBreakdown = calculatePlayTypeBreakdown(similarPlays);
	const riskAssessment = calculateRiskStats(similarPlays);
	const averageYards = calculateAverageYards(similarPlays);
	const recommendedStrategy = determineStrategy(
		runVsPass,
		currentSituation,
		playTypeBreakdown,
		riskAssessment
	);

	const bestPlay = findBestPlay(sortedPlays);
	const stats = calculateStats(currentSituation, similarPlays);
	const idealPlays = buildIdealPlays(playTypeBreakdown);
	const likelihoods = buildLikelihoods(currentSituation, stats, similarPlays.length);

	if (!bestPlay) {
		return {
			...createEmptyResult(false, true),
			totalSimilarPlays: similarPlays.length,
			gameIds: sortedPlays.slice(0, 5).map(p => p.gameID),
			runVsPass,
			playTypeBreakdown,
			riskAssessment,
			averageYards,
			recommendedStrategy
		};
	}

	const bestPlayInfo: BestPlayInfo = {
		playType: bestPlay.playType,
		passType: bestPlay.passType,
		rushDirection: bestPlay.rushDirection,
		formation: bestPlay.formation,
		gameDate: bestPlay.gameDate,
		offense: bestPlay.offense,
		defense: bestPlay.defense,
		quarter: bestPlay.quarter,
		down: bestPlay.down,
		yardLine: bestPlay.yardLine,
		toGo: bestPlay.toGo,
		minutes: bestPlay.minutes,
		seconds: bestPlay.seconds,
		resultingYards: bestPlay.resultingYards,
		isTouchdown: bestPlay.isTouchdown,
		resultIsFirstDown: bestPlay.resultIsFirstDown,
		isTwoPointConversion: bestPlay.isTwoPointConversion,
		isTwoPointConversionSuccessful: bestPlay.isTwoPointConversionSuccessful,
		isPass: bestPlay.isPass,
		isRush: bestPlay.isRush,
		description: bestPlay.description,
		gameID: bestPlay.gameID
	};

	return {
		totalSimilarPlays: similarPlays.length,
		idealPlays,
		likelihoods,
		bestPlay: bestPlayInfo,
		noMatch: false,
		noGainMatch: false,
		runVsPass,
		playTypeBreakdown,
		riskAssessment,
		averageYards,
		recommendedStrategy
	};
}

function createEmptyResult(noMatch: boolean, noGainMatch: boolean): AnalysisResult {
	return {
		totalSimilarPlays: 0,
		idealPlays: [],
		likelihoods: [],
		bestPlay: null,
		noMatch,
		noGainMatch,
		runVsPass: null,
		playTypeBreakdown: [],
		riskAssessment: null,
		averageYards: 0,
		recommendedStrategy: ''
	};
}

function filterSimilarPlays(situation: CurrentSituation, plays: Play[]): Play[] {
	let toGoLower = 0;
	let toGoUpper = 0;
	let yardLineLower = 98;
	let yardLineUpper = 99;

	if (situation.down !== 0) {
		[toGoLower, toGoUpper] = calculateToGoBounds(situation.toGo);
		[yardLineLower, yardLineUpper] = calculateYardLineBounds(situation.yardLine);
	}

	const [timeLower, timeUpper] = calculateTimeBounds(situation.minutes, situation.seconds);

	let filtered = plays.filter(play =>
		play.quarter === situation.quarter &&
		play.down === situation.down &&
		play.toGo >= toGoLower &&
		play.toGo <= toGoUpper &&
		play.yardLine >= yardLineLower &&
		play.yardLine <= yardLineUpper &&
		play.timeAsInt >= timeLower &&
		play.timeAsInt <= timeUpper
	);

	if (filtered.length < 50 && situation.down !== 0) {
		filtered = plays.filter(play =>
			play.quarter === situation.quarter &&
			play.down === situation.down &&
			play.toGo >= toGoLower &&
			play.toGo <= toGoUpper &&
			play.yardLine >= yardLineLower &&
			play.yardLine <= yardLineUpper
		);
	}

	if (filtered.length < 30 && situation.down !== 0) {
		const relaxedLower = Math.max(0, situation.yardLine - 10);
		const relaxedUpper = Math.min(99, situation.yardLine + 10);

		filtered = plays.filter(play =>
			play.quarter === situation.quarter &&
			play.down === situation.down &&
			play.toGo >= toGoLower &&
			play.toGo <= toGoUpper &&
			play.yardLine >= relaxedLower &&
			play.yardLine <= relaxedUpper
		);
	}

	return filtered;
}

function findBestPlay(sortedPlays: Play[]): Play | null {
	for (const play of sortedPlays) {
		if (!play.isIncomplete && !play.isInterception && play.resultingYards >= 0) {
			return play;
		}
	}

	return sortedPlays.length > 0 ? sortedPlays[0] : null;
}

function calculateRunVsPass(plays: Play[]): RunVsPassComparison {
	const runs = plays.filter(p => p.isRush);
	const passes = plays.filter(p => p.isPass);

	const runSuccesses = runs.filter(
		p => p.resultIsFirstDown || p.isTouchdown || p.resultingYards > 0
	);
	const passSuccesses = passes.filter(
		p => p.resultIsFirstDown || p.isTouchdown || (p.resultingYards > 0 && !p.isIncomplete)
	);

	const runTurnovers = runs.filter(p => p.isFumble);
	const passTurnovers = passes.filter(p => p.isInterception || p.isFumble);

	const runAvg = runs.length > 0 ? runs.reduce((sum, p) => sum + p.resultingYards, 0) / runs.length : 0;
	const passAvg = passes.length > 0 ? passes.reduce((sum, p) => sum + p.resultingYards, 0) / passes.length : 0;

	const runFDRate = runs.length > 0 ? (runs.filter(p => p.resultIsFirstDown).length / runs.length) * 100 : 0;
	const passFDRate = passes.length > 0 ? (passes.filter(p => p.resultIsFirstDown).length / passes.length) * 100 : 0;

	let recommendation: 'run' | 'pass' | 'balanced' = 'balanced';
	const runScore = (runAvg * 2) + runFDRate - (runTurnovers.length / Math.max(runs.length, 1) * 50);
	const passScore = (passAvg * 2) + passFDRate - (passTurnovers.length / Math.max(passes.length, 1) * 50);

	if (runScore > passScore * 1.15) {
		recommendation = 'run';
	} else if (passScore > runScore * 1.15) {
		recommendation = 'pass';
	}

	return {
		runAttempts: runs.length,
		runSuccessRate: runs.length > 0 ? (runSuccesses.length / runs.length) * 100 : 0,
		runAvgYards: runAvg,
		runFirstDownRate: runFDRate,
		runTurnoverRate: runs.length > 0 ? (runTurnovers.length / runs.length) * 100 : 0,
		passAttempts: passes.length,
		passSuccessRate: passes.length > 0 ? (passSuccesses.length / passes.length) * 100 : 0,
		passAvgYards: passAvg,
		passFirstDownRate: passFDRate,
		passTurnoverRate: passes.length > 0 ? (passTurnovers.length / passes.length) * 100 : 0,
		recommendation
	};
}

function calculatePlayTypeBreakdown(plays: Play[]): PlayTypeStats[] {
	const breakdown = new Map<string, {
		attempts: number;
		successes: number;
		totalYards: number;
		touchdowns: number;
		turnovers: number;
		negativePlays: number;
		playType: string;
		subType: string;
	}>();

	for (const play of plays) {
		const subType = play.isPass ? play.passType : play.rushDirection;
		const key = `${play.playType}|${subType || 'unknown'}`;

		if (!breakdown.has(key)) {
			breakdown.set(key, {
				attempts: 0,
				successes: 0,
				totalYards: 0,
				touchdowns: 0,
				turnovers: 0,
				negativePlays: 0,
				playType: play.playType,
				subType: subType || ''
			});
		}

		const stats = breakdown.get(key)!;
		stats.attempts++;
		stats.totalYards += play.resultingYards;

		const isSuccess =
			play.resultIsFirstDown ||
			play.isTouchdown ||
			(play.resultingYards > 0 && !play.isIncomplete);

		if (isSuccess) {
			stats.successes++;
		}

		if (play.isTouchdown) {
			stats.touchdowns++;
		}

		if (play.isInterception || play.isFumble) {
			stats.turnovers++;
		}

		if (play.resultingYards < 0) {
			stats.negativePlays++;
		}
	}

	return Array.from(breakdown.values())
		.map(s => ({
			playType: s.playType,
			subType: s.subType,
			attempts: s.attempts,
			successes: s.successes,
			successRate: s.attempts > 0 ? (s.successes / s.attempts) * 100 : 0,
			avgYards: s.attempts > 0 ? s.totalYards / s.attempts : 0,
			touchdowns: s.touchdowns,
			turnovers: s.turnovers,
			negativePlayRate: s.attempts > 0 ? (s.negativePlays / s.attempts) * 100 : 0
		}))
		.filter(s => s.attempts >= 3)
		.sort((a, b) => {
			const scoreA = a.successRate * Math.log10(a.attempts + 1) * (1 - (a.turnovers / Math.max(a.attempts, 1)) * 0.5);
			const scoreB = b.successRate * Math.log10(b.attempts + 1) * (1 - (b.turnovers / Math.max(b.attempts, 1)) * 0.5);
			return scoreB - scoreA;
		})
		.slice(0, 8);
}

function calculateRiskStats(plays: Play[]): RiskStats {
	const total = plays.length;

	if (total === 0) {
		return {
			overallTurnoverRate: 0,
			interceptionRate: 0,
			fumbleRate: 0,
			sackRate: 0,
			negativePlayRate: 0,
			incompleteRate: 0
		};
	}

	const passes = plays.filter(p => p.isPass);
	const turnovers = plays.filter(p => p.isInterception || p.isFumble);
	const interceptions = plays.filter(p => p.isInterception);
	const fumbles = plays.filter(p => p.isFumble);
	const sacks = passes.filter(p => p.isSack);
	const negative = plays.filter(p => p.resultingYards < 0);
	const incompletions = passes.filter(p => p.isIncomplete);

	return {
		overallTurnoverRate: (turnovers.length / total) * 100,
		interceptionRate: passes.length > 0 ? (interceptions.length / passes.length) * 100 : 0,
		fumbleRate: (fumbles.length / total) * 100,
		sackRate: passes.length > 0 ? (sacks.length / passes.length) * 100 : 0,
		negativePlayRate: (negative.length / total) * 100,
		incompleteRate: passes.length > 0 ? (incompletions.length / passes.length) * 100 : 0
	};
}

function calculateAverageYards(plays: Play[]): number {
	if (plays.length === 0) {
		return 0;
	}

	const total = plays.reduce((sum, play) => sum + play.resultingYards, 0);
	return total / plays.length;
}

function determineStrategy(
	runVsPass: RunVsPassComparison | null,
	situation: CurrentSituation,
	breakdown: PlayTypeStats[],
	risk: RiskStats | null
): string {
	if (!runVsPass) {
		return '';
	}

	const downLabel = situation.down === 0 ? 'conversion' : `down ${situation.down}`;
	let guidance = '';

	if (runVsPass.recommendation === 'run') {
		guidance = 'Lean on the ground game — runs are generating safer, more efficient yardage right now';
	} else if (runVsPass.recommendation === 'pass') {
		guidance = 'Spread the field — passing concepts are outperforming the run in these spots';
	} else {
		guidance = 'Mix your calls — neither run nor pass has a decisive edge, so stay unpredictable';
	}

	const topPlay = breakdown[0];
	if (topPlay) {
		guidance += `. Top result: ${topPlay.playType}${topPlay.subType ? ` (${topPlay.subType})` : ''} with ${topPlay.successRate.toFixed(1)}% success across ${topPlay.attempts} reps`;
	}

	if (risk && risk.overallTurnoverRate > 8) {
		guidance += '. Ball security is a concern — consider quicker throws or conservative concepts';
	}

	return `${guidance} for ${downLabel}, ${situation.toGo} to go inside the ${situation.yardLine}.`;
}

interface PlayStats {
	firstDowns: number;
	touchdowns: number;
	conversions: number;
	fieldGoals: number;
	firstDownPasses: number;
	firstDownRushes: number;
	touchdownPasses: number;
	touchdownRushes: number;
	twoPointPasses: number;
	twoPointRushes: number;
}

function calculateStats(situation: CurrentSituation, plays: Play[]): PlayStats {
	const stats: PlayStats = {
		firstDowns: 0,
		touchdowns: 0,
		conversions: 0,
		fieldGoals: 0,
		firstDownPasses: 0,
		firstDownRushes: 0,
		touchdownPasses: 0,
		touchdownRushes: 0,
		twoPointPasses: 0,
		twoPointRushes: 0
	};

	for (const play of plays) {
		if (situation.isTwoPointConversion && play.isTwoPointConversion && play.playType !== 'EXTRA POINT') {
			if (play.isTwoPointConversionSuccessful) {
				stats.conversions++;
				if (play.description.includes('PASS')) {
					stats.twoPointPasses++;
				} else {
					stats.twoPointRushes++;
				}
			}
			continue;
		}

		if (play.resultIsFirstDown) {
			stats.firstDowns++;
			if (play.isPass) {
				stats.firstDownPasses++;
			} else if (play.isRush) {
				stats.firstDownRushes++;
			}
		}

		if (play.isTouchdown) {
			stats.touchdowns++;
			if (play.isPass) {
				stats.touchdownPasses++;
			} else if (play.isRush) {
				stats.touchdownRushes++;
			}
		}

		if (play.playType === 'FIELD GOAL' && play.description.includes('IS GOOD')) {
			stats.fieldGoals++;
		}
	}

	return stats;
}

function buildIdealPlays(playTypeBreakdown: PlayTypeStats[]): IdealPlay[] {
	if (playTypeBreakdown.length === 0) {
		return [];
	}

	const totalAttempts = playTypeBreakdown.reduce((sum, stat) => sum + stat.attempts, 0) || 1;

	return playTypeBreakdown.slice(0, 5).map(stat => ({
		playType: stat.playType,
		subType: stat.subType || 'Balanced',
		likelihood: (stat.attempts / totalAttempts) * 100,
		attempts: stat.attempts,
		successRate: stat.successRate
	}));
}

function buildLikelihoods(
	situation: CurrentSituation,
	stats: PlayStats,
	totalPlays: number
): Likelihood[] {
	if (totalPlays === 0) {
		return [];
	}

	const likelihoods: Likelihood[] = [];

	if (situation.isTwoPointConversion) {
		const conversionPct = (stats.conversions / totalPlays) * 100;
		const passPct = stats.conversions > 0 ? (stats.twoPointPasses / stats.conversions) * 100 : 0;
		const rushPct = stats.conversions > 0 ? (stats.twoPointRushes / stats.conversions) * 100 : 0;

		likelihoods.push({
			name: 'Two Point Conversion',
			percentage: conversionPct,
			passingPercentage: passPct,
			rushingPercentage: rushPct
		});
	} else {
		const fdPct = (stats.firstDowns / totalPlays) * 100;
		if (fdPct > 0) {
			likelihoods.push({
				name: 'First Down',
				percentage: fdPct,
				passingPercentage: stats.firstDowns > 0 ? (stats.firstDownPasses / stats.firstDowns) * 100 : 0,
				rushingPercentage: stats.firstDowns > 0 ? (stats.firstDownRushes / stats.firstDowns) * 100 : 0
			});
		}

		const tdPct = (stats.touchdowns / totalPlays) * 100;
		if (tdPct > 0) {
			likelihoods.push({
				name: 'Touchdown',
				percentage: tdPct,
				passingPercentage: stats.touchdowns > 0 ? (stats.touchdownPasses / stats.touchdowns) * 100 : 0,
				rushingPercentage: stats.touchdowns > 0 ? (stats.touchdownRushes / stats.touchdowns) * 100 : 0
			});
		}

		const fgPct = (stats.fieldGoals / totalPlays) * 100;
		if (fgPct > 0) {
			likelihoods.push({
				name: 'Field Goal',
				percentage: fgPct
			});
		}
	}

	return likelihoods.sort((a, b) => b.percentage - a.percentage);
}

