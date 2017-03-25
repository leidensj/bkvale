#include "databaseutils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>

Database::Database()
  : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{

}

bool Database::isOpen(QString& error) const
{
  error.clear();
  if (!m_db.isOpen())
     error = "Arquivo de configuração não foi aberto.";
  return m_db.isOpen();
}

bool Database::open(const QString& path,
                    QString& error)
{
  error.clear();
  if (m_db.isOpen())
    m_db.close();
  m_db.setDatabaseName(path);
  bool bSuccess = m_db.open();
  if (!bSuccess)
    error = m_db.lastError().text();
  return bSuccess;
}

void Database::close()
{
  m_db.close();
}

bool Database::init(QString& error)
{
  error.clear();

  if (!isOpen(error))
    return false;

  QSqlQuery query;
  bool bSuccess = query.exec("CREATE TABLE IF NOT EXISTS _SETTINGS ("
                             "_LASTNUMBER INTEGER DEFAULT " DEFAULT_NUMBER_STR ","
                             "_SERIALPORT TEXT)");
  if (bSuccess)
  {
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS _PROMISSORYNOTES ("
                          "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "_NUMBER INTEGER NOT NULL,"
                          "_DATE INTEGER NOT NULL,"
                          "_SUPPLIER TEXT NOT NULL,"
                          "_ITEMS TEXT,"
                          "_TOTAL REAL)");
    if (bSuccess)
    {
      bSuccess = query.exec("CREATE TABLE IF NOT EXISTS _ITEMS ("
                            "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "_DESCRIPTION TEXT NOT NULL UNIQUE)");
    }
  }

  if (bSuccess && !hasConfig())
    bSuccess = query.exec("INSERT INTO _SETTINGS DEFAULT VALUES");

  if (!bSuccess)
    error = query.lastError().text();
  return bSuccess;
}

bool Database::insert(const Note& note,
                      QStringList itemDescriptions,
                      QString& error)
{
  error.clear();

  if (!isOpen(error))
    return false;

  QSqlQuery query;
  query.prepare("INSERT INTO _PROMISSORYNOTES ("
                "_NUMBER,"
                "_DATE,"
                "_SUPPLIER,"
                "_ITEMS,"
                "_TOTAL) "
                "VALUES ("
                "(:_number),"
                "(:_date),"
                "(:_supplier),"
                "(:_items),"
                "(:_total))");

  query.bindValue(":_number", note.m_number);
  query.bindValue(":_date", note.m_date);
  query.bindValue(":_supplier", note.m_supplier);
  query.bindValue(":_items", note.m_items);
  query.bindValue(":_total", note.m_total);

  bool bSuccess = query.exec();
  if (!bSuccess)
  {

    error = query.lastError().text();
  }
  else
  {
    incNumber();
    insertItemDescriptions(itemDescriptions);
  }

  return bSuccess;
}

bool Database::select(int id,
                      Note& note,
                      QString& error)
{
  error.clear();
  note.clear();

  if (!isOpen(error))
    return false;

  QSqlQuery query;
  query.prepare("SELECT "
                "_NUMBER,"
                "_DATE,"
                "_SUPPLIER,"
                "_ITEMS,"
                "_TOTAL "
                "FROM _PROMISSORYNOTES "
                "WHERE _ID = (:_id)");

  query.bindValue(":_id", id);

  bool bSuccess = query.exec();
  if (bSuccess)
  {
    bSuccess = query.next();
    if (bSuccess)
    {
       note.m_number = query.value(query.record().indexOf("_NUMBER")).toInt();
       note.m_date = query.value(query.record().indexOf("_DATE")).toLongLong();
       note.m_supplier = query.value(query.record().indexOf("_SUPPLIER")).toString();
       note.m_items = query.value(query.record().indexOf("_ITEMS")).toString();
       note.m_total = query.value(query.record().indexOf("_TOTAL")).toDouble();
    }
    else
    {
      error = "Vale não encontrado.";
    }
  }
  else
  {
    error = query.lastError().text();
  }

  return bSuccess;
}

int Database::number()
{
  QString error;
  if (!isOpen(error))
    return DEFAULT_NUMBER;

  QSqlQuery query("SELECT _LASTNUMBER FROM _SETTINGS");
  int idx = query.record().indexOf("_LASTNUMBER");
  return query.next() ? query.value(idx).toInt() : DEFAULT_NUMBER;
}

void Database::incNumber()
{
  int n = number() + 1;
  QSqlQuery query;
  query.prepare("UPDATE _SETTINGS SET _LASTNUMBER = :_number");
  query.bindValue(":_number", n);
  query.exec();
}

bool Database::hasConfig()
{
  QString error;
  if (!isOpen(error))
    return false;

  QSqlQuery query;
  if (query.exec("SELECT * FROM _SETTINGS LIMIT 1"))
    return query.next();

  return false;
}

bool Database::selectAll(Notes& notes,
                         QString& error)
{
  error.clear();
  notes.clear();

  if (!isOpen(error))
    return false;

  QSqlQuery query;
  bool bSuccess = query.exec("SELECT "
                             "_ID,"
                             "_NUMBER,"
                             "_DATE,"
                             "_SUPPLIER,"
                             "_ITEMS,"
                             "_TOTAL "
                             "FROM _PROMISSORYNOTES");

  if (bSuccess)
  {
    while (query.next())
    {
      notes.emplace_back(Note(query.value(query.record().indexOf("_ID")).toInt(),
                              query.value(query.record().indexOf("_NUMBER")).toInt(),
                              (qint64)query.value(query.record().indexOf("_DATE")).toLongLong(),
                              query.value(query.record().indexOf("_SUPPLIER")).toString(),
                              query.value(query.record().indexOf("_ITEMS")).toString(),
                              query.value(query.record().indexOf("_TOTAL")).toDouble()));
    }
  }
  else
  {
    error = query.lastError().text();
  }

  return bSuccess;
}

QStringList Database::selectSuppliers()
{
  QStringList list;
  QString error;
  if (isOpen(error))
  {
    QSqlQuery query;
    if (query.exec("SELECT DISTINCT "
                   "_SUPPLIER "
                   "FROM _PROMISSORYNOTES"))
    {
      while (query.next())
        list << query.value(query.record().indexOf("_SUPPLIER")).toString();
    }
  }
  return list;
}

void Database::insertItemDescriptions(const QStringList& itemDescriptions)
{
  for (int i = 0; i != itemDescriptions.size(); ++i)
  {
    QSqlQuery query;
    query.prepare("INSERT INTO _ITEMS ("
                  "_DESCRIPTION) "
                  "VALUES ("
                  "(:_description))");

    query.bindValue(":_description", itemDescriptions.at(i));
    query.exec();
  }
}

QStringList Database::selectItemDescriptions()
{
  QStringList list;
  QString error;
  if (isOpen(error))
  {
    QSqlQuery query;
    if (query.exec("SELECT DISTINCT "
                   "_DESCRIPTION "
                   "FROM _ITEMS"))
    {
      while (query.next())
        list << query.value(query.record().indexOf("_DESCRIPTION")).toString();
    }
  }
  return list;
}
