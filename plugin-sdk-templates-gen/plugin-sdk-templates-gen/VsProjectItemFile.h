#pragma once
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "ErrorMessage.h"

class VsProjectItemFile {
    QStringList lines;
public:
    bool Read(QString fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
            return MESSAGE_ERROR("VsXmlFile::Read(): Can't open file \"" + fileName + '"');
        QTextStream ts(&file);
        while (true) {
            QString line = ts.readLine();
            if (line.isNull())
                break;
            else
                lines.append(line);
        }
        file.close();
        return true;
    }

    bool Write(QString fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
            return MESSAGE_ERROR("VsXmlFile::Write(): Can't open file \"" + fileName + '"');
        QTextStream ts(&file);
        unsigned int linesCount = lines.size();
        if (linesCount > 1) {
            for (unsigned int i = 0; i < linesCount - 1; i++)
                ts << lines[i] << '\n';
        }
        ts << lines.last();
        file.close();
        return true;
    }

    QString GetLineValue(unsigned int index) {
        if (index > lines.size())
            return QString();
        return lines[index];
    }

    bool SetLineValue(unsigned int index, QString value) {
        if (index > lines.size())
            return false;
        lines[index] = value;
        return index;
    }

    bool SetLinesValue(QString oldValue, QString newValue) {
        unsigned int oldLengh = oldValue.length();
        for (QString &line : lines) {
            if (!line.trimmed().left(oldLengh).compare(oldValue, Qt::CaseInsensitive)) {
                size_t numSpaces = line.toStdString().find_first_not_of(" \t");
                QString newLine;
                if (numSpaces > 0)
                    newLine.fill(' ', numSpaces);
                line = newValue;
            }
        }
        return true;
    }

    unsigned int FindNodesCount(QString nodeName) {
        unsigned int counter = 0;
        QString tag = "<" + nodeName + ">";
        unsigned int tagLengh = tag.length();
        for (QString &line : lines) {
            if (!line.trimmed().left(tagLengh).compare(tag, Qt::CaseInsensitive))
                ++counter;
        }
        return counter;
    }

    bool SetNodesValue(QString nodeName, QString value) {
        QString tag = "<" + nodeName + ">";
        unsigned int tagLengh = tag.length();
        for (QString &line : lines) {
            if (!line.trimmed().left(tagLengh).compare(tag, Qt::CaseInsensitive)) {
                size_t numSpaces = line.toStdString().find_first_not_of(" \t");
                QString newLine;
                if (numSpaces > 0)
                    newLine.fill(' ', numSpaces);
                newLine += tag + value + "</" + nodeName + ">";
                line = newLine;
            }
        }
        return true;
    }

    bool SetNodesValue(QString nodeName, QString oldValueSearchSubstring, bool contains, QString value) {
        QString tag = "<" + nodeName + ">";
        unsigned int tagLengh = tag.length();
        for (QString &line : lines) {
            if (!line.trimmed().left(tagLengh).compare(tag, Qt::CaseInsensitive)
                && line.contains(oldValueSearchSubstring, Qt::CaseInsensitive) == contains)
            {
                size_t numSpaces = line.toStdString().find_first_not_of(" \t");
                QString newLine;
                if (numSpaces > 0)
                    newLine.fill(' ', numSpaces);
                newLine += tag + value + "</" + nodeName + ">";
                line = newLine;
            }
        }
        return true;
    }
};
