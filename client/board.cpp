#include "board.h"

Board::Board(QWidget *parent) : QWidget(parent)
{

}

Board::~Board(){

}

void Board::paintEvent(QPaintEvent *){
    int x=20,y=20;
    const int board_size = 10;
    const int grid_size = 30;
    paint = new QPainter;
    paint->begin(this);

    paint->setPen(QPen(Qt::gray,4,Qt::SolidLine));
    paint->setBrush(QBrush(Qt::white,Qt::SolidPattern));

    for(int i=0;i<=board_size;i++)
    {
        paint->drawLine(20,y+grid_size*i,20 + grid_size*board_size,y+grid_size*i);
    }

    for(int i=0;i<=board_size;i++)
    {
        paint->drawLine(x+grid_size*i,20,x+grid_size*i, 20 + grid_size * board_size);
    }

    paint->end();
}
