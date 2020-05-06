#include "player.h"
#include "bullet.h"
#include "monster.h"
#include "mainwindow.h"
#include "MainScene.h"
#include "plateform.h"
#include "floor.h"
#include <QObject>
#include <QDebug>
#include <QKeyEvent>
#include <QVector>
#include <QTimer>
#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPointF>

extern MainScene * mainScene;
extern Health * health;
extern Monster * monster;
extern gameOver * gameover;
extern QMediaPlayer * gameOverSound;
extern QMediaPlayer * endGameSound;
extern EndGame * endgame;
extern Player * player;
extern Wall * wall;
extern Void * vide;
extern Plateform * plateform;


Player::Player(QString description, const QString& imgFileNameRight, const QString& imgFileNameLeft){

    this->setPixmap(QPixmap(imgFileNameRight));
    groundPosition = 350;
    onGround = true;

    wallContact=false;
    groundContact=true;
    plateformContact=false;
    this->position = "Waiting";
    this->direction = "Right";
    this->setVelocity();
    this->gravity=-2;

    imgRight = imgFileNameRight;
    imgLeft = imgFileNameLeft;

}

void Player::setPosition(std::string status, std::string previousStatus) {
    this->position = std::move(status);
    this->previousStatus = std::move(previousStatus);
}

std::string Player::getPosition() {
    return this->position;
}

void Player::setwallContact()
{
    QList<QGraphicsItem *>colliding_items = collidingItems();
    for(int i=0, n=colliding_items.size(); i<n; ++i){
        if(typeid(*(colliding_items[i])) == typeid(Wall)){wallContact = true;}
        else{wallContact = false;}
    }
}
void Player::setgroundContact(){

    if (this->y()==350){
        groundContact = true;
    }
    else {
        groundContact = false;
    }

}

void Player::setDirection(std::string direction) {
    this->direction = std::move(direction);
}
std::string Player::getDirection() {
    return this->direction;
}

void Player::setVelocity(){
    this->vitesseX=2;
    this->vitesseY=3;
}

void Player::setPreviousStatus(std::string previousStatus) {
    this->previousStatus = std::move(previousStatus);
}

void Player::move() {
    if(position=="Waiting"){
        this->waiting();
    }else if(position=="Running"){
        this->running();
    } else if(position=="Jumping"){
        this->jumping();
    } else if(position=="Falling") {
        this->falling();
    }
}

void Player::collisions()
{
    QList<QGraphicsItem *>colliding_items = collidingItems();
    for(int i=0, n=colliding_items.size(); i<n; ++i){
        if(typeid(*(colliding_items[i])) == typeid(Monster)){
            //decrease health
            health->decrease();
        }
        if(typeid(*(colliding_items[i])) == typeid(Wall)){
            if (direction == "Right") {
                this->moveBy(-25, 0);
            }
            else if (direction == "Left") {
                this->moveBy(25, 0);
            }
        }
        if(typeid(*(colliding_items[i])) == typeid(Void)){
            this->position="Falling";
        }
        if(typeid(*(colliding_items[i])) == typeid(EndGame)){
            qDebug() << "End Game";
            if (endGameSound->state() == QMediaPlayer::PlayingState){
                endGameSound->setPosition(0);
            }
            else if (endGameSound->state() == QMediaPlayer::StoppedState){
                endGameSound->play();
            }
            endgame->endScreen();
            scene()->removeItem(this);
//            timer_chrono->stop();
        }
        if(typeid(*(colliding_items[i])) == typeid(Plateform)){
            plateformContact = true;
        }
    }
}

void Player::waiting() {

    if(player->pos().y()<349 && !plateformContact){
        this->falling();
    }
}

void Player::running() {

    if(groundContact) {
        if (direction == "Right") {
            this->setPixmap(QPixmap(imgRight));
            this->moveBy(10, 0);
            if(wallContact || this->x()-10 > 2900){
                this->moveBy(-25, 0);
            }
        } else if (direction == "Left") {
            this->setPixmap(QPixmap(imgLeft));
            this->moveBy(-10, 0);
            if (wallContact || this->x()-10 < 0) {
                this->moveBy(25, 0);
            }
        }
    } else {
        position = "Falling";
    }
}

//jump() function code had been inspired by the video (link below)
//Math for Game Developers - Jumping and Gravity (Time Delta, Game Loop)
//https://www.youtube.com/watch?v=c4b9lCfSDQM

void Player::jumping() {
    if(this->previousStatus=="Waiting"){
        if (vitesseY < 0 ){
            this->position = "Falling";
            return;
        } else {
            vitesseX=0;
            this->moveBy(0, -vitesseY * 10);
        }
    } else {
        if (vitesseY < 0 || wallContact){
            this->position = "Falling";
            return;
        } else {
            if (direction == "Right") {
                this->moveBy(vitesseX * 7, -vitesseY * 10);
            } else if (direction == "Left") {
                this->moveBy(-vitesseX * 10, -vitesseY * 10);
            }
        }
    }
    vitesseY = vitesseY + gravity*0.15;
}

void Player::falling() {
    if(wallContact || player->pos().x()<0){
        if(this->y()>350){
            if(this->direction=="Right"){
                this->direction="Left";
            } else if(this->direction=="Left"){
                this->direction="Right";
            }
        } else {
            vitesseX = 0;
        }
    }
    if(this->direction=="Right"){
        this->moveBy(vitesseX*5,vitesseY*5);
    } else if(this->direction=="Left"){
        this->moveBy(-vitesseX*5,vitesseY*5);
    }
    if(player->pos().y()>380){
        gameover->display();
    }
    if(plateformContact){
        this->position = "Waiting";
        this->setVelocity();
        return;
    }
    if(this->pos().y()>=349){
        this->position = "Waiting";
        this->setVelocity();
        return;
    }

    vitesseY = vitesseY - gravity * 0.15;

}
