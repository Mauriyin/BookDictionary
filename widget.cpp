#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QTime>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
int keep = 0;
int num = 0;
int pep;
int hashwordnum = 0;
int bittreewordnum=0;
BitTree dTable = NULL;
int BiTree_hash = -1;
HashTree *HT = NULL;
int hushnum = 1;
char hashtxt[1000];
char bittxt[1000];

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->radioButton->toggle();
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
        s->Node.savedate = (DateList)malloc(sizeof(SaveDate));
        s->Node = e;
        s->Node.wei = s->Node.savedate;
        s->Lchild = s->Rchild = NULL;
        s->Node.wordnum = e.wordnum;
        num++;
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
        if(strcmp(bittxt,"SaveBitTree.txt")!=0)
        {
        DateList q = p->Node.wei;
        q->next = (SaveDate *)malloc(sizeof(SaveDate));
        p->Node.wei=q->next;
        q = q->next;
        q->Line = e.savedate->Line;
        q->Page = e.savedate->Page;
        q->next = NULL;
        p->Node.wordnum ++;
        }
        return TRUE;
    }
}

void Widget::print_message(BitTree DT)
{
    if(DT)
        {
            print_message(DT->Lchild);    //先遍历左子树
            ui->listWidget->addItem(tr("单词 ")+DT->Node.word+tr(" 出现了 %1 次").arg(DT->Node.wordnum));
             bittreewordnum ++;
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
    const char *hello = name.toStdString().data();
    strcpy(bittxt,hello);
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
       fclose(fp);
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
   e.wordnum = 1;
   e.savedate = (DateList)malloc(sizeof(SaveDate));
   e.savedate->Page = k;
   e.savedate->Line = num;
   e.savedate ->next = NULL;
   InsertDSTable(&(*DT),e);
   goto M;
}

bool Widget::readBTree(BitTree *DT)
{
    FILE *fs;
    SaveWord e;
    char c;
    char name[]="SaveBitTree.txt";
    strcpy(bittxt,name);
    if((fs = fopen("SaveBitTree.txt","r"))==NULL)
    {
        return 0;
    }
    else
    {
        while((c=fgetc(fs))!=EOF)
        {
            fscanf(fs,"%s %d",e.word,&e.wordnum);
            e.savedate = (DateList)malloc(sizeof(SaveDate));
            fscanf(fs,"%d %d",&e.savedate->Line,&e.savedate->Page);
            e.wei=e.savedate;
            e.wei->next=NULL;
            while((c=fgetc(fs))!='#')
            {
                e.wei->next = (DateList)malloc(sizeof(SaveDate));
                e.wei=e.wei->next;
                e.wei->next = NULL;
                fscanf(fs,"%d %d",&e.wei->Line,&e.wei->Page);
            }
           InsertDSTable(&(*DT),e);
           fgetc(fs);
        }
    }
    fclose(fs);
    return 1;
}

void Widget::InitHash()
{

    for(int i =0;i<hushnum;i++)
    {
        HT[i]=(HashTree)malloc(sizeof(HashNode));
        HT[i]->wordnum = 0;
        HT[i]->next = NULL;
        HT[i]->first = NULL;
    }
}

void Widget::Print_Hash()
{
    for(int i = 0;i<hushnum;i++)
    {
        HashTree p = HT[i];
        while(p->next!=NULL)
        {
            ui->listWidget->addItem(tr("单词 ")+p->word+tr(" 出现了 %1 次").arg(p->wordnum));
            hashwordnum++;
            p = p->next;
        }
    }
   ui->label->setText(tr("词典中总共有 %1 个单词").arg(hashwordnum));
   hashwordnum = 0;
}

void Widget::InsertHash(HashNode e)
{
    int v = GetHashNum(e.word);
    HashTree p = HT[v];
    bool REPE = false;
    while(p->next!=NULL)
    {
       if(strcmp(p->word,e.word)==0)
       {
           if(p->first == NULL)
           {
               p->first = (Repeat *)malloc(sizeof(Repeat));
               p->tail = p->first;
               p->first->next =NULL;;
               p->first->line =  e.line;
               p->first->num = e.nuber;
               p->wordnum++;
           }
           else
           {
               p->tail->next=(Repeat *)malloc(sizeof(Repeat));
               p->tail=p->tail->next;
               p->tail->line = e.line;
               p->tail->num = e.nuber;
               p->tail->next = NULL;
               p->wordnum++;
          }
         REPE = true;
       }
        p=p->next;
    }
    if(REPE==false)
    {
    strcpy(p->word,e.word);
    p->line = e.line;
    p->nuber = e.nuber;
    p->wordnum ++;
    p->next = (HashTree)malloc(sizeof(HashNode));
    p->next->wordnum = 0;
    p->next->next = NULL;
    p->next->first = NULL;
    }
}

int Widget::GetHashNum(char *word)
{
    int hash = 0;
    int i;
    for (i=0; *word; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ (*word++) ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ (*word++) ^ (hash >> 5)));
        }
    }
    return (hash & 0x7FFFFFFF)%hushnum;
}

