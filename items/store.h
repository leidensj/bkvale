#ifndef STORE_H
#define STORE_H

#include "employee.h"
#include <QVector>

struct Store : public JItemSQL
{
  Store(Id id = Id());
  void clear(bool bClearId = true);
  bool isValid() const;
  bool operator ==(const JItem& other) const;
  bool operator !=(const JItem& other) const;
  QString SQL_tableName() const;
  bool SQL_insert_proc(QSqlQuery& query) const;
  bool SQL_update_proc(QSqlQuery& query) const;
  bool SQL_select_proc(QSqlQuery& query, QString& error);
  bool SQL_remove_proc(QSqlQuery& query) const;

  bool SQL_select_formid_proc(QSqlQuery& query) const;

  Form m_form;
  QString m_description;
  QVector<Employee> m_vEmployee;

  QString name() const { return  m_form.m_name; }
  QByteArray image() const { return m_form.m_image.m_image; }
};

#endif // STORE_H
