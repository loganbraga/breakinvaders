#include "include/breakinvaders.hpp"

QTextStream cout(stdout, QIODevice::WriteOnly);

BreakInvaders::BreakInvaders(QWidget *parent) : QWidget(parent)
{
  paddle = new Paddle();
  ball = new Ball(paddle->getGeneratedPos());
  scoreController = new Score();
  cout << "Meilleur score : " << scoreController->getBestScore() << "\n";

  anyBrick = false;
  nightMode = false;
  resetGameStatus();
}

BreakInvaders::~BreakInvaders()
{
  delete ball;
  delete paddle;
  destructBricks();
  cout << "Score : " << scoreController->getScore() << "\n";
}

void BreakInvaders::constructBricks()
{
  int k = 0;
  int imageSeed = 1;
  anyBrick = true;
  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j < COLS; j++)
    {
      rand() % 10 > 6 ? imageSeed = 2 : imageSeed = 1;
      bricks[k] = new Brick(j*40+35, i*15+30, imageSeed);
      k++;
    }
  }
}

void BreakInvaders::destructBricks()
{
  for (int i = 0; i < BRICKS; i++)
  {
    delete bricks[i];
  }
  anyBrick = false;
}

void BreakInvaders::nullifyBricks()
{
  for (int i = 0; i < BRICKS; i++)
  {
    int randSeed = rand() % 10;
    if (randSeed > 8 && nullifiedBricks < 11)
    {
      bricks[i]->setNullified(true);
      nullifiedBricks++;
    }
  }
}

void BreakInvaders::resetGameStatus()
{
  if (gameWon)
  {
    paddle->resetState();
    ball->resetState(paddle->getGeneratedPos());
  }

  if (anyBrick)
  {
    destructBricks();
  }

  gameOver = false;
  gameWon = false;
  gameStarted = false;
  paused = false;
  lives = MAX_LIVES;
  nullifiedBricks = 0;
  lastLostTime = time(NULL);
  scoreController->resetScore();
  constructBricks();
  for (int i = 0; i < BRICKS; i++)
  {
    bricks[i]->setDestroyed(false);
    bricks[i]->setNullified(false);
  }
  nullifyBricks();
}