Status Widget::OpenHash()
{
    char c;
    QString name = ui->lineEdit_2->text();
    const char *hello = name.toStdString().data();
    strcpy(hashtxt,hello);
    int num = 0;
    int counter = 0;
    int k=1;
    FILE *fp;
    char str[100];
    fp = fopen(hello,"r");
    HashNode e;
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
   e.line = k;
   e.nuber = num;
   InsertHash(e);
   goto M;
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
  QTime h;
  t.start();
  if(BiTree_hash == 1)
  {
    if(Opentxt(&dTable))
    {
        int s =t.elapsed();
        ui->label_4->setText(tr("%1ms").arg(s));
        QMessageBox::about(this,"","二叉树文件读取成功！        ");
        ui->lineEdit_2->clear();
    }
    else
    {
        QMessageBox::warning(this,"","二叉树文件读取失败！        ");
        ui->lineEdit_2->clear();
    }

  }
  else if(BiTree_hash == 2)
  {
      hushnum = 103001;
      HT =(HashTree *)malloc(sizeof(HashTree)*hushnum);
      InitHash();
      h.start();
      if(OpenHash()==TRUE)
      {
          int s =h.elapsed();
          ui->label_5->setText(tr("%1ms").arg(s));
          //ui->label->setText(tr("词典中总共有 %1 个单词").arg(num));
          QMessageBox::about(this,"","哈希表文件读取成功！        ");
          ui->lineEdit_2->clear();
      }
      else
      {
          QMessageBox::warning(this,"","哈希表文件读取失败！        ");
          ui->lineEdit_2->clear();
      }
  }
}



void Widget::on_radioButton_2_toggled()
{
     BiTree_hash = 2;
     ui->label_2->setText(tr("当前检索文件为：")+hashtxt);
     ui->listWidget->clear();
     ui->listWidget_2->clear();
     ui->label_11->clear();
     ui->label->clear();
}

void Widget::on_radioButton_toggled()
{
     BiTree_hash = 1;
     ui->label_2->setText(tr("当前检索文件为：")+bittxt);
     ui->listWidget->clear();
     ui->listWidget_2->clear();
     ui->label->clear();
     ui->label_11->clear();
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
    if(BiTree_hash == 1)
    {
        QTime t;
        t.start();
        BitTree searchResult;
        searchResult = SearchDSTable(dTable,SearKey);
        if(searchResult)
        {
            int a = t.elapsed();
            ui->label_14->setText(tr("%1ms").arg(a));
            ui->label_7->setText(tr("%1次").arg(keep));
            keep = 0;
            DateList q = searchResult->Node.savedate;
            ui->label_11->setText(tr("单词 ")+SearKey+tr(" 出现了 %1 次!").arg(searchResult->Node.wordnum));
            for(int i=0;i<searchResult->Node.wordnum;i++)
            {
              ui->listWidget_2->addItem(tr("第 %1 次出现在 line: %2  number: %3 ").arg(i+1).arg(q->Page).arg(q->Line));
              q = q->next;
            }
        }
        else
            QMessageBox::warning(this,"","Can't find this word!     ");
    }
    else if(BiTree_hash == 2)
    {
        int ah=GetHashNum(SearKey);
        HashTree p =HT[ah];
        bool find=false;
        int bi=0;
        QTime t;
        t.start();
        while(p->next!=NULL)
        {
            bi++;
            if(strcmp(p->word,SearKey)==0)
            {
                int j=2;
                int s = t.elapsed();
                ui->label_15->setText(tr("%1ms").arg(s));
                ui->label_11->setText(tr("单词 ")+SearKey+tr(" 出现了 %1 次!").arg(p->wordnum));
                ui->listWidget_2->addItem(tr("第 1 次出现在 line: %2  number: %3 ").arg(p->line).arg(p->nuber));
                Repeat *q=p->first;
                while(q!=NULL)
                {
                  ui->listWidget_2->addItem(tr("第 %1 次出现在 line: %2  number: %3 ").arg(j).arg(q->line).arg(q->num));
                  q=q->next;
                  j++;
                }
                ui->label_8->setText(tr("%1次").arg(bi));
                find = true;
                break;
            }
            p=p->next;
        }
    if(find==false)
         QMessageBox::warning(this,"","Can't find this word!     ");
    }
}

