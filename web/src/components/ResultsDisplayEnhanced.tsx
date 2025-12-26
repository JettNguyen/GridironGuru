import { AnalysisResult } from '../types/Play';
import { formatPercentage, formatTime } from '../utils/helpers';
import './ResultsDisplay.css';

interface ResultsDisplayProps {
  result: AnalysisResult;
  analysisTime: number | null;
}

function ResultsDisplay({ result, analysisTime }: ResultsDisplayProps) {
  if (result.noMatch) {
    return (
      <div className="results-container no-match">
        <h2>No Match Found</h2>
        <p>No similar situations found in the historical data. Good luck out there!</p>
      </div>
    );
  }

  if (result.noGainMatch && !result.runVsPass) {
    return (
      <div className="results-container no-gain">
        <h2>Matches Found, But No Gain</h2>
        <p>We found {result.totalSimilarPlays} similar situation(s), but none resulted in positive yardage.</p>
        {result.gameIds && result.gameIds.length > 0 && (
          <div className="game-ids">
            <h4>Reference Game IDs:</h4>
            <ul>
              {result.gameIds.map(id => (
                <li key={id}>{id}</li>
              ))}
            </ul>
          </div>
        )}
      </div>
    );
  }

  const { totalSimilarPlays, likelihoods, bestPlay, runVsPass, playTypeBreakdown, riskAssessment, averageYards, recommendedStrategy } = result;

  return (
    <div className="results-container">
      <div className="results-header">
        <h2>Analysis Results</h2>
        {analysisTime !== null && (
          <span className="analysis-time">
            Analyzed in {(analysisTime / 1000).toFixed(3)}s
          </span>
        )}
      </div>

      {/* strategy recommendation banner */}
      {recommendedStrategy && (
        <div className="strategy-banner">
          <span className="strategy-text">{recommendedStrategy}</span>
        </div>
      )}

      {/* key stats row */}
      <div className="stats-summary">
        <div className="stat-box">
          <span className="stat-value">{totalSimilarPlays.toLocaleString()}</span>
          <span className="stat-label">Similar Plays</span>
        </div>
        <div className="stat-box">
          <span className="stat-value">{averageYards.toFixed(1)}</span>
          <span className="stat-label">Avg Yards</span>
        </div>
        {riskAssessment && (
          <div className="stat-box warning">
            <span className="stat-value">{riskAssessment.overallTurnoverRate.toFixed(1)}%</span>
            <span className="stat-label">Turnover Rate</span>
          </div>
        )}
      </div>

      {/* run vs pass comparison */}
      {runVsPass && (runVsPass.runAttempts > 0 || runVsPass.passAttempts > 0) && (
        <section className="results-section">
          <h3>Run vs Pass Comparison</h3>
          <div className="run-pass-grid">
            <div className={`comparison-card ${runVsPass.recommendation === 'run' ? 'recommended' : ''}`}>
              <div className="card-header">
                <span className="card-title">Run</span>
                {runVsPass.recommendation === 'run' && <span className="rec-badge">Recommended</span>}
              </div>
              <div className="card-stats">
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.runAttempts}</span>
                  <span className="mini-label">attempts</span>
                </div>
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.runAvgYards.toFixed(1)}</span>
                  <span className="mini-label">avg yards</span>
                </div>
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.runSuccessRate.toFixed(0)}%</span>
                  <span className="mini-label">success</span>
                </div>
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.runFirstDownRate.toFixed(0)}%</span>
                  <span className="mini-label">1st down</span>
                </div>
                <div className="mini-stat warning-stat">
                  <span className="mini-value">{runVsPass.runTurnoverRate.toFixed(1)}%</span>
                  <span className="mini-label">turnover</span>
                </div>
              </div>
            </div>

            <div className={`comparison-card ${runVsPass.recommendation === 'pass' ? 'recommended' : ''}`}>
              <div className="card-header">
                <span className="card-title">Pass</span>
                {runVsPass.recommendation === 'pass' && <span className="rec-badge">Recommended</span>}
              </div>
              <div className="card-stats">
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.passAttempts}</span>
                  <span className="mini-label">attempts</span>
                </div>
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.passAvgYards.toFixed(1)}</span>
                  <span className="mini-label">avg yards</span>
                </div>
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.passSuccessRate.toFixed(0)}%</span>
                  <span className="mini-label">success</span>
                </div>
                <div className="mini-stat">
                  <span className="mini-value">{runVsPass.passFirstDownRate.toFixed(0)}%</span>
                  <span className="mini-label">1st down</span>
                </div>
                <div className="mini-stat warning-stat">
                  <span className="mini-value">{runVsPass.passTurnoverRate.toFixed(1)}%</span>
                  <span className="mini-label">turnover</span>
                </div>
              </div>
            </div>
          </div>
        </section>
      )}

      {/* play type breakdown table */}
      {playTypeBreakdown.length > 0 && (
        <section className="results-section">
          <h3>Play Type Breakdown</h3>
          <div className="breakdown-table-wrapper">
            <table className="breakdown-table">
              <thead>
                <tr>
                  <th>Play</th>
                  <th>Attempts</th>
                  <th>Success</th>
                  <th>Avg Yds</th>
                  <th>TD</th>
                  <th>Risk</th>
                </tr>
              </thead>
              <tbody>
                {playTypeBreakdown.map((stat, idx) => (
                  <tr key={idx} className={idx === 0 ? 'top-play' : ''}>
                    <td className="play-cell">
                      <span className="play-type">{stat.playType}</span>
                      {stat.subType && <span className="play-subtype">{stat.subType}</span>}
                    </td>
                    <td>{stat.attempts}</td>
                    <td>
                      <span className={`success-rate ${getSuccessClass(stat.successRate)}`}>
                        {stat.successRate.toFixed(0)}%
                      </span>
                    </td>
                    <td className={stat.avgYards >= 0 ? 'positive' : 'negative'}>
                      {stat.avgYards.toFixed(1)}
                    </td>
                    <td>{stat.touchdowns}</td>
                    <td className={stat.turnovers > 0 || stat.negativePlayRate > 20 ? 'risky' : ''}>
                      {stat.turnovers > 0 ? `${stat.turnovers} TO` : 
                       stat.negativePlayRate > 15 ? `${stat.negativePlayRate.toFixed(0)}% neg` : 
                       'Low'}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </section>
      )}

      {/* risk assessment */}
      {riskAssessment && (
        <section className="results-section">
          <h3>Risk Assessment</h3>
          <div className="risk-grid">
            <RiskIndicator 
              label="Interception" 
              value={riskAssessment.interceptionRate} 
              threshold={5}
            />
            <RiskIndicator 
              label="Fumble" 
              value={riskAssessment.fumbleRate} 
              threshold={3}
            />
            <RiskIndicator 
              label="Sack" 
              value={riskAssessment.sackRate} 
              threshold={10}
            />
            <RiskIndicator 
              label="Negative Play" 
              value={riskAssessment.negativePlayRate} 
              threshold={20}
            />
            <RiskIndicator 
              label="Incomplete" 
              value={riskAssessment.incompleteRate} 
              threshold={40}
            />
          </div>
        </section>
      )}

      {/* outcome likelihoods */}
      {likelihoods.length > 0 && (
        <section className="results-section">
          <h3>Outcome Likelihoods</h3>
          <div className="likelihoods-list">
            {likelihoods.map((likelihood, idx) => (
              <div key={idx} className="likelihood-item">
                <div className="likelihood-header">
                  <span className="likelihood-name">{likelihood.name}</span>
                  <span className="likelihood-pct">{formatPercentage(likelihood.percentage)}%</span>
                </div>
                <div className="likelihood-bar">
                  <div 
                    className="likelihood-fill" 
                    style={{ width: `${Math.min(likelihood.percentage, 100)}%` }}
                  />
                </div>
                {(likelihood.passingPercentage !== undefined || likelihood.rushingPercentage !== undefined) && (
                  <div className="likelihood-breakdown">
                    {likelihood.passingPercentage !== undefined && (
                      <span>Passing: {formatPercentage(likelihood.passingPercentage)}%</span>
                    )}
                    {likelihood.rushingPercentage !== undefined && (
                      <span>Rushing: {formatPercentage(likelihood.rushingPercentage)}%</span>
                    )}
                  </div>
                )}
              </div>
            ))}
          </div>
        </section>
      )}

      {/* best historical play */}
      {bestPlay && (
        <section className="results-section best-play-section">
          <h3>Best Historical Play</h3>
          <div className="best-play-card">
            <div className="play-type-badge">
              {bestPlay.playType}
              {bestPlay.isPass && bestPlay.passType && ` ${bestPlay.passType}`}
              {bestPlay.isRush && bestPlay.rushDirection && ` ${bestPlay.rushDirection}`}
            </div>
            <div className="formation-info">
              {startsWithVowel(bestPlay.formation) ? 'an' : 'a'} <strong>{bestPlay.formation}</strong> formation
            </div>

            <div className="play-details">
              <p className="play-narrative">
                On <strong>{bestPlay.gameDate}</strong>, {bestPlay.offense}{' '}
                {bestPlay.isTwoPointConversion && bestPlay.isTwoPointConversionSuccessful ? (
                  <>scored 2 points against {bestPlay.defense} during Q{bestPlay.quarter} at {formatTime(bestPlay.minutes, bestPlay.seconds)}.</>
                ) : (
                  <>gained <strong>{bestPlay.resultingYards} yards</strong> against {bestPlay.defense} during Q{bestPlay.quarter} on {getOrdinal(bestPlay.down)} down at the {bestPlay.yardLine} yard line with {bestPlay.toGo} yards to go at {formatTime(bestPlay.minutes, bestPlay.seconds)}.</>
                )}
              </p>

              <p className="play-outcome">
                {bestPlay.isTwoPointConversion ? (
                  <>The play was a Two Point Conversion and was {bestPlay.isTwoPointConversionSuccessful ? '' : 'not '}successful.</>
                ) : (
                  <>
                    {bestPlay.isTouchdown ? (
                      'The play resulted in a touchdown.'
                    ) : bestPlay.resultIsFirstDown ? (
                      'The play did not result in a touchdown, but it did result in a first down.'
                    ) : (
                      'The play did not result in a touchdown or a first down.'
                    )}
                  </>
                )}
              </p>

              <div className="play-description">
                <strong>Description:</strong> {bestPlay.description}
              </div>

              <div className="game-id">
                Game ID: {bestPlay.gameID}
              </div>
            </div>
          </div>
        </section>
      )}
    </div>
  );
}

function RiskIndicator({ label, value, threshold }: { label: string; value: number; threshold: number }) {
  const level = value === 0 ? 'none' : value < threshold ? 'low' : value < threshold * 2 ? 'medium' : 'high';
  
  return (
    <div className={`risk-indicator ${level}`}>
      <span className="risk-value">{value.toFixed(1)}%</span>
      <span className="risk-label">{label}</span>
    </div>
  );
}

function getSuccessClass(rate: number): string {
  if (rate >= 60) return 'high';
  if (rate >= 40) return 'medium';
  return 'low';
}

function startsWithVowel(str: string): boolean {
  if (!str) return false;
  return ['A', 'E', 'I', 'O', 'U'].includes(str[0].toUpperCase());
}

function getOrdinal(n: number): string {
  const suffixes = ['th', 'st', 'nd', 'rd'];
  const v = n % 100;
  return n + (suffixes[(v - 20) % 10] || suffixes[v] || suffixes[0]);
}

export default ResultsDisplay;