void BreakInvaders::paintEvent(QPaintEvent *event)
{
  event->rect();
  QPainter painter(this);

  if (gameOver)
  {
    QFont font("Monospace", 15, QFont::DemiBold);
    QFontMetrics fm(font);
    int textWidth = fm.width("Perdu !");

    painter.setFont(font);
    int h = height();
    int w = width();

    QPen penHText(QColor("#2f2f2f"));
    if (nightMode)
    {
      penHText.setColor("#ececec");
    }
    else
    {
      penHText.setColor("#2f2f2f");
    }
    painter.setPen(penHText);
    painter.translate(QPoint(w/2, h/2));
    painter.drawText(-textWidth/2, 0, "Perdu !");


    QFont fontMini("Monospace", 10, QFont::DemiBold);
    QFontMetrics fmMini(fontMini);
    painter.setFont(fontMini);

    QString bestScoreBase = "Meilleur score : ";
    QString bestScoreNumber = QString::number(scoreController->getBestScore());
    QString bestScoreText = bestScoreBase + bestScoreNumber;
    int textWidthBestScore = fmMini.width(bestScoreText);
    painter.drawText(-textWidthBestScore/2, 35, bestScoreText);

    QPen penHTextRed(QColor("#d9534f"));
    painter.setPen(penHTextRed);
    int textWidthReplay = fmMini.width("<Espace> pour rejouer");
    painter.drawText(-textWidthReplay/2, 100, "<Espace> pour rejouer");
  }
  else if(gameWon)
  {
    QFont font("Monospace", 15, QFont::DemiBold);
    QFontMetrics fm(font);
    int textWidth = fm.width("Victoire !");

    painter.setFont(font);
    int h = height();
    int w = width();

    QPen penHText(QColor("#d9534f"));
    painter.setPen(penHText);
    painter.translate(QPoint(w/2, h/2));
    painter.drawText(-textWidth/2, 0, "Victoire !");


    QFont fontMini("Monospace", 10, QFont::DemiBold);
    QFontMetrics fmMini(fontMini);
    painter.setFont(fontMini);
    int textWidthReplay = fmMini.width("<Espace> pour rejouer");
    painter.drawText(-textWidthReplay/2, 100, "<Espace> pour rejouer");

    QPen penHTextMini(QColor("#2f2f2f"));
    if (nightMode)
    {
      penHTextMini.setColor("#ececec");
    }
    else
    {
      penHTextMini.setColor("#2f2f2f");
    }
    painter.setPen(penHTextMini);

    QString scoreBase = "Score : ";
    QString scoreNumber = QString::number(scoreController->getScore());
    QString scoreText = scoreBase + scoreNumber;
    int textWidthScore = fmMini.width(scoreText);
    painter.drawText(-textWidthScore/2, 40, scoreText);

    QString bestScoreBase = "Meilleur score : ";
    QString bestScoreNumber = QString::number(scoreController->getBestScore());
    QString bestScoreText = bestScoreBase + bestScoreNumber;
    int textWidthBestScore = fmMini.width(bestScoreText);
    painter.drawText(-textWidthBestScore/2, 55, bestScoreText);
  }
  else
  {
    painter.drawImage(ball->getRect(), ball->getImage());
    painter.drawImage(paddle->getRect(), paddle->getImage());

    for (int i = 0; i < BRICKS; i++)
    {
      if (!bricks[i]->isDestroyed() && !bricks[i]->isNullified())
      {
        painter.drawImage(bricks[i]->getRect(), bricks[i]->getImage());
      }
    }
  }
}

void BreakInvaders::timerEvent(QTimerEvent *event)
{
  event->timerId();
  ball->autoMove();
  checkCollision();
  repaint();
}

void BreakInvaders::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Left: case Qt::Key_Up: case Qt::Key_H: case Qt::Key_J:
    {
      if (!paused && gameStarted)
      {
        int x = paddle->getRect().x();
        for (int i = 1; i <= 5; i++)
        {
          paddle->moveLeft(x-- - 2*i);
        }
      }
    }
    break;
    case Qt::Key_Right: case Qt::Key_Down: case Qt::Key_L: case Qt::Key_K:
    {
      if (!paused && gameStarted)
      {
        int x = paddle->getRect().x();
        for (int i = 1; i <= 5; i++)
        {
          paddle->moveRight(x++ + 2*i);
        }
      }
    }
    break;
    case Qt::Key_P:
    {
      pauseGame();
    }
    break;
    case Qt::Key_Space:
    {
      pauseGame();
      startGame();
    }
    break;
    case Qt::Key_R:
    {
      restartGame();
    }
    break;
    case Qt::Key_V:
    {
      for (int i = 0; i < BRICKS - 1; i++)
      {
        if (!bricks[i]->isNullified())
        {
          bricks[i]->setDestroyed(true);
        }
      }
    }
    break;
    case Qt::Key_N:
    {
      toggleNightMode();
    }
    break;
    case Qt::Key_Escape: case Qt::Key_Q:
    {
      qApp->exit();
    }
    break;
    default:
    {
      QWidget::keyPressEvent(event);
    }
  }
}

void BreakInvaders::startGame()
{
  if (!gameStarted)
  {
    if (lives != MAX_LIVES)
    {
      paddle->resetState();
      ball->resetState(paddle->getGeneratedPos());
    }

    resetGameStatus();
    gameStarted = true;
    timerId = startTimer(10);
  }
}

void BreakInvaders::pauseGame()
{
  if (paused)
  {
    timerId = startTimer(10);
    paused = false;
  }
  else
  {
    paused = true;
    killTimer(timerId);
  }
}

