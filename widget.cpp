#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QTime>
#include <QProcess>
#include <QDir>
#define Max_Num 1000000

int number = 1;
int keep = 0;
int num = 0;
int pep;
BitTree dTable = NULL;
int BiTree_Static = -1;

SaveWord word[Max_Num];

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->radioButton->toggle();
    ui->label->setText(tr("词典中总共有 %1 个单词").arg(num));
    ui->label_2->setText(tr("当前检索文件为："));
    ui->lineEdit->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->lineEdit_2->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->listWidget->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->listWidget_2->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->textBrowser_2->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->readtxt->setFlat(true);
    ui->pushButton_2->setFlat(true);
}

Widget::~Widget()
{
    delete ui;
}

Status Widget::InitDSTable(BitTree *DT)
{
    *DT = NULL;
     return OK;
}

void Widget::DestroyDSTable(BitTree *DT)
{
     //非空树
     if(*DT)
      {
          //如果有左子树，则销毁左子树
          if((*DT)->Lchild)
          {
              DestroyDSTable(&(*DT)->Lchild);
          }
          //如果有右子树，则销毁右子树
          if((*DT)->Rchild)
          {
              DestroyDSTable(&(*DT)->Rchild);
          }

          free(*DT);    //释放根结点
          *DT = NULL;
      }
}

BitTree Widget::SearchDSTable(BitTree DT, char *e)
{
    //查找结束或查找成功
    if((!DT) || strcmp(e,DT->Node.word) == 0)
    {
        return DT;
    }
    //在左子树中继续查找
    else if (strcmp(e,DT->Node.word) < 0)
    {
        keep++;
        return SearchDSTable(DT->Lchild, e);
    }
    //在右子树中继续查找
    else
    {
        keep++;
        return SearchDSTable(DT->Rchild, e);
    }
}

Status Widget::SearchDSTable2(BitTree DT,char *e, BitTree f, BitTree *p)
{
    //查找不成功
    if(!DT)
    {
        *p = f;

        return FALSE;
    }
    //查找成功
    else if (strcmp(e,DT->Node.word) == 0)
    {
        *p = DT;

        return TRUE;
    }
    //继续在左子树中查找
    else if (strcmp(e,DT->Node.word) < 0)
    {
        return SearchDSTable2(DT->Lchild, e, DT, p);
    }
    //继续在右子树中查找
    else
    {
        return SearchDSTable2(DT->Rchild, e, DT, p);
    }
}

Status Widget::InsertDSTable(BitTree *DT, SaveWord e)
{
    BitTree p, s;
    //查找不成功，插入到二叉树DT中
    if(!SearchDSTable2(*DT,e.word, NULL, &p))
    {
        s = (BitTree)malloc(sizeof(BitNode));
        //s->Node.savedate = (DateList)malloc(sizeof(SaveDate));
        s->Node = e;
        s->Node.wei = s->Node.savedate;
        s->Lchild = s->Rchild = NULL;
        s->Node.wordnum = number;
        if(!p)
        {
            *DT = s;    //被插结点*s为新的根结点
        }

        else if (strcmp(e.word,p->Node.word) < 0)
        {
            p->Lchild = s;    //被插结点*s为左孩子
        }

        else
        {
            p->Rchild = s;    //被插结点*s为右孩子
        }

        return TRUE;
    }
    //二叉树DT中已有与关键字相同的结点，不再插入
    else
    {
        DateList q = p->Node.wei;
        q->next = (SaveDate *)malloc(sizeof(SaveDate));
        p->Node.wei=q->next;
        q = q->next;
        q->Line = e.savedate->Line;
        q->Page = e.savedate->Page;
        q->next = NULL;
        p->Node.wordnum ++;
        return TRUE;
    }
}

