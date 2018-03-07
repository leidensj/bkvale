#ifndef NOTE_H
#define NOTE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include "jtablecolumn.h"

#define NUMBER_OF_COLUMNS 5
#define INVALID_NOTE_ID -1

enum class NoteColumn : int
{
  Ammount,
  Unity,
  Description,
  Price,
  SubTotal
};

enum class NoteTableIndex : int
{
  ID = 0,
  Number,
  Date,
  Supplier,
  Total,
  Cash
};

struct NoteItem
{
  mutable int m_id;
  double m_ammount;
  double m_price;
  QString m_unity;
  QString m_description;

  NoteItem();
  void clear();
  double subtotal() const { return m_ammount * m_price; }
  static QString st_strSubTotal(double subtotal) { return QString::number(subtotal, 'f', 2); }
  static QString st_strAmmount(double ammount) { return QString::number(ammount, 'f', 3); }
  static QString st_strPrice(double price) { return QString::number(price, 'f', 2); }
  QString strSubtotal() const { return st_strSubTotal(subtotal()); }
  QString strAmmount() const { return st_strAmmount(m_ammount); }
  QString strPrice() const { return st_strPrice(m_price); }
};

struct Note
{
  mutable int m_id;
  qint64 m_date;
  QString m_supplier;
  QVector<NoteItem> m_items;
  double m_total;
  bool m_bCash;

  Note();

  void clear();
  QString strDate() const;
  QString strDayOfWeek() const;
  QString strId() const { return QString::number(m_id); }
  QString strTotal() const { return QString::number(m_total, 'f', 2); }
  static QString strNumber(int number) { return QString::number(number); }
  static bool isValidID(int id) { return id != INVALID_NOTE_ID; }
  static QString getTableName() { return "_PROMISSORYNOTES"; }
  static QVector<JTableColumn> getColumns()
  {
    QVector<JTableColumn> c;
    c.push_back(JTableColumn("_ID", QObject::tr("Id")));
    c.push_back(JTableColumn("_NUMBER", QObject::tr("Número"), false, true));
    c.push_back(JTableColumn("_DATE", QObject::tr("Data"), false));
    c.push_back(JTableColumn("_SUPPLIER", QObject::tr("Fornecedor"), false, false, JResizeMode::Stretch));
    c.push_back(JTableColumn("_TOTAL", QObject::tr("Total"), false));
    c.push_back(JTableColumn("_CASH", QObject::tr("À Vista")));
    return c;
  }
};

#endif // COMMON_H
