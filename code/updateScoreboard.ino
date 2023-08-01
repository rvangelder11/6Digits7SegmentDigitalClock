void updateScoreboard() {
  byte sl1 = scoreboardLeft / 10;
  byte sl2 = scoreboardLeft % 10;
  byte sr1 = scoreboardRight / 10;
  byte sr2 = scoreboardRight % 10;

  displayNumber(sl1, 3, scoreboardColorLeft,0);
  displayNumber(sl2, 2, scoreboardColorLeft,0);
  displayNumber(sr1, 1, scoreboardColorRight,0);
  displayNumber(sr2, 0, scoreboardColorRight,0);
  hideDots();
}