void Widget::Delete(BitTree *DT)
{
    BitTree q,s;

    //右子树为空则只需重接它的左子树
    if(!(*DT)->Rchild)
    {
         q = *DT;
        *DT = (*DT)->Lchild;
        free(q);
    }
    //只需重接它的右子树
    else if(!(*DT)->Lchild)
    {
         q = *DT;
         *DT = (*DT)->Rchild;
        free(q);
    }

    else
    {
        q = *DT;
        s = (*DT)->Lchild;

        //转左，然后向右到尽头
        while(s->Rchild)
        {
            q = s;
            s = s->Rchild;
        }

        (*DT)->Node = s->Node;    //s指向被删结点的前驱


        if(q != *DT)
        {
            q->Rchild = s->Lchild;    //重接*q的右子树
        }

        else
        {
            q->Lchild = s->Lchild;    //重接*q的左子树
        }

        free(s);
    }
}

void Widget::TraverseDSTable(BitTree DT, void (*Visit)(SaveWord))
{
    if(DT)
        {
            TraverseDSTable(DT->Lchild, Visit);    //先遍历左子树
            Visit(DT->Node);    //再访问根结点
            TraverseDSTable(DT->Rchild, Visit);    //遍历右子树
        }
}

void Widget::print_message(BitTree DT)
{
    if(DT)
        {
            print_message(DT->Lchild);    //先遍历左子树
            ui->listWidget->addItem(tr("单词 ")+DT->Node.word+tr(" 出现了 %1 次").arg(DT->Node.wordnum));
            print_message(DT->Rchild);    //遍历右子树
        }
}

int Widget::isNull(char c)
{
    if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='\'')
        return 0;
    else
        return 1;
}

Status Widget::Opentxt(BitTree *DT)
{
    char c;
    DestroyDSTable(DT);
    QString name = ui->lineEdit_2->text();
    const char * hello = name.toStdString().data();
    int num = 0;
    int counter = 0;
    int k=1;
    FILE *fp;
    char str[100];
    SaveWord e;
    fp = fopen(hello,"r");

    if(fp ==NULL)
       {
        return FALSE;
       }

  M:
          while((c = fgetc(fp)) != EOF)
   {

       if(c=='\n')
       {
           k++;
           num = 0;
       }

       if( isNull(c) && (counter <= 0))
       { //消去非字母字符
           continue;
       }
        else if(isNull(c) && (counter > 0))
       { //一个单词读取完毕
            break;
       }
       str[counter++] = c;
   }
   if(c==EOF)
   {
       ui->label_2->setText(tr("当前检索文件为：")+hello);
       return TRUE;
   }
   str[counter] = '\0'; //成功返回SUCCESS 否则返回FAIL
   if(str[0]=='\'')
       for(int i = 0;str[i]!='\0';i++)
       {
           str[i]=str[i+1];
       }
   if(str[0]>='A'&&str[0]<='Z')
   {
       str[0] = str[0]+'a'-'A';
   }
   counter = 0;
   if(str[0]=='\0')
   {
       goto M;
   }
   num++;
   strcpy(e.word,str);
   e.savedate = (DateList)malloc(sizeof(SaveDate));
   e.savedate->Page = k;
   e.savedate->Line = num;
   e.savedate ->next = NULL;
   InsertDSTable(&(*DT),e);
   goto M;
}

int Widget::readBTree(BitTree *DT)
{
    FILE *fs;
    SaveWord e;
    char c;
    if((fs = fopen("save.txt","r"))==NULL)
    {
        return 0;
    }
    else
    {
        while((c=fgetc(fs))!=EOF)
        {
            fscanf(fs,"%s %d\n",e.word,&e.wordnum);
            e.savedate = (DateList)malloc(sizeof(SaveDate));
            e.savedate->next = NULL;
            DateList q = e.savedate;
            fscanf(fs,"%d %d",&q->Page,&q->Line);
            while((c=fgetc(fs))!='#')
            {
                q->next = (DateList)malloc(sizeof(SaveDate));
                q = q->next;
                q->next = NULL;
                fscanf(fs,"%d %d",&q->Page,&q->Line);
                number++;
            }
            fgetc(fs);
            InsertDSTable(&(*DT),e);
            number = 1;
        }
    }
    fclose(fs);
    return 1;
}

