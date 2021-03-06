#include "store.h"

Store::Store(Id id)
{
  clear();
  m_id = id;
}

void Store::clear(bool bClearId)
{
  if (bClearId)
    m_id.clear();
  m_form.clear(bClearId);
  m_form.m_bCompany = true;
  m_description.clear();
  m_vEmployee.clear();
}

bool Store:: operator != (const JItem& other) const
{
  const Store& another = dynamic_cast<const Store&>(other);
  return m_form.m_id != another.m_form.m_id ||
         m_description != another.m_description ||
         m_vEmployee != another.m_vEmployee;
}

bool Store:: operator == (const JItem& other) const
{
  return !(*this != other);
}

bool Store::isValid() const
{
  return m_form.m_id.isValid();
}

QString Store::SQL_tableName() const
{
  return STORE_SQL_TABLE_NAME;
}

bool Store::SQL_insert_proc(QSqlQuery& query) const
{
  bool bSuccess = m_form.SQL_insert_proc(query);
  if (bSuccess)
  {
    query.prepare("INSERT INTO " STORE_SQL_TABLE_NAME " ("
                  STORE_SQL_COL_FID ","
                  STORE_SQL_COL_DES ")"
                  " VALUES ("
                  "(:_v01),"
                  "(:_v02))");

    query.bindValue(":_v01", m_form.m_id.getIdNull());
    query.bindValue(":_v02", m_description);

    bSuccess = query.exec();
    if (bSuccess)
    {
      m_id.set(query.lastInsertId().toLongLong());
      for (int i = 0; i != m_vEmployee.size() && bSuccess; ++i)
      {
        query.prepare("INSERT INTO " STORE_EMPLOYEES_SQL_TABLE_NAME " ("
                      STORE_EMPLOYEES_SQL_COL_EID ","
                      STORE_EMPLOYEES_SQL_COL_SID ")"
                      " VALUES ("
                      "(:_v01),"
                      "(:_v02))");
        query.bindValue(":_v01", m_vEmployee.at(i).m_id.get());
        query.bindValue(":_v02", m_id.get());
        bSuccess = query.exec();
      }
    }
  }
  return bSuccess;
}

bool Store::SQL_update_proc(QSqlQuery& query) const
{
  bool bSuccess = m_form.SQL_update_proc(query);
  if (bSuccess)
  {
    query.prepare("UPDATE " STORE_SQL_TABLE_NAME " SET "
                  STORE_SQL_COL_FID " = (:_v01),"
                  STORE_SQL_COL_DES " = (:_v02)"
                  " WHERE " SQL_COLID " = (:_v00)");

    query.bindValue(":_v00", m_id.get());
    query.bindValue(":_v01", m_form.m_id.getIdNull());
    query.bindValue(":_v02", m_description);
    bSuccess = query.exec();
    if (bSuccess)
    {
      query.prepare("DELETE FROM " STORE_EMPLOYEES_SQL_TABLE_NAME
                    " WHERE " STORE_EMPLOYEES_SQL_COL_SID " = (:_v02)");
      query.bindValue(":_v02", m_id.get());
      bSuccess = query.exec();
      for (int i = 0; i != m_vEmployee.size() && bSuccess; ++i)
      {
        query.prepare("INSERT INTO " STORE_EMPLOYEES_SQL_TABLE_NAME " ("
                      STORE_EMPLOYEES_SQL_COL_EID ","
                      STORE_EMPLOYEES_SQL_COL_SID ")"
                      " VALUES ("
                      "(:_v01),"
                      "(:_v02))");
        query.bindValue(":_v01", m_vEmployee.at(i).m_id.get());
        query.bindValue(":_v02", m_id.get());
        bSuccess = query.exec();
      }
    }
  }

  return bSuccess;
}

bool Store::SQL_select_proc(QSqlQuery& query, QString& error)
{
  error.clear();

  query.prepare("SELECT "
                STORE_SQL_COL_FID ","
                STORE_SQL_COL_DES
                " FROM " STORE_SQL_TABLE_NAME
                " WHERE " SQL_COLID " = (:_v00)");
  query.bindValue(":_v00", m_id.get());

  bool bSuccess = query.exec();
  if (bSuccess)
  {
    if (query.next())
    {
      m_form.m_id = query.value(0).toLongLong();
      m_description = query.value(1).toString();
    }
    else
    {
      error = "Loja não encontrada.";
      bSuccess = false;
    }
  }

  if (bSuccess)
  {
    query.prepare("SELECT "
                  STORE_EMPLOYEES_SQL_COL_EID
                  " FROM " STORE_EMPLOYEES_SQL_TABLE_NAME
                  " WHERE " STORE_EMPLOYEES_SQL_COL_SID " = (:_v02)");
    query.bindValue(":_v02", m_id.get());
    bSuccess = query.exec();
    while (bSuccess && query.next())
    {
      Employee e;
      e.m_id.set(query.value(0).toLongLong());
      m_vEmployee.push_back(e);
    }
  }

  for (int i = 0; i != m_vEmployee.size() && bSuccess; ++i)
    bSuccess = m_vEmployee[i].SQL_select_proc(query, error);

  if (bSuccess && m_form.m_id.isValid())
    bSuccess = m_form.SQL_select_proc(query, error);

  return bSuccess;
}

bool Store::SQL_remove_proc(QSqlQuery& query) const
{
  bool bSuccess = SQL_select_formid_proc(query);
  if (bSuccess)
    bSuccess = m_form.SQL_remove_proc(query);
  return bSuccess;
}

bool Store::SQL_select_formid_proc(QSqlQuery& query) const
{
  query.prepare("SELECT "
                STORE_SQL_COL_FID
                " FROM " STORE_SQL_TABLE_NAME
                " WHERE " SQL_COLID " = (:_v00)");
  query.bindValue(":_v00", m_id.get());

  bool bSuccess = query.exec();
  if (bSuccess && query.next())
    m_form.m_id = query.value(0).toLongLong();
  return bSuccess;
}
