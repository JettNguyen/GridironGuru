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

  if (result.noGainMatch) {
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

  const { totalSimilarPlays, idealPlays, likelihoods, bestPlay } = result;

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

      <div className="stats-summary">
        <div className="stat-box">
          <span className="stat-value">{totalSimilarPlays.toLocaleString()}</span>
          <span className="stat-label">Similar Situations</span>
        </div>
      </div>

      {idealPlays.length > 0 && (
        <section className="results-section">
          <h3>
            {idealPlays.length > 1 ? 'Ideal Plays' : 'Ideal Play'}
          </h3>
          <div className="ideal-plays-grid">
            {idealPlays.map((play, idx) => (
              <div key={idx} className="ideal-play-card">
                <div className="play-name">
                  {play.playType === 'FIELD GOAL' 
                    ? `${play.playType} in ${play.subType} Formation`
                    : `${play.playType} ${play.subType}`}
                </div>
                <div className="play-likelihood">
                  {formatPercentage(play.likelihood)}%
                </div>
              </div>
            ))}
          </div>
        </section>
      )}

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