void Widget::CreateStaticTree(BitTree DT)
{
    if(DT)
        {
            CreateStaticTree(DT->Lchild);    //先遍历左子树
            word[num] = DT->Node;
            num++;
            CreateStaticTree(DT->Rchild);    //遍历右子树
        }
}

void Widget::on_pushButton_2_clicked()
{
    ui->listWidget->clear();
   if(BiTree_Static == 1)
    {
        print_message(dTable);
    }
   else if(BiTree_Static == 2)
    {
        int i;
        for(i=0;i<num;i++)
        {
          ui->listWidget->addItem(tr("单词 ")+word[i].word+tr(" 出现了 %1 次").arg(word[i].wordnum));
        }
    }    
}


void Widget::on_readtxt_clicked()
{
  ui->label_11->clear();
  ui->listWidget->clear();
  ui->label_7->clear();
  ui->label_8->clear();
  ui->label_14->clear();
  ui->label_15->clear();
  ui->listWidget_2->clear();
  num = 0;
  QTime t;
  t.start();
  if(num!=0)
      num = 0;
  if(Opentxt(&dTable))
  {
      int s =t.elapsed();
      ui->label_4->setText(tr("%1ms").arg(s));
      CreateStaticTree(dTable);
      s = t.elapsed();
      ui->label_5->setText(tr("%1ms").arg(s));
      ui->label->setText(tr("词典中总共有 %1 个单词").arg(num));
      QMessageBox::about(this,"","文件读取成功！        ");
      ui->lineEdit_2->clear();
  }
  else
  {
        ui->lineEdit_2->clear();
        QMessageBox::warning(this,"","文件读取失败！     ");
  }
}



void Widget::on_radioButton_2_toggled()
{
     BiTree_Static = 2;
     ui->listWidget->clear();
     ui->listWidget_2->clear();
     ui->label_11->clear();
}

void Widget::on_radioButton_toggled()
{
     BiTree_Static = 1;
     ui->listWidget->clear();
     ui->listWidget_2->clear();
     ui->label_11->clear();
}

int Widget::Search_Bin(SaveWord *word,char *e)
{
    //折半查找
    int low,high,mid;
    pep=0;
    low=0;
    high=num;
    while (low<=high)
    {
        mid=(low+high)/2;
        pep++;
        if (strcmp(e,word[mid].word)==0)
            return mid;
        else if (strcmp(e,word[mid].word)<0)
            high=mid-1;
        else
            low=mid+1;
    }
    return -1;
}

void Widget::on_pushButton_clicked()
{
    ui->listWidget_2->clear();
    char SearKey[100];
    int i;
    QString str = ui->lineEdit->text();
    for(i=0;str[i].toLatin1()!='\0';i++)
    {
        SearKey[i]=str[i].toLatin1();
    }
    SearKey[i]='\0';
    if(BiTree_Static == 1)
    {
        QTime t;
        t.start();
        BitTree searchResult;
        searchResult = SearchDSTable(dTable,SearKey);
        if(searchResult)
        {
            int a = t.elapsed();
            ui->label_14->setText(tr("%1ms").arg(a));
            int j = 0;
            ui->label_7->setText(tr("%1次").arg(keep));
            keep = 0;
            DateList q = searchResult->Node.savedate;
            ui->label_11->setText(tr("单词 ")+SearKey+tr(" 出现了 %1 次!").arg(searchResult->Node.wordnum));
            for(j = 0;j < searchResult->Node.wordnum;j++)
            {
              ui->listWidget_2->addItem(tr("第 %1 次出现在 line: %2  number: %3 ").arg(j+1).arg(q->Page).arg(q->Line));
              q = q->next;
            }
        }
        else
            ui->label_11->setText(tr("Can't find this word!"));
    }
    else if(BiTree_Static == 2)
    {
        QTime t;
        int a = t.elapsed();
        ui->label_15->setText(tr("%1ms").arg(a));
        int k = Search_Bin(word,SearKey);
        if(k>=0)
        {
         ui->label_8->setText(tr("%1次").arg(pep));
         ui->label_11->setText(tr("单词 ")+SearKey+tr(" 出现了 %1 次!").arg(word[k].wordnum));
         int i = 0;
         DateList q = word[k].savedate;
         for(i = 0;i < word[k].wordnum;i++)
            {
             ui->listWidget_2->addItem(tr("第 %1 次出现在 line: %2  number: %3 ").arg(i+1).arg(q->Page).arg(q->Line));
             q = q->next;
             }
         }
         else
         {
            ui->label_11->setText(tr("Can't find this word!"));
         }
    }
}