void BreakInvaders::restartGame()
{
  if (gameStarted)
  {
    killTimer(timerId);
    resetGameStatus();
    paddle->resetState();
    ball->resetState(paddle->getGeneratedPos());
    startGame();
  }
}

void BreakInvaders::defeat()
{
  killTimer(timerId);
  scoreController->calculateScore(bricks, BRICKS, lives);
  gameOver = true;
  gameStarted = false;
}

void BreakInvaders::victory()
{
  killTimer(timerId);
  scoreController->calculateScore(bricks, BRICKS, lives);
  gameWon = true;
  gameStarted = false;
}

void BreakInvaders::lostBall()
{
  currentTime = time(NULL);
  if (lastLostTime < currentTime - 1)
  {
    lastLostTime = time(NULL);
    lives--;

    if (lives <= 0)
    {
      defeat();
    }
    else
    {
      paddle->resetState();
      ball->resetState(paddle->getGeneratedPos());
      pauseGame();
    }
  }
}

void BreakInvaders::toggleNightMode()
{
  if (nightMode)
  {
    this->setStyleSheet("QWidget { background-color: #f2f2f2 }");
    nightMode = false;
  }
  else
  {
    this->setStyleSheet("QWidget { background-color: #2f2f2f }");
    nightMode = true;
  }
}

void BreakInvaders::checkCollision()
{
  if (ball->getRect().bottom() > 400)
  {
    lostBall();
  }

  for (int i = 0, j = 0; i < BRICKS; i++)
  {
    if (bricks[i]->isDestroyed())
    {
      j++;
    }
    if (j >= BRICKS - nullifiedBricks)
    {
      victory();
    }
  }

  if ((ball->getRect()).intersects(paddle->getRect()))
  {
    int paddleLPos = paddle->getRect().left();
    int ballLPos = ball->getRect().left();

    int first = paddleLPos + 8;
    int second = paddleLPos + 16;
    int third = paddleLPos + 24;
    int fourth = paddleLPos + 32;

    if (ballLPos < first)
    {
      ball->setXDir(-1);
      ball->setYDir(-1);
    }

    if (ballLPos >= first && ballLPos < second)
    {
      ball->setXDir(-1);
      ball->setYDir(-1*ball->getYDir());
    }

    if (ballLPos >= second && ballLPos < third)
    {
      ball->setXDir(0);
      ball->setYDir(-1);
    }

    if (ballLPos >= third && ballLPos < fourth)
    {
      ball->setXDir(1);
      ball->setYDir(-1*ball->getYDir());
    }

    if (ballLPos > fourth)
    {
      ball->setXDir(1);
      ball->setYDir(-1);
    }
  }

  for (int i = 0; i < BRICKS; i++)
  {
    if ((ball->getRect()).intersects(bricks[i]->getRect()))
    {
      int ballLeft = ball->getRect().left();
      int ballHeight = ball->getRect().height();
      int ballWidth = ball->getRect().width();
      int ballTop = ball->getRect().top();

      QPoint pointRight(ballLeft + ballWidth + 1, ballTop);
      QPoint pointLeft(ballLeft - 1, ballTop);
      QPoint pointTop(ballLeft, ballTop -1);
      QPoint pointBottom(ballLeft, ballTop + ballHeight + 1);

      if (!bricks[i]->isDestroyed() && !bricks[i]->isNullified())
      {
        if(bricks[i]->getRect().contains(pointRight))
        {
          ball->setXDir(-1);
        }
        else if(bricks[i]->getRect().contains(pointLeft))
        {
          ball->setXDir(1);
        }

        if(bricks[i]->getRect().contains(pointTop))
        {
          ball->setYDir(1);
        }
        else if(bricks[i]->getRect().contains(pointBottom))
        {
          ball->setYDir(-1);
        }

        if (bricks[i]->isBlack())
        {
          bricks[i]->setBlack(false);
        }
        else
        {
          bricks[i]->setDestroyed(true);
        }
      }
    }
  }
}
