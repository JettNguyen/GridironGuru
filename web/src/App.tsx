import { useState, useEffect, useCallback } from 'react';
import Header from './components/Header';
import InputForm from './components/InputForm.tsx';
import ResultsDisplay from './components/ResultsDisplayEnhanced';
import LoadingScreen from './components/LoadingScreen.tsx';
import Footer from './components/Footer.tsx';
import { Play, CurrentSituation, AnalysisResult } from './types/Play';
import { loadPlayData, LoadProgress } from './utils/dataLoader';
import { analyzeWithHeap } from './utils/heapAnalysis';
import { PlayHashTable, analyzeWithHashTable } from './utils/hashTableAnalysis';

type DataStructure = 'heap' | 'hashtable';

function App() {
  const [plays, setPlays] = useState<Play[]>([]);
  const [hashTable, setHashTable] = useState<PlayHashTable | null>(null);
  const [loading, setLoading] = useState(true);
  const [loadProgress, setLoadProgress] = useState<LoadProgress>({ 
    loaded: 0, 
    total: 100, 
    phase: 'downloading' 
  });
  const [error, setError] = useState<string | null>(null);
  const [result, setResult] = useState<AnalysisResult | null>(null);
  const [analyzing, setAnalyzing] = useState(false);
  const [analysisTime, setAnalysisTime] = useState<number | null>(null);

  useEffect(() => {
    async function initData() {
      try {
        const loadedPlays = await loadPlayData(setLoadProgress);
        setPlays(loadedPlays);

        // build hash table in background
        const table = PlayHashTable.buildFromPlays(loadedPlays);
        setHashTable(table);

        setLoading(false);
      } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to load play data');
        setLoading(false);
      }
    }

    initData();
  }, []);

  const handleAnalyze = useCallback((
    situation: CurrentSituation,
    dataStructure: DataStructure
  ) => {
    setAnalyzing(true);
    setResult(null);

    // use setTimeout to allow UI to update before heavy computation
    setTimeout(() => {
      const startTime = performance.now();

      let analysisResult: AnalysisResult;

      if (dataStructure === 'hashtable' && hashTable) {
        analysisResult = analyzeWithHashTable(situation, hashTable);
      } else {
        analysisResult = analyzeWithHeap(situation, plays);
      }

      const endTime = performance.now();
      setAnalysisTime(endTime - startTime);
      setResult(analysisResult);
      setAnalyzing(false);
    }, 50);
  }, [plays, hashTable]);

  if (loading) {
    return <LoadingScreen progress={loadProgress} />;
  }

  if (error) {
    return (
      <div className="error-container">
        <h2>Something went wrong</h2>
        <p>{error}</p>
        <button onClick={() => window.location.reload()}>Try Again</button>
      </div>
    );
  }

  return (
    <div className="app">
      <Header />
      <main className="main-content">
        <section className="context-band">
          <div className="context-card">
            <span className="card-label">Historical Sample</span>
            <strong>{plays.length.toLocaleString()}</strong>
            <small>plays from 2013-2024</small>
          </div>
          <div className="context-card">
            <span className="card-label">Data Structures Ready</span>
            <strong>{hashTable ? 'Heap · Hash Table' : 'Building...'}</strong>
            <small>optimized for instant lookups</small>
          </div>
          <div className="context-card">
            <span className="card-label">Analysis Speed</span>
            <strong>{analysisTime ? `${(analysisTime / 1000).toFixed(2)}s` : '< 0.5s'}</strong>
            <small>typical decision latency</small>
          </div>
        </section>

        <section className="experience-grid">
          <InputForm 
            onAnalyze={handleAnalyze}
            disabled={analyzing}
            playCount={plays.length}
          />

          <div className="insights-panel" aria-live="polite">
            {analyzing ? (
              <div className="analyzing-indicator">
                <div className="pulse-ring" aria-hidden="true"></div>
                <div>
                  <p>Crunching {plays.length.toLocaleString()} plays...</p>
                  <span>We sift through every comparable snap before recommending a call.</span>
                </div>
              </div>
            ) : result ? (
              <ResultsDisplay 
                result={result} 
                analysisTime={analysisTime}
              />
            ) : (
              <div className="insights-placeholder">
                <h3>Ready when you are</h3>
                <p>Choose an analysis method, enter the situation, and we'll surface the most human-relevant insights from the dataset.</p>
                <ul>
                  <li>Compare run vs pass tendencies side-by-side</li>
                  <li>Spot the safest plays before dialing up aggression</li>
                  <li>Review a real historical snap that matches your call</li>
                </ul>
              </div>
            )}
          </div>
        </section>
      </main>
      <Footer />
    </div>
  );
}

export default App;
