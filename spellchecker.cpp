#include "spellchecker.h"
#include "ui_spellchecker.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <bits/stdc++.h>

using namespace std;

spellchecker::spellchecker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::spellchecker)
{
    ui->setupUi(this);
    get_text();
}

spellchecker::~spellchecker()
{
    delete ui;
}
node *spellchecker::findkey(node *Node, string word, int ind)
{
    if(Node == NULL) return Node;

    if(ind == word.length()) return Node;

    return findkey(Node->links[word[ind]], word, ind+1);
}
node *spellchecker::insert(node *Node, string word, int val, int ind)
{
    if(Node == NULL)
    {
        Node = new node();
    }

    if(ind == word.length())
    {
        Node->val = 1;
        return Node;
    }
    char ch = word[ind];
    Node->links[ch] = insert(Node->links[ch], word, val, ind+1);

    return Node;
}
node *spellchecker::util(node *Node, string word, string &commonprefix, int ind)
{
    if(Node->links[word[ind]] == NULL)
        return Node;
    if(ind == word.length())
        return Node;
    commonprefix+=word[ind];
    return util(Node->links[word[ind]], word, commonprefix, ind+1);


}
node *spellchecker::getcommonprefix(node *Node, string word, string &commonprefix)
{
    return util(Node, word, commonprefix, 0);
}
void spellchecker::collect(node *Node, string commonprefix, deque<string> &sugg)
{
    if(Node == NULL)
    {
        return;
    }
    if(Node->val == 1)
    {
        sugg.push_back(commonprefix);
    }
    for(int i = 0; i < 256; ++i)
    {
            collect(Node->links[i], commonprefix+(char) i, sugg);
    }

}
QStringList spellchecker::suggestions(node *root, string word)
{
    string commonprefix = "";
    node *Node = getcommonprefix(root, word, commonprefix);
    deque<string> sugg;
    collect(Node, commonprefix, sugg);
    QStringList sugg_list;
    while(!sugg.empty())
    {
        string temp = sugg.front();
        QString qstr = QString::fromStdString(temp);
        sugg_list << qstr;
        sugg.pop_front();
    }
    return sugg_list;
}

void spellchecker::get_text()
{

    QFile myFile(":/words_file/words.txt");
    myFile.open(QIODevice::ReadOnly);

    QTextStream textStream(&myFile);
    QString line = textStream.readAll();

    ui->textEdit_2->setPlainText(line);
    QTextCursor textCursor = ui->textEdit_2->textCursor();
    textCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);

    QStringList _list = line.split("\n");

    for(int i = 0; i < _list.length(); ++i)
    {
        QString temp = _list.at(i);
        string word = temp.toStdString();
        root = insert(root, word, 1, 0);
    }
     myFile.close();
}

void spellchecker::on_pushButton_2_clicked()
{
    QString word = ui->lineEdit_2->text();

    string searchkey = word.toStdString();
    node *temp = findkey(root, searchkey, 0);
    if(temp != NULL and temp->val == 1)
    {
        ui->textEdit->setFontWeight( 100 );
        ui->textEdit->setTextColor( QColor( "green" ) );
        ui->textEdit->setPlainText("It is in the Dictionary");
        ui->textEdit_2->find(word, QTextDocument::FindWholeWords);
    }
    else
    {
           QStringList sugg_list = suggestions(root, searchkey);
           ui->textEdit->setFontWeight( 100 );
           ui->textEdit->setTextColor( QColor( "red" ) );
           ui->textEdit->setPlainText("It is not in the Dictionary.....Probably spelled incorrectly\n");
           ui->textEdit->setTextColor( QColor("black"));
           ui->textEdit->append("Suggestions are:\n");
           for(int i = 0; i <sugg_list.length(); ++i)
           {
               QString temp = sugg_list.at(i);
               string word = temp.toStdString();
               ui->textEdit->append(temp);
           }
    }

}
