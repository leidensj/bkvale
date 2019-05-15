#ifndef NOTE_H
#define NOTE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDate>
#include "supplier.h"
#include "product.h"
#include "defines.h"
#include "jitemsql.h"
#include "employee.h"
#include "payment.h"

enum class NoteColumn : int
{
  Ammount,
  Unity,
  Description,
  Price,
  SubTotal
};

struct NoteItem : public JItem
{
  NoteItem();
  void clear(bool bClearId = true);
  bool isValid() const;
  bool operator !=(const JItem& other) const;
  bool operator ==(const JItem& other) const;
  QString strTableName() const;

  double subtotal() const;
  QString strSubtotal() const;
  QString strAmmount() const;
  QString strPrice() const;

  Product m_product;
  double m_ammount;
  double m_price;
  Package m_package;
};

struct Note : public JItemSQL
{
  Note(Id = Id());
  void clear(bool bClearId = true);
  bool isValid() const;
  bool operator !=(const JItem& other) const;
  bool operator ==(const JItem& other) const;

  QString SQL_tableName() const;
  bool SQL_insert_proc(QSqlQuery& query) const;
  bool SQL_update_proc(QSqlQuery& query) const;
  bool SQL_select_proc(QSqlQuery& query, QString& error);
  bool SQL_remove_proc(QSqlQuery& query) const;
  JModel* SQL_table_model(QObject* parent) const;

  bool SQL_authentication_insert_update() const { return true; }
  bool SQL_authentication_remove() const { return true; }
  static NoteItem SQL_select_last_item(Id supplierId, Id productId);

  void setEmployee(const JItemSQL& e) const;

  QString strDate() const;
  QString strDayOfWeek() const;
  QString strNumber() const;
  QString strDisccount() const;
  double total() const;
  double subTotal() const;
  QString strTotal() const;
  QString strSubTotal() const;
  bool isPaymentOk() const;

  mutable qlonglong m_number;
  QDate m_date;
  Supplier m_supplier;
  Payment m_payment;
  QString m_observation;
  QVector<NoteItem> m_vNoteItem;
  double m_disccount;
  mutable Employee m_employee;
};

#endif // COMMON_H