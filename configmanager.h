#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "exceptionhierarchy.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QString>
#include <QHash>


/*
 *  TODO:
 *
 *      - Reduce boilerplatet,
 *      - Settle the matter of error passing style.
 *      - Change code to conform into settled error passing style.
 *      - Comments and doxygen.
 */





class IniParser
{
    /*
     *  I assume .ini file have following syntax:
     *   - one line == one statement
     *   - Statement is in one of the two following forms:
     *      #A comment which will be ignored in parsing process
     *      [whitespaces]constant_name[whitespaces]=[whitespaces]value[whitespaces]
     *   - The '=' and '#' signs reserved and should be present in a noncomment
     *     statement only once!
     *   - statements first part (the key) of the statement is not case sensitive
     *     and by default they will be perceived as being lowercase.
     *     Second part (the value) is case sensitive.
     *
     *  Everything which is not a equality statement parseINI ignores,
     *  giving a debug informations about nonstatement (when in debug mode).
     */
   static QVector<QPair<QString, QString> >
                            parseINI(const QStringList& lines)
   {
       QVector<QPair<QString, QString> > keyValuePairs;

       /*
        * Filter out comments:
        *      \s* <=> zero or more white space characters
        *      #   <=> a hash sign
        *      .*  <=> match zero or more times any character.
        */
       foreach(QString statement, lines)
       {
           statement = statement.trimmed();
           if(QRegExp("#.*").exactMatch(statement) || statement.isEmpty())
           {
           }
           else if(QRegExp("[^#=]+=[^#=]+").exactMatch(statement))
           {
               QVector<QString> keyValue =
                       QVector<QString>::fromList(statement.split(QRegExp("=")));
               keyValuePairs.push_back(
                           qMakePair(keyValue[0].trimmed().toLower(),
                                     keyValue[1].trimmed()));
           }
           else
               qDebug() << "[parseINI]: Not a statement " << statement;
       }
       return keyValuePairs;

   }

   template<typename Container>
   static void parseAndBuildFromLines(QStringList& lines,
                                      Container& container)
   {
       QPair<QString, QString> keyValue;
       foreach(keyValue, IniParser::parseINI(lines))
       {
           if(container.contains(keyValue.first))
           {
               qDebug() << "[INIParser] Key " << keyValue.first
                        << " duplicate found. Replacing with new value.";
           }
           container[keyValue.first] = keyValue.second;
       }
   }

public:

   template<typename Container>
   static void parseFile(const QString& pathToIni,
                               Container& container,
                               unsigned int chunkSizeInLines)
   {
       /*
        *  1. Open a file pathToFile
        *  2. Read all it's contents
        *  3. Parse it.
        *  4. Produce a Container containing configuration setup.
        */
       qDebug() << "[IniParser] Opening file " << pathToIni << "...";
       QFile iniFile(pathToIni);

       if(!iniFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
           qDebug() << "[IniParser] Error: file " << pathToIni
                    << " could not be open.";
           throw DummyException();
       }

       QTextStream iniStream(&iniFile);
       unsigned int linesRead = 0;
       QStringList lines;
       qDebug() << "[IniParser] Parsing...";
       while(!iniStream.atEnd())
       {

           lines += iniStream.readLine();
           linesRead++;
           if(linesRead == chunkSizeInLines)
           {
               IniParser::parseAndBuildFromLines(lines, container);
               linesRead = 0;
               lines.clear();
           }
       }
       IniParser::parseAndBuildFromLines(lines, container);
       qDebug() << "[IniParser] Parsing finished."
                << "[IniParser] Closing file " << pathToIni << "...";
       iniFile.close();
       qDebug() << "[IniParser] File closed.";
   }
};

/*
 *  ConfigManager reads configuration file from a given path.
 *  When it has a set of Key, Value pairs it can be queried
 *  in given manner:
 *
 *      int port = myConfig["SERVER PORT"];
 *      bool hasServerIp = myConfig.contains("SERVER IP");
 *
 *  It is ConfigManger's role to check whether VALUE held by a given
 *  key can be translated to an output type.
 *
 */

/*
 *  Fun fact:   You cannot partialy specialize template methods
 *              if without specializing class.
 *  Func fact2: You cannot partialy specialize template methods !
 *
 */

template<typename ConfigFormat = IniParser,
         typename Container = QHash<QString, QString> >
class ConfigManager
{
    Container m_Container;
public:
    ConfigManager(const QString& pathToConfigFile)
    {
        qDebug() << "[ConfigManager] Creating a config using "
                 << pathToConfigFile << ".";
        ConfigFormat::parseFile(pathToConfigFile, m_Container, 100);
        qDebug() << "[ConfigManager] Config created.";
    }

    QString operator[](const QString& key) const;
    int intAt(const QString&) const;
    bool contains(const QString&) const;
};

#endif // CONFIGMANAGER_H
