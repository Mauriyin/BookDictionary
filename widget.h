#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<qdebug.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0

typedef int Status;
typedef struct SaveDate{
    int Line,Page;
    struct SaveDate *next;
}SaveDate,*DateList;

typedef struct SaveWord{
   char word[100];
   DateList savedate;
   DateList wei;
   int wordnum;
}SaveWord;

typedef struct BitNode{
    SaveWord Node;
    struct BitNode *Lchild, *Rchild;
}BitNode,*BitTree;

typedef struct Repeat{
    int line;
    int num;
    struct Repeat *next;
}Repeat;

typedef struct HashNode{
    char word[100];
    int wordnum;
    int line;
    int nuber;
    Repeat *first;
    Repeat *tail;
    struct HashNode *next;
}HashNode,*HashTree;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    Status InitDSTable(BitTree *DT);
    void DestroyDSTable(BitTree *DT);
    BitTree SearchDSTable(BitTree DT, char *e);
    Status SearchDSTable2(BitTree DT,char *e, BitTree f, BitTree *p);
    Status InsertDSTable(BitTree *DT, SaveWord e);
    void print_message(BitTree DT);
    int isNull(char c);
    Status Opentxt(BitTree *DT);
    bool readBTree(BitTree *DT);
    bool savbittree(BitTree DT);

    void InitHash();
    Status OpenHash();
    void InsertHash(HashNode e);
    int GetHashNum(char *word);
    bool savehash();
    bool readhash();
    void Print_Hash();
    ~Widget();

private slots:

    void on_pushButton_2_clicked();

    void on_readtxt_clicked();

    void on_radioButton_2_toggled();

    void on_radioButton_toggled();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_readtxt_2_clicked();

private:
    Ui::Widget *ui;
};



#endif // WIDGET_H
