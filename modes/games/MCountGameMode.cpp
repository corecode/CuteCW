#include "MCountGameMode.h"

#include <qdebug.h>

MCountGameMode::MCountGameMode(int maxTurns)
  : MGameMode(), m_turnNumber(0), m_maxTurns(maxTurns)
{
}

void MCountGameMode::startGame() 
{
  m_turnNumber = 0;
  MGameMode::startGame();
}

void MCountGameMode::nextTurn(int scoreAddition) 
{
  m_turnNumber++;
  MGameMode::nextTurn(scoreAddition);

  qDebug() << "turn: " << m_turnNumber << " / " << m_maxTurns;

  if (m_turnNumber >= m_maxTurns) {
    gameOver();
  }   
}
