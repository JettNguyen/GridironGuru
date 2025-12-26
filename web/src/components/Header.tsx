import './Header.css';

function Header() {
  return (
    <header className="header">
      <div className="header-content">
        <div className="logo-section">
          <img 
            src={import.meta.env.BASE_URL + 'GridironGuru.svg'} 
            alt="Gridiron Guru logo" 
            className="logo-icon" 
          />
          <h1>Gridiron Guru</h1>
        </div>
        <p className="tagline">
          NFL play analysis powered by 12 years of historical data
        </p>
      </div>
    </header>
  );
}

export default Header;
