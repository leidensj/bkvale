#include "activeuser.h"
#include "jmodel.h"
#include <QHostInfo>

class ActiveUserModel : public JModel
{
public:
  ActiveUserModel(QObject *parent)
    : JModel(parent)
  {

  }

  QString getStrQuery()
  {
    return
        "SELECT "
        ACTIVE_USERS_SQL_TABLE_NAME "." SQL_COLID ","
        ACTIVE_USERS_SQL_TABLE_NAME "." ACTIVE_USERS_SQL_COL01 ","
        USER_SQL_TABLE_NAME "." USER_SQL_COL01 ","
        ACTIVE_USERS_SQL_TABLE_NAME "." ACTIVE_USERS_SQL_COL03 ","
        ACTIVE_USERS_SQL_TABLE_NAME "." ACTIVE_USERS_SQL_COL04
        " FROM "
        ACTIVE_USERS_SQL_TABLE_NAME " LEFT JOIN "
        USER_SQL_TABLE_NAME " ON "
        ACTIVE_USERS_SQL_TABLE_NAME "." ACTIVE_USERS_SQL_COL02 " = "
        USER_SQL_TABLE_NAME "." SQL_COLID;
  }

  void select(QHeaderView* header)
  {
    JModel::select(getStrQuery());
    setHeaderData(0, Qt::Horizontal, tr("ID"));
    setHeaderData(1, Qt::Horizontal, tr("PID"));
    setHeaderData(2, Qt::Horizontal, tr("Usuário"));
    setHeaderData(3, Qt::Horizontal, tr("Máquina"));
    setHeaderData(4, Qt::Horizontal, tr("Login"));
    if (header != nullptr && header->count() == 5)
    {
      header->hideSection(0);
      header->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
      header->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
      header->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);
      header->setSectionResizeMode(4, QHeaderView::ResizeMode::ResizeToContents);
    }
  }
};

ActiveUser::ActiveUser()
{
  clear();
}

ActiveUser::~ActiveUser()
{
  if (m_id.isValid())
  {
    QString error;
    SQL_remove(error);
  }
}

void ActiveUser::clear()
{
  m_user.clear();
}

bool ActiveUser::operator != (const JItem& other) const
{
  const ActiveUser& another = dynamic_cast<const ActiveUser&>(other);
  return m_user.m_id != another.m_user.m_id;
}

bool ActiveUser::operator == (const JItem& other) const
{
  return !(*this != other);
}

bool ActiveUser::isValid() const
{
  return m_user.m_id.isValid();
}

QString ActiveUser::SQL_tableName() const
{
  return ACTIVE_USERS_SQL_TABLE_NAME;
}

bool ActiveUser::SQL_insert_proc(QSqlQuery& query) const
{
  query.prepare("INSERT INTO " ACTIVE_USERS_SQL_TABLE_NAME " ("
                ACTIVE_USERS_SQL_COL01 ","
                ACTIVE_USERS_SQL_COL02 ","
                ACTIVE_USERS_SQL_COL03 ","
                ACTIVE_USERS_SQL_COL04 ")"
                " VALUES ("
                "(SELECT pg_backend_pid()),"
                "(:_v01),"
                "(:_v02),"
                "current_timestamp)");
  query.bindValue(":_v01", m_user.m_id.get());
  query.bindValue(":_v02", QHostInfo::localHostName().toUpper());
  bool bSuccess = query.exec();
  if (bSuccess)
    m_id.set(query.lastInsertId().toLongLong());
  return bSuccess;
}

bool ActiveUser::SQL_update_proc(QSqlQuery& query) const
{
  query.prepare("UPDATE " ACTIVE_USERS_SQL_TABLE_NAME " SET "
                ACTIVE_USERS_SQL_COL01 " = (SELECT pg_backend_pid()),"
                ACTIVE_USERS_SQL_COL02 " = (:_v01),"
                ACTIVE_USERS_SQL_COL03 " = (:_v02),"
                ACTIVE_USERS_SQL_COL04 " = current_timestamp"
                " WHERE " SQL_COLID " = (:_v00)");
  query.bindValue(":_v00", m_id.get());
  query.bindValue(":_v01", m_user.m_id.get());
  query.bindValue(":_v02", QHostInfo::localHostName().toUpper());
  return query.exec();
}

