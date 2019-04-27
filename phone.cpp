#include "phone.h"
#include "jmodel.h"

class PhoneModel : public JModel
{
public:
  PhoneModel(QObject *parent)
    : JModel(parent)
  {

  }

  QString getStrQuery()
  {
    QString strQuery("SELECT "
                     SQL_COLID ","
                     PHONE_SQL_COL02 ","
                     PHONE_SQL_COL03 ","
                     PHONE_SQL_COL04 ","
                     PHONE_SQL_COL05
                     " FROM "
                     PHONE_SQL_TABLE_NAME);
    return strQuery;
  }

  virtual void select(QHeaderView* header)
  {
    JModel::select("");
    setHeaderData(0, Qt::Horizontal, tr("ID"));
    setHeaderData(1, Qt::Horizontal, tr("País"));
    setHeaderData(2, Qt::Horizontal, tr("Código"));
    setHeaderData(3, Qt::Horizontal, tr("Número"));
    setHeaderData(4, Qt::Horizontal, tr("Nome"));
    if (header != nullptr && header->count() == 5)
    {
      header->hideSection(0);
      header->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
      header->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
      header->setSectionResizeMode(3, QHeaderView::ResizeMode::ResizeToContents);
      header->setSectionResizeMode(4, QHeaderView::ResizeMode::Stretch);
    }
  }
};

Phone::Phone()
{
  clear();
}

void Phone::clear(bool bClearId)
{
  if (bClearId)
    m_id.clear();
  m_countryCode = PHONE_DEFAULT_COUNTRY_CODE_VALUE;
  m_code = PHONE_DEFAULT_CODE_VALUE;
  m_number.clear();
  m_name.clear();
}

bool Phone::operator !=(const JItem& other) const
{
  const Phone& another = dynamic_cast<const Phone&>(other);
  return
      m_countryCode != another.m_countryCode ||
      m_code != another.m_code ||
      m_number != another.m_number ||
      m_name != another.m_name;
}

bool Phone::operator ==(const JItem& other) const
{
  return !(*this != other);
}

bool Phone::isValid() const
{
  return !m_number.isEmpty();
}

QString Phone::SQL_tableName() const
{
  return PHONE_SQL_TABLE_NAME;
}

bool Phone::SQL_insert_proc(QSqlQuery& /*query*/) const
{
  return false;
}

bool Phone::SQL_update_proc(QSqlQuery& /*query*/) const
{
  return false;
}

bool Phone::SQL_select_proc(QSqlQuery& query, QString& error)
{
  error.clear();
  query.prepare("SELECT "
                PHONE_SQL_COL01 ","
                PHONE_SQL_COL02 ","
                PHONE_SQL_COL03 ","
                PHONE_SQL_COL04 ","
                PHONE_SQL_COL05
                " FROM " PHONE_SQL_TABLE_NAME
                " WHERE " SQL_COLID " = (:_v00)");
  query.bindValue(":_v00", m_id.get());

  bool bSuccess = query.exec();
  if (bSuccess)
  {
    if (query.next())
    {
      query.value(0).toLongLong(); //personId não usamos
      m_countryCode = query.value(1).toInt();
      m_code = query.value(2).toInt();
      m_number = query.value(3).toString();
      m_name = query.value(4).toString();
    }
    else
    {
      error = "Telefone não encontrado.";
      bSuccess = false;
    }
  }

  return bSuccess;
}

bool Phone::SQL_remove_proc(QSqlQuery& /*query*/) const
{
  return false;
}

QString Phone::strFormattedPhone() const
{
  if (m_number.isEmpty())
    return "";

  return
    "+" +
    QString::number(m_countryCode) +
    " (" +
    QString::number(m_code) +
    ") " +
    m_number;
}

QString Phone::strFormattedPhoneWithName() const
{
  QString fmt = strFormattedPhone();
  if (fmt.isEmpty())
    return "";

  QString str;
  if (!m_name.isEmpty())
    str += m_name + " ";
  str += strFormattedPhone();
  return str;
}

JModel* Phone::SQL_table_model(QObject* parent) const
{
  return new PhoneModel(parent);
}