int Widget::saveSTree(SaveWord *word)
{
    FILE *fs;
    int i = 0;
    int k = 0;
    if((fs = fopen("save.txt","w+"))==NULL)
    {
       return 0;
    }
    else
    {
        for(k=0;k<num;k++)
        {
        fprintf(fs," %s %d\n",word[k].word,word[k].wordnum);
        DateList q = word[k].savedate;
        for(i=0;i<word[k].wordnum;i++)
        {
            fprintf(fs," %d %d",q->Page,q->Line);
            q = q->next;
        }
        fprintf(fs,"#\n");
        }
    }
    fclose(fs);
    return 1;
}

void Widget::on_pushButton_5_clicked()
{
   if(saveSTree(word))
       QMessageBox::about(this,"","文件保存成功！     ");
   else
       QMessageBox::warning(this,"","文件保存失败！     ");
}

void Widget::readSTree(SaveWord *word)
{
    FILE *fs;
    char c;
    QString name("save.txt");
    const char * hello = name.toStdString().data();
    if((fs = fopen(hello,"r"))==NULL)
    {
        QMessageBox::warning(this,"","文件恢复失败！     ");
    }
    else
    {
        while((c=fgetc(fs))!=EOF)
        {
            fscanf(fs,"%s %d\n",word[num].word,&word[num].wordnum);
            word[num].savedate = (DateList)malloc(sizeof(SaveDate));
            word[num].savedate->next = NULL;
            DateList q = word[num].savedate;
            fscanf(fs,"%d %d",&q->Page,&q->Line);
            while((c=fgetc(fs))!='#')
            {
                q->next = (DateList)malloc(sizeof(SaveDate));
                q = q->next;
                q->next = NULL;
                fscanf(fs,"%d %d",&q->Page,&q->Line);
            }
            fgetc(fs);
            num++;
        }
        QMessageBox::about(this,"","文件恢复成功！     ");
        ui->label_2->setText(tr("当前检索文件为：")+hello);
        ui->label->setText(tr("词典中总共有 %1 个单词").arg(num));
    }
    fclose(fs);
}

void Widget::on_pushButton_6_clicked()
{
    ui->label_11->clear();
    ui->listWidget->clear();
    ui->label_7->clear();
    ui->label_8->clear();
    ui->label_14->clear();
    ui->label_15->clear();
    ui->listWidget_2->clear();
    num = 0;
    QTime t;
    t.start();
    if(readBTree(&dTable))
    {
        int s = t.elapsed();
        ui->label_4->setText(tr("%1ms").arg(s));
        s = t.elapsed();
        readSTree(word);
        ui->label_5->setText(tr("%1ms").arg(s));
    }
    else
      QMessageBox::warning(this,"","文件恢复失败！     ");
}

void Widget::on_readtxt_2_clicked()
{
    QString path=QDir::currentPath();//获取程序当前目录
    path.replace("/","\\");//将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
    QProcess::startDetached("explorer "+path);//打开上面获取的目录
}
