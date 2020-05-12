#include "databaseutils.h"
#include "defines.h"
#include "items/user.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QHostInfo>
#include <QDate>

#define DATABASE_NOT_OPEN_TXT "O banco de dados não foi aberto."

namespace {
  bool finishTransaction(QSqlDatabase db,
                         const QSqlQuery& query,
                         bool bExecSelectResult,
                         QString& error)
  {
    if (!bExecSelectResult)
    {
      if (error.isEmpty())
      {
        error = query.lastError().text();
        if (error.isEmpty())
          error = db.lastError().text();
      }
      db.rollback();
      return false;
    }
    else
      bExecSelectResult = db.commit();

    if (!bExecSelectResult && error.isEmpty())
    {
      error = query.lastError().text();
      if (error.isEmpty())
        error = db.lastError().text();
    }

    return bExecSelectResult;
  }
}

bool BaitaSQL::isOpen(QString& error)
{
  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  error.clear();
  if (!db.isOpen())
     error = "Banco de dados não foi aberto.";
  return db.isOpen();
}

bool BaitaSQL::open(const QString& hostName,
                    int port,
                    QString& error)
{
  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  error.clear();
  if (db.isOpen())
    db.close();
  db.setPort(port);
  db.setHostName(hostName);
  db.setDatabaseName("BaitaAssistente");
  db.setUserName("BaitaAssistente");
  db.setPassword("jfljfl");
  bool bSuccess = db.open();
  if (!bSuccess)
    error = db.lastError().text();
  return bSuccess;
}

void BaitaSQL::close()
{
  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  db.close();
}

bool BaitaSQL::init(const QString& hostName,
                    int port,
                    QString& error)
{
  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  bool bSuccess = db.isValid();
  if (bSuccess)
  {
    if (db.isOpen())
      db.close();
    bSuccess = BaitaSQL::open(hostName, port, error);
    if (bSuccess)
    {
      bSuccess = BaitaSQL::createTables(error);
      if (!bSuccess)
        db.close();
    }
  }
  else
  {
    error = "Driver not loaded.";
  }
  return bSuccess;
}

QDate BaitaSQL::getDate(bool dfltMax)
{
  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  QSqlQuery query(db);
  if (query.exec("SELECT current_date") && query.next())
    return query.value(0).toDate();
  return QDate::fromJulianDay(dfltMax ? Q_INT64_C(784354017364) : Q_INT64_C(-784350574879));
}