bool ActiveUser::SQL_select_proc(QSqlQuery& query, QString& error)
{
  error.clear();
  query.prepare("SELECT "
                ACTIVE_USERS_SQL_COL01 ","
                ACTIVE_USERS_SQL_COL02 ","
                ACTIVE_USERS_SQL_COL03 ","
                ACTIVE_USERS_SQL_COL04
                " FROM " ACTIVE_USERS_SQL_TABLE_NAME
                " WHERE " SQL_COLID " = (:_v00)");
  query.bindValue(":_v00", m_id.get());
  bool bSuccess = query.exec();
  if (bSuccess)
  {
    if (query.next())
    {
      m_pid = query.value(0).toLongLong();
      m_user.m_id.set(query.value(1).toLongLong());
      m_machine = query.value(2).toString();
      m_time = query.value(3).toTime();
    }
    else
    {
      error = "Login não encontrado.";
      bSuccess = false;
    }
  }

  if (bSuccess)
    m_user.SQL_select_proc(query, error);

  return bSuccess;
}

bool ActiveUser::SQL_remove_proc(QSqlQuery& query) const
{
  query.prepare("DELETE FROM " ACTIVE_USERS_SQL_TABLE_NAME " WHERE " SQL_COLID " = (:_v00)");
  query.bindValue(":_v00", m_id.get());
  return query.exec();
}

JModel* ActiveUser::SQL_table_model(QObject* parent) const
{
  return new ActiveUserModel(parent);
}

bool ActiveUser::SQL_login(const QString& strUser, const QString& strPassword, QString& error)
{
  error.clear();

  if (!SQL_isOpen(error))
    return false;

  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  db.transaction();
  QSqlQuery query(db);

  m_user.clear();
  m_user.m_strUser = strUser;
  m_user.m_password = strPassword;
  bool bSuccess = m_user.SQL_select_password_proc(query, error);

  if (bSuccess)
  {
    bSuccess = SQL_logout_proc(query);
    if (bSuccess)
    {
      query.prepare("SELECT " ACTIVE_USERS_SQL_COL03
                    " FROM "
                    ACTIVE_USERS_SQL_TABLE_NAME " WHERE "
                    ACTIVE_USERS_SQL_COL02 " = (:v02) LIMIT 1");
      query.bindValue(":v02", m_user.m_id.get());
      if (query.next())
      {
        bSuccess = false;
        error = "Usuário " + m_user.m_strUser  + " já logado na máquina " + query.value(0).toString();
      }
      else
      {
        bSuccess = SQL_insert_proc(query);
      }
    }
  }

  return SQL_finish(db, query, bSuccess, error);
}

bool ActiveUser::SQL_logout(QString& error)
{
  error.clear();

  if (!SQL_isOpen(error))
    return false;

  QSqlDatabase db(QSqlDatabase::database(POSTGRE_CONNECTION_NAME));
  db.transaction();
  QSqlQuery query(db);

  bool bSuccess = SQL_logout_proc(query);
  return SQL_finish(db, query, bSuccess, error);
}

bool ActiveUser::SQL_logout_proc(QSqlQuery& query)
{
  query.prepare("DELETE FROM " ACTIVE_USERS_SQL_TABLE_NAME
                " WHERE " ACTIVE_USERS_SQL_COL01 " = pg_backend_pid()");
  bool bSuccess = query.exec();
  if (bSuccess)
  {
    query.prepare("DELETE FROM " ACTIVE_USERS_SQL_TABLE_NAME
                  " WHERE " ACTIVE_USERS_SQL_COL01 " NOT IN "
                  "(SELECT pid FROM pg_stat_activity)");
    bSuccess = query.exec();
  }
  return bSuccess;
}
