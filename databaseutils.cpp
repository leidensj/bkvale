#include "databaseutils.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>

#define DATABASE_NOT_OPEN_TXT "O arquivo de configuração não foi aberto."

int NoteSQL::nextNumber(QSqlDatabase db)
{
  if (!db.isOpen())
    return -1;

  QSqlQuery query("SELECT MAX(_NUMBER) FROM _PROMISSORYNOTES", db);
  int number = query.next() ? query.value(0).toInt() + 1 : DEFAULT_NUMBER;
  return number > DEFAULT_NUMBER ? number : DEFAULT_NUMBER;
}

QStringList NoteSQL::suppliers(QSqlDatabase db)
{
  QStringList list;
  if (db.isOpen())
  {
    QSqlQuery query(db);
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

bool NoteSQL::insert(QSqlDatabase db,
                     const Note& note,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  db.transaction();
  QSqlQuery query(db);

  query.exec("SELECT MAX(_NUMBER) FROM _PROMISSORYNOTES");
  int number = query.next()
               ? query.value(0).toInt() + 1
               : DEFAULT_NUMBER;
  number = number > DEFAULT_NUMBER ? number : DEFAULT_NUMBER;

  query.prepare("INSERT INTO _PROMISSORYNOTES ("
                "_NUMBER,"
                "_DATE,"
                "_SUPPLIER,"
                "_TOTAL,"
                "_CASH) "
                "VALUES ("
                "(:_number),"
                "(:_date),"
                "(:_supplier),"
                "(:_total),"
                "(:_cash))");
  query.bindValue(":_number", number);
  query.bindValue(":_date", note.m_date);
  query.bindValue(":_supplier", note.m_supplier);
  query.bindValue(":_total", note.m_total);
  query.bindValue(":_cash", note.m_bCash);
  query.exec();

  note.m_id = query.lastInsertId().toInt();

  for (int i = 0; i != note.m_items.size(); ++i)
  {
    query.prepare("INSERT INTO _PROMISSORYNOTESITEMS ("
                  "_NOTEID,"
                  "_AMMOUNT,"
                  "_PRICE,"
                  "_UNITY,"
                  "_DESCRIPTION) "
                  "VALUES ("
                  "(:_noteid),"
                  "(:_ammount),"
                  "(:_price),"
                  "(:_unity),"
                  "(:_description))");
    query.bindValue(":_noteid", note.m_id);
    query.bindValue(":_ammount", note.m_items.at(i).m_ammount);
    query.bindValue(":_price", note.m_items.at(i).m_price);
    query.bindValue(":_unity", note.m_items.at(i).m_unity);
    query.bindValue(":_description", note.m_items.at(i).m_description);
    query.exec();
    note.m_items.at(i).m_id = query.lastInsertId().toInt();
  }

  if (db.commit())
    return true;

  error = query.lastError().text();
  return false;
}

bool NoteSQL::update(QSqlDatabase db,
                     const Note& note,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  db.transaction();
  QSqlQuery query(db);
  query.prepare("UPDATE _PROMISSORYNOTES SET "
                "_DATE = (:_date),"
                "_SUPPLIER = (:_supplier),"
                "_TOTAL = (:_total),"
                "_CASH = (:_cash) "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", note.m_id);
  query.bindValue(":_date", note.m_date);
  query.bindValue(":_supplier", note.m_supplier);
  query.bindValue(":_total", note.m_total);
  query.bindValue(":_cash", note.m_bCash);
  query.exec();

  {
    QString strQuery = "DELETE FROM _PROMISSORYNOTESITEMS "
                       "WHERE _NOTEID = (:_noteid) AND "
                       "_ID NOT IN (";
    for (int i = 0; i != note.m_items.size(); ++i)
      strQuery += "(:_id" + QString::number(i) + "),";
    strQuery.replace(strQuery.length() - 1, 1, ")");
    query.prepare(strQuery);
  }

  query.bindValue(":_noteid", note.m_id);
  for (int i = 0; i != note.m_items.size(); ++i)
    query.bindValue("_id" + QString::number(i), note.m_items.at(i).m_id);
  query.exec();

  for (int i = 0; i != note.m_items.size(); ++i)
  {
    if (Note::isValidID(note.m_items.at(i).m_id))
    {
      query.prepare("UPDATE _PROMISSORYNOTESITEMS SET "
                    "_NOTEID = (:_noteid),"
                    "_AMMOUNT = (:_ammount),"
                    "_PRICE = (:_price),"
                    "_UNITY = (:_unity),"
                    "_DESCRIPTION = (:_description) "
                    "WHERE _ID = (:_id)");
      query.bindValue(":_id", note.m_items.at(i).m_id);
      query.bindValue(":_noteid", note.m_id);
      query.bindValue(":_ammount", note.m_items.at(i).m_ammount);
      query.bindValue(":_price", note.m_items.at(i).m_price);
      query.bindValue(":_unity", note.m_items.at(i).m_unity);
      query.bindValue(":_description", note.m_items.at(i).m_description);
      query.exec();
    }
    else
    {
      query.prepare("INSERT INTO _PROMISSORYNOTESITEMS ("
                    "_NOTEID,"
                    "_AMMOUNT,"
                    "_PRICE,"
                    "_UNITY,"
                    "_DESCRIPTION) "
                    "VALUES ("
                    "(:_noteid),"
                    "(:_ammount),"
                    "(:_price),"
                    "(:_unity),"
                    "(:_description))");
      query.bindValue(":_noteid", note.m_id);
      query.bindValue(":_ammount", note.m_items.at(i).m_ammount);
      query.bindValue(":_price", note.m_items.at(i).m_price);
      query.bindValue(":_unity", note.m_items.at(i).m_unity);
      query.bindValue(":_description", note.m_items.at(i).m_description);
      query.exec();
      note.m_items.at(i).m_id = query.lastInsertId().toInt();
    }

  }

  if (db.commit())
    return true;

  error = query.lastError().text();
  return false;
}

bool NoteSQL::select(QSqlDatabase db,
                     Note& note,
                     int& number,
                     QString& error)
{
  error.clear();
  int id = note.m_id;
  note.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  bool bFound = false;
  db.transaction();
  QSqlQuery query(db);
  query.prepare("SELECT "
                "_NUMBER,"
                "_DATE,"
                "_SUPPLIER,"
                "_TOTAL,"
                "_CASH "
                "FROM _PROMISSORYNOTES "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);
  query.exec();
  if (query.next())
  {
    note.m_id = id;
    number = query.value(query.record().indexOf("_NUMBER")).toInt();
    note.m_date = query.value(query.record().indexOf("_DATE")).toLongLong();
    note.m_supplier = query.value(query.record().indexOf("_SUPPLIER")).toString();
    note.m_total = query.value(query.record().indexOf("_TOTAL")).toDouble();
    note.m_bCash = query.value(query.record().indexOf("_CASH")).toBool();
    bFound = true;
  }

  query.prepare("SELECT "
                "_ID,"
                "_AMMOUNT,"
                "_PRICE,"
                "_UNITY,"
                "_DESCRIPTION "
                "FROM _PROMISSORYNOTESITEMS "
                "WHERE _NOTEID = (:_noteId)");
  query.bindValue(":_noteId", id);
  query.exec();
  while (query.next())
  {
    NoteItem item;
    item.m_id = query.value(query.record().indexOf("_ID")).toInt();
    item.m_ammount = query.value(query.record().indexOf("_AMMOUNT")).toDouble();
    item.m_price = query.value(query.record().indexOf("_PRICE")).toDouble();
    item.m_unity = query.value(query.record().indexOf("_UNITY")).toString();
    item.m_description = query.value(query.record().indexOf("_DESCRIPTION")).toString();
    note.m_items.push_back(item);
  }

  if (db.commit())
  {
    if (!bFound)
      error = "Vale não encontrado.";
    return bFound;
  }
  else
  {
    error = query.lastError().text();
    return false;
  }
}

bool NoteSQL::remove(QSqlDatabase db,
                     int id,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  db.transaction();
  QSqlQuery query(db);
  query.prepare("DELETE FROM _PROMISSORYNOTES "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);
  query.exec();
  query.prepare("DELETE FROM _PROMISSORYNOTESITEMS "
                "WHERE _NOTEID = (:_id)");
  query.bindValue(":_id", id);
  query.exec();

  if (db.commit())
    return true;

  error = query.lastError().text();
  return false;
}

bool BaitaSQL::isOpen(QSqlDatabase db,
                      QString& error)
{
  error.clear();
  if (!db.isOpen())
     error = "Arquivo de configuração não foi aberto.";
  return db.isOpen();
}

bool BaitaSQL::open(QSqlDatabase db,
                    const QString& path,
                    QString& error)
{
  error.clear();
  if (db.isOpen())
    db.close();
  db.setDatabaseName(path);
  bool bSuccess = db.open();
  if (!bSuccess)
    error = db.lastError().text();
  return bSuccess;
}

void BaitaSQL::close(QSqlDatabase db)
{
  db.close();
}

bool BaitaSQL::init(QSqlDatabase db,
                    QString& error)
{
  error.clear();

  if (!isOpen(db, error))
    return false;

  db.transaction();
  QSqlQuery query(db);

  query.exec("CREATE TABLE IF NOT EXISTS _PROMISSORYNOTES ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_NUMBER INTEGER NOT NULL,"
             "_DATE INTEGER NOT NULL,"
             "_SUPPLIER TEXT NOT NULL,"
             "_TOTAL REAL,"
             "_CASH INT)");

  query.exec("CREATE TABLE IF NOT EXISTS _PROMISSORYNOTESITEMS ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_NOTEID INTEGER NOT NULL,"
             "_AMMOUNT REAL,"
             "_PRICE REAL,"
             "_UNITY TEXT,"
             "_DESCRIPTION TEXT)");

  query.exec("CREATE TABLE IF NOT EXISTS _ITEMS ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_NAME TEXT NOT NULL UNIQUE,"
             "_CATEGORYID INTEGER NOT NULL,"
             "_UNITY TEXT NOT NULL,"
             "_PACKAGE_UNITY TEXT,"
             "_PACKAGE_AMMOUNT REAL,"
             "_DETAILS TEXT,"
             "_CODE TEXT,"
             "_ICON INT,"
             "_AVAILABLE_AT_NOTES,"
             "_AVAILABLE_AT_SHOP,"
             "AVAILABLE_AT_CONSUMPTION)");

  query.exec("CREATE TABLE IF NOT EXISTS _CATEGORIES ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_NAME TEXT NOT NULL UNIQUE,"
             "_ICON INT)");

  query.exec("CREATE TABLE IF NOT EXISTS _REMINDERS ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_TITLE TEXT,"
             "_MESSAGE TEXT,"
             "_FAVORITE INT,"
             "_CAPITALIZATION INT,"
             "_SIZE INT)");

  query.exec("CREATE TABLE IF NOT EXISTS _CONSUMPTION ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_DATE INTEGER,"
             "_ITEMID INTEGER,"
             "_PRICE REAL,"
             "_AMMOUNT REAL,"
             "_TOTAL REAL)");

  query.exec("CREATE TABLE IF NOT EXISTS _USERS ("
             "_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
             "_USER TEXT NOT NULL UNIQUE,"
             "_PASSWORD TEXT NOT NULL,"
             "_ACCESS_NOTE INT,"
             "_ACCESS_REMINDER INT,"
             "_ACCESS_CALCULATOR INT,"
             "_ACCESS_SHOP INT,"
             "_ACCESS_CONSUMPTION INT,"
             "_ACCESS_USER INT,"
             "_ACCESS_ITEM INT,"
             "_ACCESS_SETTINGS INT)");

  query.exec("SELECT * FROM _USERS LIMIT 1");
  if (!query.next())
  {
    query.prepare("INSERT INTO _USERS ("
                  "_USER,"
                  "_PASSWORD,"
                  "_ACCESS_NOTE,"
                  "_ACCESS_REMINDER,"
                  "_ACCESS_CALCULATOR,"
                  "_ACCESS_SHOP,"
                  "_ACCESS_CONSUMPTION,"
                  "_ACCESS_USER,"
                  "_ACCESS_ITEM,"
                  "_ACCESS_SETTINGS) "
                  "VALUES ("
                  "'ADMIN',"
                  "(:_password),"
                  "1,1,1,1,1,1,1,1)");
    query.bindValue(":_password", User::st_strEncryptedPassword("admin"));
    query.exec();
  }

  bool bSuccess = db.commit();
  if (!bSuccess)
    error = db.lastError().text();

  return bSuccess;
}

bool ItemSQL::select(QSqlDatabase db,
                     Item& item,
                     QString& error)
{
  error.clear();
  int id = item.m_id;
  item.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  bool bFound = false;
  QSqlQuery query(db);
  query.prepare("SELECT "
                "_NAME,"
                "_CATEGORYID,"
                "_UNITY,"
                "_PACKAGE_UNITY,"
                "_PACKAGE_AMMOUNT,"
                "_DETAILS,"
                "_CODE,"
                "_ICON,"
                "_AVAILABLE_AT_NOTES,"
                "_AVAILABLE_AT_SHOP,"
                "_AVAILABLE_AT_CONSUMPTION,"
                "FROM _ITEMS "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);
  if (query.next())
  {
    item.m_id = id;
    item.m_name = query.value(query.record().indexOf("_NAME")).toString();
    item.m_categoryId = query.value(query.record().indexOf("_CATEGORYID")).toInt();
    item.m_unity = query.value(query.record().indexOf("_UNITY")).toString();
    item.m_packageUnity = query.value(query.record().indexOf("_PACKAGE_UNITY")).toString();
    item.m_packageAmmount = query.value(query.record().indexOf("_PACKAGE_AMMOUNT")).toDouble();
    item.m_details = query.value(query.record().indexOf("_DETAILS")).toString();
    item.m_code = query.value(query.record().indexOf("_CODE")).toString();
    item.m_icon = query.value(query.record().indexOf("_ICON")).toInt();
    item.m_bAvailableAtNotes = query.value(query.record().indexOf("_AVAILABLE_AT_NOTES")).toBool();
    item.m_bAvailableAtShop = query.value(query.record().indexOf("_AVAILABLE_AT_SHOP")).toBool();
    item.m_bAvailableAtConsumption = query.value(query.record().indexOf("_AVAILABLE_AT_CONSUMPTION")).toBool();
    bFound = true;
  }

  if (query.exec())
  {
    if (!bFound)
      error = "Item não encontrado.";
    return bFound;
  }
  else
  {
    error = query.lastError().text();
    return false;
  }
}

bool ItemSQL::insert(QSqlDatabase db,
                     const Item& item,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("INSERT INTO _ITEMS ("
                "_NAME,"
                "_CATEGORYID,"
                "_UNITY,"
                "_PACKAGE_UNITY,"
                "_PACKAGE_AMMOUNT,"
                "_DETAILS,"
                "_CODE,"
                "_ICON,"
                "_AVAILABLE_AT_NOTES,"
                "_AVAILABLE_AT_SHOP,"
                "_AVAILABLE_AT_CONSUMPTION) "
                "VALUES ("
                "(:_name),"
                "(:_categoryid),"
                "(:_unity),"
                "(:_packageunity),"
                "(:_packageammount),"
                "(:_details),"
                "(:_code),"
                "(:_icon),"
                "(:_availableatnotes),"
                "(:_availableatshop),"
                "(:_availableatconsumption))");
  query.bindValue(":_name", item.m_name);
  query.bindValue(":_categoryid", item.m_categoryId);
  query.bindValue(":_unity", item.m_unity);
  query.bindValue(":_packageunity", item.m_packageUnity);
  query.bindValue(":_packageammount", item.m_packageAmmount);
  query.bindValue(":_details", item.m_details);
  query.bindValue(":_code", item.m_code);
  query.bindValue(":_icon", item.m_icon);
  query.bindValue(":_availableatnotes", item.m_bAvailableAtNotes);
  query.bindValue(":_availableatshop", item.m_bAvailableAtShop);
  query.bindValue(":_availableatconsumption", item.m_bAvailableAtConsumption);

  if (query.exec())
  {
    item.m_id = query.lastInsertId().toInt();
    return true;
  }

  error = query.lastError().text();
  return false;
}

bool ItemSQL::update(QSqlDatabase db,
                     const Item& item,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("UPDATE _ITEMS SET "
                "_NAME = (:_name),"
                "_CATEGORYID = (:_categoryid),"
                "_UNITY = (:_unity),"
                "_PACKAGE_UNITY = (:_packageunity),"
                "_PACKAGE_AMMOUNT = (:_packageammount),"
                "_DETAILS = (:_details),"
                "_CODE = (:_code),"
                "_ICON = (:_icon),"
                "_AVAILABLE_AT_NOTES = (:_availableatnotes),"
                "_AVAILABLE_AT_SHOP = (:_availableatshop),"
                "_AVAILABLE_AT_CONSUMPTION = (:_availableatconsumption) "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", item.m_id);
  query.bindValue(":_name", item.m_name);
  query.bindValue(":_categoryid", item.m_categoryId);
  query.bindValue(":_unity", item.m_unity);
  query.bindValue(":_packageunity", item.m_packageUnity);
  query.bindValue(":_packageammount", item.m_packageAmmount);
  query.bindValue(":_details", item.m_details);
  query.bindValue(":_code", item.m_code);
  query.bindValue(":_icon", item.m_icon);
  query.bindValue(":_availableatnotes", item.m_bAvailableAtNotes);
  query.bindValue(":_availableatshop", item.m_bAvailableAtShop);
  query.bindValue(":_availableatconsumption", item.m_bAvailableAtConsumption);

  if (query.exec())
    return true;

  error = query.lastError().text();
  return false;
}

bool ItemSQL::remove(QSqlDatabase db,
                         int id,
                         QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("DELETE FROM _ITEMS "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);

  if (query.exec())
    return true;

  error = query.lastError().text();
  return false;
}

bool CategorySQL::select(QSqlDatabase db,
                         Category& category,
                         QString& error)
{
  error.clear();
  int id = category.m_id;
  category.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  bool bFound = false;
  QSqlQuery query(db);
  query.prepare("SELECT "
                "_NAME,"
                "_ICON "
                "FROM _CATEGORIES "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);
  if (query.next())
  {
    category.m_id = id;
    category.m_name = query.value(query.record().indexOf("_NAME")).toString();
    category.m_icon = query.value(query.record().indexOf("_ICON")).toInt();
    bFound = true;
  }

  if (query.exec())
  {
    if (!bFound)
      error = "Categoria não encontrada.";
    return bFound;
  }
  else
  {
    error = query.lastError().text();
    return false;
  }
}

bool CategorySQL::insert(QSqlDatabase db,
                         const Category& category,
                         QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("INSERT INTO _CATEGORIES ("
                "_NAME,"
                "_ICON) "
                "VALUES ("
                "(:_name),"
                "(:_icon))");
  query.bindValue(":_name", category.m_name);
  query.bindValue(":_icon", category.m_icon);

  if (query.exec())
  {
    category.m_id = query.lastInsertId().toInt();
    return true;
  }

  error = query.lastError().text();
  return false;
}

bool CategorySQL::update(QSqlDatabase db,
                         const Category& category,
                         QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("UPDATE _CATEGORIES SET "
                "_NAME = (:_name),"
                "_ICON = (:_icon) "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", category.m_id);
  query.bindValue(":_name", category.m_name);
  query.bindValue(":_icon", category.m_icon);

  if (query.exec())
    return true;

  error = query.lastError().text();
  return false;
}

bool CategorySQL::remove(QSqlDatabase db,
                         int id,
                         QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("DELETE FROM _CATEGORIES "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);

  if (query.exec())
    return true;

  error = query.lastError().text();
  return false;
}

bool ReminderSQL::select(QSqlDatabase db,
                         Reminder& r,
                         QString error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("SELECT "
                "_TITLE,"
                "_MESSAGE,"
                "_FAVORITE,"
                "_CAPITALIZATION,"
                "_SIZE "
                "FROM _REMINDERS "
                "WHERE _ID = (:_id)");

  query.bindValue(":_id", r.m_id);

  bool bSuccess = query.exec();
  if (bSuccess)
  {
    bSuccess = query.next();
    if (bSuccess)
    {
      r.m_title = query.value(query.record().indexOf("_TITLE")).toString();
      r.m_message = query.value(query.record().indexOf("_MESSAGE")).toString();
      r.m_bFavorite = query.value(query.record().indexOf("_FAVORITE")).toBool();
      r.m_capitalization = (Reminder::Capitalization)
                           query.value(query.record().indexOf("_CAPITALIZATION")).toInt();
      r.m_size = (Reminder::Size)
                 query.value(query.record().indexOf("_SIZE")).toInt();
    }
    else
    {
      r.clear();
      r.m_title = "Lembrete não encontrado.";
      error = "Lembrete não encontrado.";
    }
  }
  else
  {
    error = query.lastError().text();
  }

  return bSuccess;
}

bool ReminderSQL::insertOrUpdate(QSqlDatabase db,
                                 const Reminder& r,
                                 QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  bool bSuccess = false;
  QSqlQuery query(db);
  if (r.isValidID())
  {
    bSuccess = query.prepare("UPDATE _REMINDERS SET "
                             "_TITLE = :_title,"
                             "_MESSAGE = :_msg,"
                             "_FAVORITE = :_fav,"
                             "_CAPITALIZATION = :_cap,"
                             "_SIZE = :_size"
                             " WHERE _ID = (:_id)");
    if (bSuccess)
      query.bindValue(":_id", r.m_id);
  }
  else
  {
    bSuccess = query.prepare("INSERT INTO _REMINDERS ("
                             "_TITLE,"
                             "_MESSAGE,"
                             "_FAVORITE,"
                             "_CAPITALIZATION,"
                             "_SIZE) "
                             "VALUES ("
                             "(:_title),"
                             "(:_msg),"
                             "(:_fav),"
                             "(:_cap),"
                             "(:_size))");
  }

  if (bSuccess)
  {
    query.bindValue(":_id", r.m_id);
    query.bindValue(":_title", r.m_title);
    query.bindValue(":_msg", r.m_message);
    query.bindValue(":_fav", r.m_bFavorite);
    query.bindValue(":_cap", (int)r.m_capitalization);
    query.bindValue(":_size", (int)r.m_size);
    bSuccess = query.exec();
  }

  if (!bSuccess)
    error = query.lastError().text();

  return bSuccess;
}

void ReminderSQL::setFavorite(QSqlDatabase db,
                              int id,
                              bool bFav)
{
    QSqlQuery query(db);
    if (query.prepare("UPDATE _REMINDERS SET "
                      "_FAVORITE = :_fav"
                      " WHERE _ID = (:_id)"))
    {
      query.bindValue(":_id", id);
      query.bindValue(":_fav", bFav);
    }
    query.exec();
}

bool ReminderSQL::isFavorite(QSqlDatabase db,
                             int id)
{
  QSqlQuery query(db);
  if (query.prepare("SELECT "
                    "_FAVORITE "
                    "FROM _REMINDERS "
                    "WHERE _ID = (:_id)"))
  {
    query.bindValue(":_id", id);
    if (query.exec())
      if (query.next())
        return query.value(0).toBool();
  }
  return false;
}

bool ConsumptionSQL::selectTotal(QSqlDatabase db,
                                 const Consumption::Filter& filter,
                                 double& total,
                                 QString& error)
{
  error.clear();
  total = 0.0;

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QString str("SELECT SUM(_TOTAL) FROM _CONSUMPTION");
  if (filter.m_bDate)
  {
    str += " WHERE _DATE BETWEEN " +
           QString::number(filter.m_datei) +
           " AND " +
           QString::number(filter.m_datef);
  }

  QSqlQuery query(db);
  bool bSuccess = query.exec(str);
  if (bSuccess)
  {
    bSuccess = query.next();
    if (bSuccess)
      total = query.value(0).toDouble();
    else
      error = "Valor não encontrado.";
  }
  else
  {
    error = query.lastError().text();
  }

  return bSuccess;
}

bool ConsumptionSQL::selectSubTotal(QSqlDatabase db,
                                    const Consumption::Filter& filter,
                                    QVector<qint64>& dates,
                                    QVector<double>& totals,
                                    QString &error)
{
  dates.clear();
  totals.clear();
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QString str("SELECT SUM(_TOTAL), _DATE FROM _CONSUMPTION");
  if (filter.m_bDate)
  {
    str += " WHERE _DATE BETWEEN " +
           QString::number(filter.m_datei) +
           " AND " +
           QString::number(filter.m_datef);
  }
  str += " GROUP BY _DATE";

  QSqlQuery query(db);
  bool bSuccess = query.exec(str);
  if (bSuccess)
  {
    bSuccess = false;
    while (query.next())
    {
      totals.push_back(query.value(0).toDouble());
      dates.push_back(query.value(1).toLongLong());
      bSuccess = true;
    }
    if (!bSuccess)
      error = "Valor não encontrado.";
  }
  else
  {
    error = query.lastError().text();
  }

  return bSuccess;
}

bool ConsumptionSQL::selectByDate(QSqlDatabase db,
                                  qint64 date,
                                  QVector<Consumption>& consumptions,
                                  QString& error)
{
  consumptions.clear();
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  bool bSuccess = query.exec("SELECT "
                             "_ID,"
                             "_DATE,"
                             "_ITEMID,"
                             "_PRICE,"
                             "_AMMOUNT,"
                             "_TOTAL "
                             "FROM _CONSUMPTION "
                             "WHERE _DATE = " + QString::number(date));

  if (bSuccess)
  {
    while (query.next())
    {
      Consumption c;
      c.m_id = query.value(0).toInt();
      c.m_date = query.value(1).toLongLong();
      c.m_itemID = query.value(2).toInt();
      c.m_price = query.value(3).toDouble();
      c.m_ammount = query.value(4).toDouble();
      c.m_total = query.value(5).toDouble();
      consumptions.push_back(c);
    }
  }
  else
  {
    error = query.lastError().text();
  }

  return bSuccess;
}

void ConsumptionSQL::getConsumption(QSqlDatabase db,
                                    qint64 date,
                                    QVector<Consumption>& vConsumption,
                                    QVector<Item>& vItem)
{
  vConsumption.clear();
  vItem.clear();

  QString error;
  selectByDate(db, date, vConsumption, error);

  for (int i = 0; i != vConsumption.size(); ++i)
  {
    Item item;
    item.m_id = vConsumption.at(i).m_itemID;
    ItemSQL::select(db, item, error);
    vItem.push_back(item);
  }
}

double ConsumptionSQL::getTotal(QSqlDatabase db,
                                const Consumption::Filter& filter)
{
  double total = 0.0;
  QString error;
  ConsumptionSQL::selectTotal(db, filter, total, error);
  return total;
}

double ConsumptionSQL::getTotal(QSqlDatabase db,
                                qint64 date)
{
  double total = 0.0;
  QString error;
  Consumption::Filter filter;
  filter.m_bDate = true;
  filter.m_datei = date;
  filter.m_datef = date;
  ConsumptionSQL::selectTotal(db, filter, total, error);
  return total;
}

bool UserSQL::insert(QSqlDatabase db,
                     const User& user,
                     const QString& strPassword,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("INSERT INTO _USERS ("
                "_USER,"
                "_PASSWORD,"
                "_ACCESS_NOTE,"
                "_ACCESS_REMINDER,"
                "_ACCESS_CALCULATOR,"
                "_ACCESS_SHOP,"
                "_ACCESS_CONSUMPTION,"
                "_ACCESS_USER,"
                "_ACCESS_ITEM,"
                "_ACCESS_SETTINGS) "
                "VALUES ("
                "(:_user),"
                "(:_password),"
                "(:_accessNote),"
                "(:_accessReminder),"
                "(:_accessCalculator),"
                "(:_accessShop),"
                "(:_accessConsumption),"
                "(:_accessUser),"
                "(:_accessItem),"
                "(:_accessSettings))");
  query.bindValue(":_user", user.m_strUser);
  query.bindValue(":_password", user.st_strEncryptedPassword(strPassword));
  query.bindValue(":_accessNote", user.m_bAccessNote);
  query.bindValue(":_accessReminder", user.m_bAccessReminder);
  query.bindValue(":_accessCalculator", user.m_bAccessCalculator);
  query.bindValue(":_accessShop", user.m_bAccessShop);
  query.bindValue(":_accessConsumption", user.m_bAccessConsumption);
  query.bindValue(":_accessUser", user.m_bAccessUser);
  query.bindValue(":_accessItem", user.m_bAccessItem);
  query.bindValue(":_accessSettings", user.m_bAccessSettings);

  if (query.exec())
  {
    user.m_id = query.lastInsertId().toInt();
    return true;
  }

  error = query.lastError().text();
  return false;
}

bool UserSQL::update(QSqlDatabase db,
                     const User& user,
                     const QString& strPassword,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QString strQuery("UPDATE _USERS SET "
                   "_USER = (:_user),");
  if (!strPassword.isEmpty())
    strQuery += "_PASSWORD = (:_password),";
  strQuery += "_ACCESS_NOTE = (:_accessNote),"
              "_ACCESS_REMINDER = (:_accessReminder),"
              "_ACCESS_CALCULATOR = (:_accessCalculator),"
              "_ACCESS_SHOP = (:_accessShop),"
              "_ACCESS_CONSUMPTION = (:_accessConsumption),"
              "_ACCESS_USER = (:_accessUser),"
              "_ACCESS_ITEM = (:_accessItem),"
              "_ACCESS_SETTINGS = (:_accessSettings) "
              "WHERE _ID = (:_id)";


  QSqlQuery query(db);
  query.prepare(strQuery);
  query.bindValue(":_id", user.m_id);
  query.bindValue(":_user", user.m_strUser);
  if (!strPassword.isEmpty())
    query.bindValue(":_password", User::st_strEncryptedPassword(strPassword));
  query.bindValue(":_accessNote", user.m_bAccessNote);
  query.bindValue(":_accessReminder", user.m_bAccessReminder);
  query.bindValue(":_accessCalculator", user.m_bAccessCalculator);
  query.bindValue(":_accessShop", user.m_bAccessShop);
  query.bindValue(":_accessConsumption", user.m_bAccessConsumption);
  query.bindValue(":_accessUser", user.m_bAccessUser);
  query.bindValue(":_accessItem", user.m_bAccessItem);
  query.bindValue(":_accessSettings", user.m_bAccessSettings);

  if (query.exec())
    return true;

  error = query.lastError().text();
  return false;
}

bool UserSQL::select(QSqlDatabase db,
                     User& user,
                     QString& error)
{
  error.clear();
  int id = user.m_id;
  user.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  bool bFound = false;
  QSqlQuery query(db);
  query.prepare("SELECT "
                "_USER,"
                "_ACCESS_NOTE,"
                "_ACCESS_REMINDER,"
                "_ACCESS_CALCULATOR,"
                "_ACCESS_SHOP,"
                "_ACCESS_CONSUMPTION,"
                "_ACCESS_USER,"
                "_ACCESS_ITEM,"
                "_ACCESS_SETTINGS "
                "FROM _USERS "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);
  if (query.exec())
  {
    if (query.next())
    {
      user.m_id = id;
      user.m_strUser = query.value(query.record().indexOf("_USER")).toString();
      user.m_bAccessNote = query.value(query.record().indexOf("_ACCESS_NOTE")).toBool();
      user.m_bAccessReminder = query.value(query.record().indexOf("_ACCESS_REMINDER")).toBool();
      user.m_bAccessCalculator = query.value(query.record().indexOf("_ACCESS_CALCULATOR")).toBool();
      user.m_bAccessShop = query.value(query.record().indexOf("_ACCESS_SHOP")).toBool();
      user.m_bAccessConsumption = query.value(query.record().indexOf("_ACCESS_CONSUMPTION")).toBool();
      user.m_bAccessUser = query.value(query.record().indexOf("_ACCESS_USER")).toBool();
      user.m_bAccessItem = query.value(query.record().indexOf("_ACCESS_ITEM")).toBool();
      user.m_bAccessSettings = query.value(query.record().indexOf("_ACCESS_SETTINGS")).toBool();
      bFound = true;
    }

    if (!bFound)
      error = "Usuário não encontrado.";
    return bFound;
  }

  error = query.lastError().text();
  return false;
}

bool UserSQL::remove(QSqlDatabase db,
                     int id,
                     QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(db, error))
    return false;

  QSqlQuery query(db);
  query.prepare("DELETE FROM _USERS "
                "WHERE _ID = (:_id)");
  query.bindValue(":_id", id);
  if (query.exec())
    return true;

  error = query.lastError().text();
  return false;
}

UserLoginSQL::UserLoginSQL()
{

}

void UserLoginSQL::setDatabase(QSqlDatabase db)
{
  m_db = db;
}

QSqlDatabase UserLoginSQL::getDatabase() const
{
  return m_db;
}

bool UserLoginSQL::login(const QString& strUser,
                         const QString& strPassword,
                         QString& error)
{
  error.clear();

  if (!BaitaSQL::isOpen(m_db, error))
    return false;

  QSqlQuery query(m_db);
  query.prepare("SELECT * FROM _USERS "
                "WHERE _USER = (:_user) AND "
                "_PASSWORD = (:_password) LIMIT 1");
  query.bindValue(":_user", strUser);
  query.bindValue(":_password", User::st_strEncryptedPassword(strPassword));

  if (query.exec())
  {
    bool bFound = false;
    if (query.next())
    {
      m_user.m_id = query.value(query.record().indexOf("_ID")).toInt();
      m_user.m_strUser = query.value(query.record().indexOf("_USER")).toString();
      m_user.m_bAccessNote = query.value(query.record().indexOf("_ACCESS_NOTE")).toBool();
      m_user.m_bAccessReminder = query.value(query.record().indexOf("_ACCESS_REMINDER")).toBool();
      m_user.m_bAccessCalculator = query.value(query.record().indexOf("_ACCESS_CALCULATOR")).toBool();
      m_user.m_bAccessShop = query.value(query.record().indexOf("_ACCESS_SHOP")).toBool();
      m_user.m_bAccessConsumption = query.value(query.record().indexOf("_ACCESS_CONSUMPTION")).toBool();
      m_user.m_bAccessUser = query.value(query.record().indexOf("_ACCESS_USER")).toBool();
      m_user.m_bAccessItem = query.value(query.record().indexOf("_ACCESS_ITEM")).toBool();
      m_user.m_bAccessSettings = query.value(query.record().indexOf("_ACCESS_SETTINGS")).toBool();
      bFound = true;
    }

    if (!bFound)
      error = "Usuário ou senha inválidos.";
    return bFound;
  }

  error = query.lastError().text();
  return false;
}