bool Widget::savbittree(BitTree DT)
{
  if(DT)
  {
    savbittree(DT->Lchild);
    FILE *fs;
    int i = 0;
    if((fs = fopen("SaveBitTree.txt","a"))==NULL)
    {
        return false;
    }
    else
    {
        fprintf(fs," %s %d\n", DT->Node.word, DT->Node.wordnum);
        DateList q = DT->Node.savedate;
        for(i=0;i<DT->Node.wordnum;i++)
        {
            fprintf(fs," %d %d",q->Page,q->Line);
            q = q->next;
        }
        fprintf(fs,"#\n");
    }
    fclose(fs);
    savbittree(DT->Rchild);
   }
   return true;
}

bool Widget::savehash()
{
    FILE *fs;
    int i ;
    if((fs = fopen("SaveHash.txt","w"))==NULL)
    {
        return false;
    }
    for(i=0;i<hushnum;i++)
    {
       HashTree p =HT[i];
       while(p->next!=NULL)
       {
            fprintf(fs," %s\n",p->word);
            fprintf(fs," %d %d",p->line,p->nuber);
            Repeat *q=p->first;
            while(q!=NULL)
            {
              fprintf(fs," %d %d",q->line,q->num);
              q=q->next;
            }
            fprintf(fs,"#\n");
            p=p->next;
        }

    }
    fclose(fs);
    return true;
}

void Widget::on_pushButton_5_clicked()
{
    if(BiTree_hash==1)
    {
        FILE *fp;
        fp = fopen("SaveBitTree.txt","w");
        fclose(fp);
        savbittree(dTable);
        if(savbittree(dTable)==true)
            QMessageBox::about(this,"","二叉树文件保存成功！     ");
        else
            QMessageBox::warning(this,"","二叉树文件保存失败！     ");
    }
   else
    {
        if(savehash()==true)
            QMessageBox::about(this,"","哈希表文件保存成功！     ");
        else
            QMessageBox::warning(this,"","哈希表文件保存失败！     ");
    }
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
    QTime h;
    t.start();
    if(BiTree_hash==1)
    {
        DestroyDSTable(&dTable);
        if(readBTree(&dTable))
        {
            int s = t.elapsed();
            ui->label_4->setText(tr("%1ms").arg(s));
            QMessageBox::about(this,"","二叉树文件恢复成功！     ");
            ui->label_2->setText(tr("当前检索文件为：")+bittxt);
        }
        else
          QMessageBox::warning(this,"","二叉树文件恢复失败！     ");
    }
    else
    {
        h.start();
        if(readhash()==true)
        {
            int s = t.elapsed();
            ui->label_5->setText(tr("%1ms").arg(s));
            QMessageBox::about(this,"","哈希表文件恢复成功！     ");
            char name[]="SaveHash.txt";
            strcpy(hashtxt,name);
            ui->label_2->setText(tr("当前检索文件为：")+hashtxt);
        }
        else
          QMessageBox::warning(this,"","哈希表文件恢复失败！     ");
    }
}

bool Widget::readhash()
{
    FILE *fs;
    char c;
    char word[100];
    if((fs = fopen("SaveHash.txt","r"))==NULL)
    {
        return false;
    }
    HT=(HashTree *)malloc(sizeof(HashTree)*hushnum);
    InitHash();
    while((c=fgetc(fs))!=EOF)
    {
       fscanf(fs,"%s",word);
       int n=GetHashNum(word);
       HashTree p = HT[n];
       while(p->next!=NULL)
       {
           p=p->next;
       }
       strcpy(p->word,word);
       fscanf(fs,"%d %d",&p->line,&p->nuber);
       p->next=(HashTree)malloc(sizeof(HashNode));
       p->next->wordnum=0;
       p->next->first=NULL;
       p->wordnum ++;
       while((c=fgetc(fs))!='#')
       {
           if(p->first == NULL)
           {
               p->first = (Repeat *)malloc(sizeof(Repeat));
               p->tail = p->first;
               p->first->next =NULL;
               fscanf(fs,"%d %d",&p->tail->line,&p->tail->num);
               p->wordnum++;
           }
           else
           {
               p->tail->next=(Repeat *)malloc(sizeof(Repeat));
               p->tail=p->tail->next;
               fscanf(fs,"%d %d",&p->tail->line,&p->tail->num);
               p->tail->next = NULL;
               p->wordnum++;
           }
       }
       fgetc(fs);
       p->next->next=NULL;
    }
    fclose(fs);
    return true;
}

void Widget::on_readtxt_2_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,tr("File Path"),"/");
    ui->lineEdit_2->setText(path);

}


void Widget::on_pushButton_2_clicked()
{
    if(BiTree_hash==1)
    {
        print_message(dTable);
        ui->label->setText(tr("词典中总共有 %1 个单词").arg(bittreewordnum));
        bittreewordnum = 0;
    }
    else
        Print_Hash();
}
