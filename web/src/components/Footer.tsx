import './Footer.css';

function Footer() {
  return (
    <footer className="footer">
      <div className="footer-content">
        <p className="credits">
          Developed by Jett Nguyen, Zach Ostroff, and William Shaoul
        </p>
        <p className="data-info">
          Play-by-play data from 2013–2024 NFL seasons
        </p>
        <p className="links">
          <a 
            href="https://github.com/JettNguyen/GridironGuru" 
            target="_blank" 
            rel="noopener noreferrer"
          >
            View on GitHub
          </a>
        </p>
      </div>
    </footer>
  );
}

export default Footer;