bool BaitaSQL::createTables(QString& error)
{
  error.clear();

  if (!isOpen(error))
    return false;

  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  db.transaction();
  QSqlQuery query(db);

  bool bSuccess = true;

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " IMAGE_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          IMAGE_SQL_COL01 " TEXT NOT NULL UNIQUE,"
                          IMAGE_SQL_COL02 " BYTEA)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " CATEGORY_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          CATEGORY_SQL_COL01 " INTEGER,"
                          CATEGORY_SQL_COL02 " TEXT NOT NULL UNIQUE,"
                          "FOREIGN KEY(" CATEGORY_SQL_COL01 ") REFERENCES "
                          IMAGE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " REMINDER_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          REMINDER_SQL_COL01 " TEXT,"
                          REMINDER_SQL_COL02 " TEXT,"
                          REMINDER_SQL_COL03 " BOOLEAN,"
                          REMINDER_SQL_COL04 " INTEGER,"
                          REMINDER_SQL_COL05 " INTEGER,"
                          REMINDER_SQL_COL06 " BOOLEAN,"
                          REMINDER_SQL_COL07 " TEXT,"
                          REMINDER_SQL_COL08 " DATE,"
                          REMINDER_SQL_COL09 " BOOLEAN,"
                          REMINDER_SQL_COL10 " TIME,"
                          REMINDER_SQL_COL11 " BOOLEAN,"
                          REMINDER_SQL_COL12 " TEXT)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " USER_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          USER_SQL_COL01 " TEXT NOT NULL UNIQUE CHECK ("
                          USER_SQL_COL01 " <> ''),"
                          USER_SQL_COL02 " TEXT NOT NULL,"
                          USER_SQL_COL03 " BOOLEAN,"
                          USER_SQL_COL04 " BOOLEAN,"
                          USER_SQL_COL05 " BOOLEAN,"
                          USER_SQL_COL06 " BOOLEAN,"
                          USER_SQL_COL07 " BOOLEAN,"
                          USER_SQL_COL08 " BOOLEAN,"
                          USER_SQL_COL09 " BOOLEAN,"
                          USER_SQL_COL10 " BOOLEAN,"
                          USER_SQL_COL11 " BOOLEAN,"
                          USER_SQL_COL12 " BOOLEAN,"
                          USER_SQL_COL13 " BOOLEAN,"
                          USER_SQL_COL14 " BOOLEAN,"
                          USER_SQL_COL15 " BOOLEAN,"
                          USER_SQL_COL16 " BOOLEAN)");

  if (bSuccess)
  bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " PRODUCT_SQL_TABLE_NAME " ("
                        SQL_COLID " SERIAL PRIMARY KEY,"
                        PRODUCT_SQL_COL01 " TEXT NOT NULL UNIQUE,"
                        PRODUCT_SQL_COL02 " INTEGER,"
                        PRODUCT_SQL_COL03 " INTEGER,"
                        PRODUCT_SQL_COL04 " TEXT NOT NULL,"
                        PRODUCT_SQL_COL05 " TEXT,"
                        PRODUCT_SQL_COL06 " BOOLEAN,"
                        PRODUCT_SQL_COL07 " BOOLEAN,"
                        "FOREIGN KEY(" PRODUCT_SQL_COL02 ") REFERENCES "
                        CATEGORY_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL,"
                        "FOREIGN KEY(" PRODUCT_SQL_COL03 ") REFERENCES "
                        IMAGE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " FORM_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          FORM_SQL_COL01 " INTEGER,"
                          FORM_SQL_COL02 " TEXT NOT NULL,"
                          FORM_SQL_COL03 " TEXT,"
                          FORM_SQL_COL04 " TEXT,"
                          FORM_SQL_COL05 " TEXT,"
                          FORM_SQL_COL06 " TEXT,"
                          FORM_SQL_COL07 " TEXT,"
                          FORM_SQL_COL08 " DATE,"
                          FORM_SQL_COL09 " DATE,"
                          FORM_SQL_COL10 " BOOLEAN,"
                          FORM_SQL_COL11 " BOOLEAN,"
                          "FOREIGN KEY(" FORM_SQL_COL01 ") REFERENCES "
                          IMAGE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
  bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " ADDRESS_SQL_TABLE_NAME " ("
                        SQL_COLID " SERIAL PRIMARY KEY,"
                        ADDRESS_SQL_COL01 " INTEGER,"
                        ADDRESS_SQL_COL02 " TEXT,"
                        ADDRESS_SQL_COL03 " TEXT,"
                        ADDRESS_SQL_COL04 " TEXT,"
                        ADDRESS_SQL_COL05 " INTEGER,"
                        ADDRESS_SQL_COL06 " TEXT,"
                        ADDRESS_SQL_COL07 " INTEGER,"
                        ADDRESS_SQL_COL08 " TEXT,"
                        ADDRESS_SQL_COL09 " TEXT,"
                        "FOREIGN KEY(" ADDRESS_SQL_COL01 ") REFERENCES "
                        FORM_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " PHONE_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          PHONE_SQL_COL01 " INTEGER,"
                          PHONE_SQL_COL02 " INTEGER DEFAULT " PHONE_DEFAULT_COUNTRY_CODE_VALUE_STR ","
                          PHONE_SQL_COL03 " INTEGER DEFAULT " PHONE_DEFAULT_CODE_VALUE_STR ","
                          PHONE_SQL_COL04 " TEXT,"
                          PHONE_SQL_COL05 " TEXT,"
                          "FOREIGN KEY(" PHONE_SQL_COL01 ") REFERENCES "
                          FORM_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " STORE_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          STORE_SQL_COL01 " INTEGER,"
                          STORE_SQL_COL02 " INTEGER,"
                          STORE_SQL_COL03 " INTEGER,"
                          STORE_SQL_COL04 " TEXT,"
                          "FOREIGN KEY(" STORE_SQL_COL01 ") REFERENCES "
                          FORM_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE,"
                          "FOREIGN KEY(" STORE_SQL_COL02 ") REFERENCES "
                          ADDRESS_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL,"
                          "FOREIGN KEY(" STORE_SQL_COL03 ") REFERENCES "
                          PHONE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " EMPLOYEE_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          EMPLOYEE_SQL_COL01 " INTEGER NOT NULL UNIQUE,"
                          EMPLOYEE_SQL_COL02 " TEXT UNIQUE,"
                          EMPLOYEE_SQL_COL03 " BOOLEAN,"
                          EMPLOYEE_SQL_COL04 " BOOLEAN,"
                          "FOREIGN KEY(" EMPLOYEE_SQL_COL01 ") REFERENCES "
                          FORM_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " EMPLOYEE_HOURS_SQL_TABLE_NAME " ("
                          EMPLOYEE_HOURS_SQL_COL01 " INTEGER NOT NULL,"
                          EMPLOYEE_HOURS_SQL_COL02 " INTEGER,"
                          EMPLOYEE_HOURS_SQL_COL03 " TIME,"
                          EMPLOYEE_HOURS_SQL_COL04 " TIME,"
                          "FOREIGN KEY(" EMPLOYEE_HOURS_SQL_COL01 ") REFERENCES "
                          EMPLOYEE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " SUPPLIER_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          SUPPLIER_SQL_COL01 " INTEGER NOT NULL UNIQUE,"
                          "FOREIGN KEY(" SUPPLIER_SQL_COL01 ") REFERENCES "
                          FORM_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " STORE_EMPLOYEES_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          STORE_EMPLOYEES_SQL_COL01 " INTEGER NOT NULL UNIQUE,"
                          STORE_EMPLOYEES_SQL_COL02 " INTEGER NOT NULL,"
                          "FOREIGN KEY(" STORE_EMPLOYEES_SQL_COL01 ") REFERENCES "
                          EMPLOYEE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE,"
                          "FOREIGN KEY(" STORE_EMPLOYEES_SQL_COL02 ") REFERENCES "
                          STORE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
  bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " PURCHASE_SQL_TABLE_NAME " ("
                        SQL_COLID " SERIAL PRIMARY KEY,"
                        PURCHASE_SQL_COL_NMB " INTEGER UNIQUE NOT NULL,"
                        PURCHASE_SQL_COL_DTE " DATE NOT NULL,"
                        PURCHASE_SQL_COL_SPL " INTEGER,"
                        PURCHASE_SQL_COL_OBS " TEXT,"
                        PURCHASE_SQL_COL_DSC " REAL,"
                        PURCHASE_SQL_COL_EMP " INTEGER,"
                        PURCHASE_SQL_COL_MTH " INTEGER NOT NULL,"
                        PURCHASE_SQL_COL_STR " INTEGER NOT NULL,"
                        "FOREIGN KEY(" PURCHASE_SQL_COL_SPL ") REFERENCES "
                        SUPPLIER_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL,"
                        "FOREIGN KEY(" PURCHASE_SQL_COL_EMP ") REFERENCES "
                        EMPLOYEE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL,"
                        "FOREIGN KEY(" PURCHASE_SQL_COL_STR ") REFERENCES "
                        STORE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " PURCHASE_ELEMENTS_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          PURCHASE_ELEMENTS_SQL_COL_NID " INTEGER NOT NULL,"
                          PURCHASE_ELEMENTS_SQL_COL_PID " INTEGER,"
                          PURCHASE_ELEMENTS_SQL_COL_AMT " REAL,"
                          PURCHASE_ELEMENTS_SQL_COL_PRC " REAL,"
                          PURCHASE_ELEMENTS_SQL_COL_PCK " BOOLEAN,"
                          PURCHASE_ELEMENTS_SQL_COL_UNT " TEXT,"
                          PURCHASE_ELEMENTS_SQL_COL_PAM " REAL,"
                          "FOREIGN KEY(" PURCHASE_ELEMENTS_SQL_COL_NID ") REFERENCES "
                          PURCHASE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE,"
                          "FOREIGN KEY(" PURCHASE_ELEMENTS_SQL_COL_PID ") REFERENCES "
                          PRODUCT_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " PAYMENT_ELEMENTS_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          PAYMENT_ELEMENTS_SQL_COL_NID " INTEGER NOT NULL,"
                          PAYMENT_ELEMENTS_SQL_COL_DTE " DATE,"
                          PAYMENT_ELEMENTS_SQL_COL_VLE " REAL,"
                          "FOREIGN KEY(" PAYMENT_ELEMENTS_SQL_COL_NID ") REFERENCES "
                          PURCHASE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " SHOPPING_LIST_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          SHOPPING_LIST_SQL_COL01 " INTEGER,"
                          SHOPPING_LIST_SQL_COL02 " INTEGER,"
                          SHOPPING_LIST_SQL_COL03 " TEXT NOT NULL,"
                          SHOPPING_LIST_SQL_COL04 " TEXT,"
                          SHOPPING_LIST_SQL_COL05 " TEXT,"
                          SHOPPING_LIST_SQL_COL06 " TEXT,"
                          SHOPPING_LIST_SQL_COL07 " INTEGER,"
                          "FOREIGN KEY(" SHOPPING_LIST_SQL_COL01 ") REFERENCES "
                          SUPPLIER_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL,"
                          "FOREIGN KEY(" SHOPPING_LIST_SQL_COL02 ") REFERENCES "
                          IMAGE_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " SHOPPING_LIST_ITEMS_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          SHOPPING_LIST_ITEMS_SQL_LID " INTEGER NOT NULL,"
                          SHOPPING_LIST_ITEMS_SQL_PID " INTEGER NOT NULL,"
                          SHOPPING_LIST_ITEMS_SQL_AMT " REAL,"
                          SHOPPING_LIST_ITEMS_SQL_PRC " REAL,"
                          SHOPPING_LIST_ITEMS_SQL_PCK " BOOLEAN,"
                          SHOPPING_LIST_ITEMS_SQL_UNT " TEXT,"
                          SHOPPING_LIST_ITEMS_SQL_PAM " REAL,"
                          SHOPPING_LIST_ITEMS_SQL_SID " INTEGER,"
                          "FOREIGN KEY(" SHOPPING_LIST_ITEMS_SQL_LID ") REFERENCES "
                          SHOPPING_LIST_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE,"
                          "FOREIGN KEY(" SHOPPING_LIST_ITEMS_SQL_PID ") REFERENCES "
                          PRODUCT_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE,"
                          "FOREIGN KEY(" SHOPPING_LIST_ITEMS_SQL_SID ") REFERENCES "
                          SUPPLIER_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE SET NULL)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " PRODUCT_CODE_ITEMS_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          PRODUCT_CODE_ITEMS_SQL_COL01 " INTEGER NOT NULL,"
                          PRODUCT_CODE_ITEMS_SQL_COL02 " TEXT UNIQUE NOT NULL CHECK ("
                          PRODUCT_CODE_ITEMS_SQL_COL02 " <> ''),"
                          "FOREIGN KEY(" PRODUCT_CODE_ITEMS_SQL_COL01 ") REFERENCES "
                          PRODUCT_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE CASCADE)");

  if (bSuccess)
    bSuccess = query.exec("CREATE TABLE IF NOT EXISTS " ACTIVE_USERS_SQL_TABLE_NAME " ("
                          SQL_COLID " SERIAL PRIMARY KEY,"
                          ACTIVE_USERS_SQL_COL01 " INTEGER,"
                          ACTIVE_USERS_SQL_COL02 " INTEGER UNIQUE NOT NULL,"
                          ACTIVE_USERS_SQL_COL03 " TEXT,"
                          ACTIVE_USERS_SQL_COL04 " TIMESTAMP,"
                          "FOREIGN KEY(" ACTIVE_USERS_SQL_COL02 ") REFERENCES "
                          USER_SQL_TABLE_NAME "(" SQL_COLID ") ON DELETE NO ACTION);");

  if (bSuccess)
  {
    query.exec("SELECT * FROM " USER_SQL_TABLE_NAME " LIMIT 1");
    if (!query.next())
    {
      QString str = "INSERT INTO " USER_SQL_TABLE_NAME " ("
                    USER_SQL_COL01 ","
                    USER_SQL_COL02 ","
                    USER_SQL_COL03 ","
                    USER_SQL_COL04 ","
                    USER_SQL_COL05 ","
                    USER_SQL_COL06 ","
                    USER_SQL_COL07 ","
                    USER_SQL_COL08 ","
                    USER_SQL_COL09 ","
                    USER_SQL_COL10 ","
                    USER_SQL_COL11 ","
                    USER_SQL_COL12 ","
                    USER_SQL_COL13 ","
                    USER_SQL_COL14 ","
                    USER_SQL_COL15 ","
                    USER_SQL_COL16 ")"
                    " VALUES ('"
                    USER_SQL_DEFAULT_NAME "',"
                    ":_password,"
                    "TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE);";
      query.prepare(str);
      query.bindValue(":_password", User::st_strEncryptedPassword(USER_SQL_DEFAULT_PASSWORD));
      bSuccess = query.exec();
    }
  }

  return finishTransaction(db, query, bSuccess, error);
}
