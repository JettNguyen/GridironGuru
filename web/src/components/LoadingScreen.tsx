import { LoadProgress } from '../utils/dataLoader';
import './LoadingScreen.css';

interface LoadingScreenProps {
  progress: LoadProgress;
}

function LoadingScreen({ progress }: LoadingScreenProps) {
  const getPhaseText = () => {
    switch (progress.phase) {
      case 'downloading':
        return 'Fetching play data...';
      case 'parsing':
        return `Processing plays... (${progress.loaded.toLocaleString()})`;
      case 'complete':
        return 'Ready!';
      default:
        return 'Loading...';
    }
  };

  const percentage = progress.phase === 'downloading' 
    ? 0 
    : Math.min((progress.loaded / progress.total) * 100, 100);

  return (
    <div className="loading-screen">
      <div className="loading-content">
        <img 
          src={import.meta.env.BASE_URL + 'GridironGuru.svg'} 
          alt="Gridiron Guru logo" 
          className="loading-icon bounce" 
        />
        <h1>Gridiron Guru</h1>
        <p className="loading-text">{getPhaseText()}</p>
        <div className="progress-container">
          <div 
            className="progress-bar" 
            style={{ width: `${percentage}%` }}
          />
        </div>
        <p className="loading-info">
          Loading 538,000+ plays from 2013-2024
        </p>
      </div>
    </div>
  );
}

export default LoadingScreen;
