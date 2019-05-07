#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QPainter>

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr);
    ~Board();
signals:

public slots:

private:
    void paintEvent(QPaintEvent *);
    QPainter *paint;
};

#endif // BOARD_H
