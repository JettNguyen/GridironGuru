import { useMemo, useState } from 'react';
import { CurrentSituation } from '../types/Play';
import { timeToInt, validateQuarter, validateDown, validateYardsToGo, validateYardLine, parseTimeString } from '../utils/helpers';
import './InputForm.css';

type DataStructure = 'heap' | 'hashtable';

interface InputFormProps {
  onAnalyze: (situation: CurrentSituation, dataStructure: DataStructure) => void;
  disabled: boolean;
  playCount: number;
}

function InputForm({ onAnalyze, disabled, playCount }: InputFormProps) {
  const [dataStructure, setDataStructure] = useState<DataStructure>('heap');
  const [quarter, setQuarter] = useState('1');
  const [down, setDown] = useState('1');
  const [yardsToGo, setYardsToGo] = useState('10');
  const [yardLine, setYardLine] = useState('50');
  const [minutes, setMinutes] = useState('07');
  const [seconds, setSeconds] = useState('30');
  const [errors, setErrors] = useState<Record<string, string>>({});

  const minuteOptions = useMemo(() => Array.from({ length: 16 }, (_, i) => i.toString().padStart(2, '0')), []);
  const secondOptions = useMemo(() => Array.from({ length: 60 }, (_, i) => i.toString().padStart(2, '0')), []);

  const setFieldError = (field: string, message?: string) => {
    setErrors(prev => {
      const next = { ...prev };
      if (message) {
        next[field] = message;
      } else {
        delete next[field];
      }
      return next;
    });
  };

  const clampNumericInput = (value: string, min: number, max: number): string => {
    const digitsOnly = value.replace(/\D/g, '');
    if (digitsOnly === '') {
      return '';
    }
    const numeric = Math.max(min, Math.min(parseInt(digitsOnly, 10), max));
    return String(numeric);
  };

  const handleQuarterChange = (value: string) => {
    setQuarter(value);
    const qtr = parseInt(value, 10);
    setFieldError('quarter', validateQuarter(qtr) ? undefined : 'Quarter must be 1-4');
  };

  const handleDownChange = (value: string) => {
    setDown(value);
    const dn = parseInt(value, 10);
    setFieldError('down', validateDown(dn) ? undefined : 'Down must be 0-4');
  };

  const handleYardsToGoChange = (value: string) => {
    const formatted = clampNumericInput(value, 0, 99);
    setYardsToGo(formatted);

    if (formatted === '') {
      setFieldError('yardsToGo', 'Enter yards to go');
      return;
    }

    const distance = parseInt(formatted, 10);
    setFieldError('yardsToGo', validateYardsToGo(distance) ? undefined : 'Yards to go must be 0-99');
  };

  const handleYardLineChange = (value: string) => {
    const formatted = clampNumericInput(value, 1, 99);
    setYardLine(formatted);

    if (formatted === '') {
      setFieldError('yardLine', 'Enter field position');
      return;
    }

    const position = parseInt(formatted, 10);
    setFieldError('yardLine', validateYardLine(position) ? undefined : 'Yard line must be 1-99');
  };

  const handleMinutesChange = (value: string) => {
    setMinutes(value);
    setFieldError('time', undefined);

    if (value === '15' && seconds !== '00') {
      setSeconds('00');
    }
  };

  const handleSecondsChange = (value: string) => {
    if (minutes === '15') {
      setSeconds('00');
      return;
    }
    setSeconds(value);
    setFieldError('time', undefined);
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    const newErrors: Record<string, string> = {};

    const qtr = parseInt(quarter, 10);
    if (!validateQuarter(qtr)) {
      newErrors.quarter = 'Quarter must be 1-4';
    }

    const dn = parseInt(down, 10);
    if (!validateDown(dn)) {
      newErrors.down = 'Down must be 0-4 (use 0 for 2pt conversion)';
    }

    const ytg = yardsToGo === '' ? NaN : parseInt(yardsToGo, 10);
    if (!validateYardsToGo(ytg)) {
      newErrors.yardsToGo = 'Yards to go must be 0-99';
    }

    const yl = yardLine === '' ? NaN : parseInt(yardLine, 10);
    if (!validateYardLine(yl)) {
      newErrors.yardLine = 'Yard line must be 1-99';
    }

    const normalizedTime = `${minutes}:${seconds}`;
    const parsedTime = parseTimeString(normalizedTime);
    if (!parsedTime) {
      newErrors.time = 'Time must be in mm:ss format (max 15:00)';
    }

    if (Object.keys(newErrors).length > 0) {
      setErrors(newErrors);
      return;
    }

    setErrors({});

    const isTwoPointConversion = dn === 0 && ytg === 0 && (yl === 98 || yl === 99);

    const situation: CurrentSituation = {
      quarter: qtr,
      down: dn,
      toGo: ytg,
      yardLine: yl,
      minutes: parsedTime!.minutes,
      seconds: parsedTime!.seconds,
      timeAsInt: timeToInt(parsedTime!.minutes, parsedTime!.seconds),
      isTwoPointConversion
    };

    onAnalyze(situation, dataStructure);
  };

  const handleTwoPointPreset = () => {
    handleDownChange('0');
    handleYardsToGoChange('0');
    handleYardLineChange('98');
  };

  return (
    <form className="input-form" onSubmit={handleSubmit}>
      <div className="form-intro">
        <p>
          Enter the current game situation and we'll analyze <strong>{playCount.toLocaleString()}</strong> historical 
          plays from 2013-2024 to suggest the best play call.
        </p>
      </div>

      <div className="data-structure-toggle">
        <span className="toggle-label">Analysis Method:</span>
        <div className="toggle-buttons">
          <button
            type="button"
            className={`toggle-btn ${dataStructure === 'heap' ? 'active' : ''}`}
            onClick={() => setDataStructure('heap')}
          >
            Max Heap
          </button>
          <button
            type="button"
            className={`toggle-btn ${dataStructure === 'hashtable' ? 'active' : ''}`}
            onClick={() => setDataStructure('hashtable')}
          >
            Hash Table
          </button>
        </div>
      </div>

      <div className="form-grid">
        <div className={`form-group ${errors.quarter ? 'has-error' : ''}`}>
          <label htmlFor="quarter">Quarter</label>
          <select
            id="quarter"
            value={quarter}
            onChange={(e) => handleQuarterChange(e.target.value)}
            disabled={disabled}
          >
            <option value="1">1st Quarter</option>
            <option value="2">2nd Quarter</option>
            <option value="3">3rd Quarter</option>
            <option value="4">4th Quarter</option>
          </select>
          {errors.quarter && <span className="error-msg">{errors.quarter}</span>}
        </div>

        <div className={`form-group ${errors.down ? 'has-error' : ''}`}>
          <label htmlFor="down">Down</label>
          <select
            id="down"
            value={down}
            onChange={(e) => handleDownChange(e.target.value)}
            disabled={disabled}
          >
            <option value="0">2pt Conversion</option>
            <option value="1">1st Down</option>
            <option value="2">2nd Down</option>
            <option value="3">3rd Down</option>
            <option value="4">4th Down</option>
          </select>
          {errors.down && <span className="error-msg">{errors.down}</span>}
        </div>

        <div className={`form-group ${errors.yardsToGo ? 'has-error' : ''}`}>
          <label htmlFor="yardsToGo">Yards to Go</label>
          <input
            type="text"
            id="yardsToGo"
            value={yardsToGo}
            onChange={(e) => handleYardsToGoChange(e.target.value)}
            inputMode="numeric"
            pattern="[0-9]*"
            disabled={disabled}
            placeholder="e.g., 10"
          />
          {errors.yardsToGo && <span className="error-msg">{errors.yardsToGo}</span>}
        </div>

        <div className={`form-group ${errors.yardLine ? 'has-error' : ''}`}>
          <label htmlFor="yardLine">
            Field Position
            <span className="field-hint">(0 = your goal, 100 = opponent's)</span>
          </label>
          <input
            type="text"
            id="yardLine"
            value={yardLine}
            onChange={(e) => handleYardLineChange(e.target.value)}
            inputMode="numeric"
            pattern="[0-9]*"
            disabled={disabled}
            placeholder="e.g., 50"
          />
          {errors.yardLine && <span className="error-msg">{errors.yardLine}</span>}
        </div>

        <div className={`form-group ${errors.time ? 'has-error' : ''}`}>
          <label>Time Remaining</label>
          <div className={`time-scrollers ${disabled ? 'is-disabled' : ''}`}>
            <div className="time-scroll">
              <span className="time-scroll-label">Minutes</span>
              <select
                value={minutes}
                onChange={(e) => handleMinutesChange(e.target.value)}
                disabled={disabled}
                size={5}
                className="time-scroll-select"
              >
                {minuteOptions.map((option) => (
                  <option key={option} value={option}>
                    {option}
                  </option>
                ))}
              </select>
            </div>
            <div className="time-scroll">
              <span className="time-scroll-label">Seconds</span>
              <select
                value={minutes === '15' ? '00' : seconds}
                onChange={(e) => handleSecondsChange(e.target.value)}
                disabled={disabled || minutes === '15'}
                size={6}
                className="time-scroll-select"
              >
                {(minutes === '15' ? ['00'] : secondOptions).map((option) => (
                  <option key={option} value={option}>
                    {option}
                  </option>
                ))}
              </select>
            </div>
          </div>
          {errors.time && <span className="error-msg">{errors.time}</span>}
        </div>
      </div>

      <div className="form-actions">
        <button 
          type="button" 
          className="preset-btn"
          onClick={handleTwoPointPreset}
          disabled={disabled}
        >
          Set 2pt Conversion
        </button>
        <button 
          type="submit" 
          className="submit-btn"
          disabled={disabled}
        >
          {disabled ? 'Analyzing...' : 'Analyze Situation'}
        </button>
      </div>
    </form>
  );
}

export default InputForm;
