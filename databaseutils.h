#ifndef DATABASEUTILS_H
#define DATABASEUTILS_H

#include <QSqlDatabase>
#include <cons.h>

class Database
{
  QSqlDatabase m_db;

public:

  Database();

  bool isOpen(QString& error);

  bool open(const QString& path,
            QString& error);

  bool createTables(QString& error);

  bool insertPromissoryNote(PromissoryNote note,
                            QString& error);
};
#endif // DATABASEUTILS_H
